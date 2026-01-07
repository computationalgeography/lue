#pragma once
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/core/annotate.hpp"
#include "lue/framework/core/component.hpp"
#include "lue/framework/io/dataset.hpp"
#include "lue/framework/io/lue.hpp"
#include "lue/framework/io/util.hpp"
#include "lue/data_model/hl/raster_view.hpp"
#include "lue/data_model/hl/util.hpp"
#include "lue/configure.hpp"


/*!
    @file

    Thiѕ header contains the implementation of the various from_lue function overloads.

    The goal is to return an array as soon as possible, allowing the caller to create more work, while
    the read-tasks start the reading. The idea is that, as soon as partitions have been read, they can
    participate in computations, while other partitions are still being read.
*/


// LUE dataset is an HDF5 file
// LUE array is an HDF5 dataset

// All I/O:
// - Reading from a dataset must happen after any previous calls to writing to that same dataset have
//   finished. See usage of to_lue_order.

// Serial I/O:
// - Notes:
//     - Don't read while a write is still ongoing
//     - Reading from multiple processes at the same time is fine
//     - Parallel I/O requires serializing collective calls to open / close of HDF5 dataset within each
//       process. These need to happen in the same order in all processes. The implementation does the same
//       thing in the serial case.
//     - In the case of serial I/O icw non-threadsafe HDF5, all HDF5 API calls must be done from the same OS
//       thread
//
// - Root process:
//     - Group partitions by process
//     - Obtain from_lue_order to be able to serialize open / close calls to the same dataset in worker
//       processes
//     - Obtain to_lue_order to be able to wait on last to_lue call (if any) to the same dataset
//     - Asynchronously wait on last to_lue call
//     - Spawn a task per process to read into its partitions
//
//  - Worker process:
//     - Obtain a promise to be able to signal any successors once we are done opening the file
//     - Obtain a future representing a previous from_lue open dataset call done by the same process
//     - Obtain a future representing a previous to_lue call (if any) done by the same process
//     - Asynchronously wait on previous to_lue call, previous from_lue open dataset call, and all partitions
//       to read into
//     - Open dataset for reading
//     - Set value of open dataset promise
//     - Read HDF5 dataset into LUE array
//     - Obtain a promise to be able to signal any successors once we are done closing the file
//     - Obtain a future representing a previous from_lue close dataset call done by the same process
//     - Asynchronously wait on previous from_lue close dataset call
//     - Close dataset
//     - Set value of close dataset promise
//
// Parallel I/O:
// - TODO: doc
// - Notes:
//     - Opening and closing a dataset are collective operations. They need to happen in the same order in all
//       processes. See usage of from_lue_order.
//     - Reading from an array can be done independently
// - Root process:
//     - Group partitions by process
//     - Obtain from_lue_order to be able to serialize open / close calls in worker processes
//     - Obtain to_lue_order to be able to wait on last to_lue call (if any) to the same dataset in worker
//       processes
//     - Spawn a task per process to read into its partitions
//
// - Worker process:
//     - ...
//
//


namespace lue {

    namespace detail {

        template<typename Policies, typename CreateHyperslab, typename Partitions>
        void read_partitions(
            [[maybe_unused]] Policies const& policies,
            data_model::Array const& array,
            CreateHyperslab create_hyperslab,
            Partitions const& partitions)
        {
            AnnotateFunction const annotate{"read: partitions"};

            // Open value. Configure for use of parallel I/O if necessary.
            hdf5::Dataset::TransferPropertyList transfer_property_list{};

#ifdef LUE_FRAMEWORK_WITH_PARALLEL_IO
            transfer_property_list.set_transfer_mode(H5FD_MPIO_INDEPENDENT);
#endif

            // Iterate over partitions and read each partition's piece from the dataset
            // Synchronously read all partitions, from the same OS thread

            using Partition = typename Partitions::value_type;
            using Element = ElementT<Partition>;

            hdf5::Datatype const memory_datatype{hdf5::native_datatype<Element>()};

            for (std::size_t partition_idx = 0; partition_idx < std::size(partitions); ++partition_idx)
            {
                Partition const& partition{partitions[partition_idx]};
                lue_hpx_assert(partition.is_ready());

                auto partition_ptr{detail::ready_component_ptr(partition)};
                auto& partition_server{*partition_ptr};
                Element* buffer{partition_server.data().data()};

                array.read(
                    memory_datatype, create_hyperslab(partition_server), transfer_property_list, buffer);

                // TODO Use no-data policy
                // If no-data in the dataset, write no-data to the partition
            }
        }


        template<typename Policies, typename Partitions>
        auto read_partitions_constant(
            Policies const& policies,
            std::string const& array_pathname,
            Count const from_lue_order,
            Count const to_lue_order,
            hdf5::Offset const& array_hyperslab_start,  // Only needed to offset block read from array
            data_model::ID const object_id,
            Partitions const& partitions) -> hpx::future<Partitions>
        {
            // hpx::cout << std::format("DEBUG: from_lue/open {} (start trying)\n", from_lue_order)
            //           << std::flush;

            using Partition = typename Partitions::value_type;
            using PartitionServer = Partition::Server;

            auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
                parse_array_pathname(array_pathname);

            std::filesystem::path const dataset_path{normalize(dataset_pathname)};
            hpx::promise<void> from_lue_open_dataset_p =
                from_lue_open_dataset_promise_for(dataset_path, from_lue_order);
            hpx::shared_future<void> from_lue_open_dataset_when_predecessor_done_f =
                from_lue_open_dataset_when_predecessor_done(dataset_path, from_lue_order);

            // TODO: Only needed in parallel I/O case?
            // if (to_lue_order > 0)
            // {
            //     // Due to scheduling of tasks we may get here before a previous call to to_lue has even
            //     // started
            //     while (!to_lue_close_dataset_done_available(dataset_path, to_lue_order))
            //     {
            //         // hpx::cout << std::format(
            //         //                  "DEBUG: from_lue/open {} (sleep for {})\n", from_lue_order,
            //         //                  to_lue_order)
            //         //           << std::flush;
            //         hpx::this_thread::sleep_for(std::chrono::seconds(1));
            //     }
            // }

            // TODO: This may not be true in case of parallel I/O(?) Is it required to be true in that case?
            lue_hpx_assert(
                to_lue_order == 0 || to_lue_close_dataset_done_available(dataset_path, to_lue_order));
            hpx::shared_future<void> to_lue_close_dataset_done_f =
                to_lue_close_dataset_done(dataset_path, to_lue_order);
            // TODO: This is not the case in case of parallel I/O, right?
            lue_hpx_assert(to_lue_close_dataset_done_f.is_ready());

            hpx::future<Partitions> partitions_f = hpx::dataflow(
                hpx::launch::async,
                [policies,
                 array_hyperslab_start,
                 dataset_path,
                 from_lue_order,
                 from_lue_open_dataset_p = std::move(from_lue_open_dataset_p),
                 phenomenon_name,
                 property_set_name,
                 property_name,
                 object_id](
                    [[maybe_unused]] auto const& to_lue_close_dataset_done_f,
                    [[maybe_unused]] auto const& from_lue_open_dataset_when_predecessor_done_f,
                    auto&& partitions_f) mutable -> hpx::future<Partitions>
                {
                    AnnotateFunction const annotate{"from_lue: partitions constant"};

                    // hpx::cout << std::format("DEBUG: from_lue/open {}\n", from_lue_order) << std::flush;

                    Partitions partitions = partitions_f.get();

                    auto dataset = open_dataset(dataset_path.string(), H5F_ACC_RDONLY);

                    // Done with the collective call
                    from_lue_open_dataset_p.set_value();

                    // Open phenomenon
                    auto const& phenomenon{dataset.phenomena()[phenomenon_name]};

                    // Open property-set
                    auto const& property_set{phenomenon.property_sets()[property_set_name]};

                    // Open property
                    lue_hpx_assert(property_set.properties().contains(property_name));
                    lue_hpx_assert(
                        property_set.properties().shape_per_object(property_name) ==
                        data_model::ShapePerObject::different);
                    lue_hpx_assert(
                        property_set.properties().value_variability(property_name) ==
                        data_model::ValueVariability::constant);
                    using Properties = data_model::different_shape::Properties;
                    auto const& property{property_set.properties().collection<Properties>()[property_name]};

                    // constant: data_model::different_shape::Value
                    auto const& value{property.value()};

                    // constant: data_model::Array: hdf5::Dataset
                    auto array{value[object_id]};

                    auto create_hyperslab =
                        [array_hyperslab_start](PartitionServer const& partition_server) -> auto
                    { return hyperslab(array_hyperslab_start, partition_server); };

                    // Synchronous
                    read_partitions(policies, array, create_hyperslab, partitions);

                    // Closing a dataset is a collective operation: only close the dataset (let it go out
                    // of scope) when it is our turn to do so.
                    hpx::promise<void> from_lue_close_dataset_p =
                        from_lue_close_dataset_promise_for(dataset_path, from_lue_order);
                    hpx::shared_future<void> from_lue_close_dataset_predecessor_f =
                        from_lue_close_dataset_when_predecessor_done(dataset_path, from_lue_order);

                    return from_lue_close_dataset_predecessor_f.then(
                        ternary_if<serial_io_non_thread_safe>(hpx::launch::sync, hpx::launch::async),
                        [dataset = std::move(dataset),
                         from_lue_close_dataset_p = std::move(from_lue_close_dataset_p),
                         partitions = std::move(partitions)](
                            [[maybe_unused]] auto const& close_for_read_file_predecessor_f) mutable
                            -> Partitions
                        {
                            // The dataset must go out of scope before we set the promise's value
                            [](auto&& dataset) -> auto { HPX_UNUSED(dataset); }(std::move(dataset));
                            from_lue_close_dataset_p.set_value();
                            return partitions;
                        });
                },
                to_lue_close_dataset_done_f,
                from_lue_open_dataset_when_predecessor_done_f,
                hpx::when_all(partitions));

            return partitions_f;
        }


        /*!
            @brief      .
            @tparam     .
            @param      .
            @return     .
            @exception  .
        */
        template<typename Policies, typename Partitions>
        auto read_partitions_variable(
            Policies const& policies,
            std::string const& array_pathname,
            Count const from_lue_order,
            Count const to_lue_order,
            hdf5::Offset const& array_hyperslab_start,
            data_model::ID const object_id,
            Index const time_step_idx,
            Partitions const& partitions) -> hpx::future<std::tuple<Partitions, hpx::future<void>>>
        {
            print_debug("from_lue/open {} (start trying)", from_lue_order);

            using Partition = typename Partitions::value_type;
            using PartitionServer = Partition::Server;

            auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
                parse_array_pathname(array_pathname);

            std::filesystem::path const dataset_path{normalize(dataset_pathname)};
            hpx::promise<void> from_lue_open_dataset_p =
                from_lue_open_dataset_promise_for(dataset_path, from_lue_order);
            hpx::shared_future<void> from_lue_open_dataset_when_predecessor_done_f =
                from_lue_open_dataset_when_predecessor_done(dataset_path, from_lue_order);

            // TODO: Only needed in parallel I/O case?
            // if (to_lue_order > 0)
            // {
            //     // Due to scheduling of tasks we may get here before a previous call to to_lue has even
            //     // started
            //     while (!to_lue_close_dataset_done_available(dataset_path, to_lue_order))
            //     {
            //         hpx::cout << std::format(
            //                          "DEBUG: from_lue/open {} (sleep for {})\n", from_lue_order,
            //                          to_lue_order)
            //                   << std::flush;
            //         hpx::this_thread::sleep_for(std::chrono::seconds(1));
            //     }
            // }

            hpx::future<void> precondition_f{};

            // NOTE: In serial non-threadsafe, reading can happen from different OS threads, as long as it
            //       doesn't happen concurrently

            if constexpr (serial_io)
            {
                // Serial I/O: we can only start reading once everybody is done writing to the dataset. This
                // is the case if:
                //
                // - Any previous to_lue call has finished (to_lue_order)
                //     - This is handled in the root locality and must therefore be true already
                //     - We can't assert that here. This info is only available on the root locality.

#ifndef NDEBUG
                // lue_hpx_assert(
                //     to_lue_order == 0 || to_lue_close_dataset_done_available(dataset_path, to_lue_order));
                hpx::shared_future<void> to_lue_close_dataset_done_f =
                    to_lue_close_dataset_done(dataset_path, to_lue_order);
                lue_hpx_assert(to_lue_close_dataset_done_f.is_ready());
#endif

                if constexpr (serial_io_non_thread_safe)
                {
                    // In case of non-threadsafe I/O, a previous read must have finished, to prevent I/O from
                    // different I/O threads. This is handled at the root locality.
                    lue_hpx_assert(from_lue_open_dataset_when_predecessor_done_f.is_ready());

                    precondition_f = hpx::make_ready_future();
                }

                if constexpr (serial_io_thread_safe)
                {
                    // Within this process, serialize open dataset calls of subsequent calls to from_lue
                    precondition_f = hpx::when_all(from_lue_open_dataset_when_predecessor_done_f);
                }
            }

            if constexpr (parallel_io)
            {
                // from_lue_open_dataset_when_predecessor_done_f
            }

            return hpx::dataflow(
                hpx::launch::async,
                [policies,
                 array_hyperslab_start,
                 dataset_path,
                 from_lue_order,
                 from_lue_open_dataset_p = std::move(from_lue_open_dataset_p),
                 phenomenon_name,
                 property_set_name,
                 property_name,
                 object_id,
                 time_step_idx]([[maybe_unused]] auto const& precondition_f, auto&& partitions_f) mutable
                    -> std::tuple<Partitions, hpx::future<void>>
                {
                    AnnotateFunction const annotate{"from_lue: partitions variable"};

                    print_debug("from_lue/open {}", from_lue_order);

                    Partitions partitions = partitions_f.get();

                    auto dataset = open_dataset(dataset_path.string(), H5F_ACC_RDONLY);

                    // Done with the collective call
                    from_lue_open_dataset_p.set_value();

                    {
                        // Find the array to read from (the HDF5 dataset)
                        auto const& phenomenon{dataset.phenomena()[phenomenon_name]};
                        auto const& property_set{phenomenon.property_sets()[property_set_name]};
                        lue_hpx_assert(property_set.properties().contains(property_name));
                        lue_hpx_assert(
                            property_set.properties().shape_per_object(property_name) ==
                            data_model::ShapePerObject::different);
                        lue_hpx_assert(
                            property_set.properties().value_variability(property_name) ==
                            data_model::ValueVariability::variable);
                        lue_hpx_assert(
                            property_set.properties().shape_variability(property_name) ==
                            data_model::ShapeVariability::constant);
                        using Properties = data_model::different_shape::constant_shape::Properties;
                        auto const& property{
                            property_set.properties().collection<Properties>()[property_name]};
                        // constant: data_model::different_shape::Value
                        auto const& value{property.value()};
                        // constant: data_model::Array: hdf5::Dataset
                        auto const array{value[object_id]};

                        auto create_hyperslab = [array_hyperslab_start, time_step_idx](
                                                    PartitionServer const& partition_server) -> auto
                        { return hyperslab(array_hyperslab_start, partition_server, 0, time_step_idx); };

                        // Synchronous
                        read_partitions(policies, array, create_hyperslab, partitions);
                    }

                    // Closing a dataset is a collective operation: only close the dataset (let it go out
                    // of scope) when it is our turn to do so.
                    hpx::promise<void> from_lue_close_dataset_p =
                        from_lue_close_dataset_promise_for(dataset_path, from_lue_order);
                    hpx::shared_future<void> from_lue_close_dataset_predecessor_f =
                        from_lue_close_dataset_when_predecessor_done(dataset_path, from_lue_order);

                    if constexpr (serial_io_non_thread_safe)
                    {
                        // No reason to go async. Any predecessor is done already.
                        lue_hpx_assert(from_lue_close_dataset_predecessor_f.is_ready());

                        // The dataset must go out of scope before we set the promise's value
                        [](auto&& dataset) -> auto
                        {
                            HPX_UNUSED(dataset);
                            dataset.flush();
                        }(std::move(dataset));

                        // from_lue_close_dataset_promise_for(dataset_path, from_lue_order).set_value();
                        from_lue_close_dataset_p.set_value();

                        print_debug("from_lue/close {}", from_lue_order);

                        return {std::move(partitions), hpx::make_ready_future()};
                    }

                    if constexpr (!serial_io_non_thread_safe)
                    {
                        return {
                            std::move(partitions),
                            from_lue_close_dataset_predecessor_f.then(
                                hpx::launch::async,
                                [dataset = std::move(dataset),
                                 from_lue_order,
                                 from_lue_close_dataset_p = std::move(from_lue_close_dataset_p)](
                                    [[maybe_unused]] auto const& close_for_read_file_predecessor_f) mutable
                                    -> auto
                                {
                                    // The dataset must go out of scope before we set the promise's value
                                    [](auto&& dataset) -> auto
                                    {
                                        HPX_UNUSED(dataset);
                                        dataset.flush();
                                    }(std::move(dataset));

                                    // from_lue_close_dataset_promise_for(dataset_path, from_lue_order)
                                    //     .set_value();
                                    from_lue_close_dataset_p.set_value();

                                    print_debug("from_lue/close {}", from_lue_order);
                                })};
                    }
                },
                precondition_f,
                hpx::when_all(partitions));

            // return {std::move(partitions_f), std::move(dataset_closed_f)};
        };


        template<typename Policies, typename Partitions>
        struct ReadPartitionsConstantAction:
            hpx::actions::make_action<
                decltype(&read_partitions_constant<Policies, Partitions>),
                &read_partitions_constant<Policies, Partitions>,
                ReadPartitionsConstantAction<Policies, Partitions>>::type
        {
        };


        template<typename Policies, typename Partitions>
        struct ReadPartitionsVariableAction:
            hpx::actions::make_action<
                decltype(&read_partitions_variable<Policies, Partitions>),
                &read_partitions_variable<Policies, Partitions>,
                ReadPartitionsVariableAction<Policies, Partitions>>::type
        {
        };


        template<typename Policies, Rank rank>
        auto from_lue(
            Policies const& policies,
            std::string const& array_pathname,
            hdf5::Offset const& array_hyperslab_start,
            data_model::ID const object_id,
            PartitionedArray<policy::OutputElementT<Policies>, rank> array)
            -> PartitionedArray<policy::OutputElementT<Policies>, rank>
        {
            using Element = policy::OutputElementT<Policies>;
            using Array = PartitionedArray<Element, rank>;
            using Partition = PartitionT<Array>;
            using Action = ReadPartitionsConstantAction<Policies, std::vector<Partition>>;

            // Partitions and localities
            auto const partition_idxs_by_locality{detail::partition_idxs_by_locality(array)};

            Action action{};

            auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
                parse_array_pathname(array_pathname);
            auto const dataset_path{detail::normalize(dataset_pathname)};

            // Dependencies
            auto const from_lue_order = detail::from_lue_order(dataset_path);
            auto const to_lue_order = detail::current_to_lue_order(dataset_path);

#ifndef LUE_FRAMEWORK_WITH_PARALLEL_IO
            // Make read dependent on any previous write to the same dataset. This assumes all to_lue
            // calls are serialized so waiting on the latest one is enough.

            hpx::shared_future<void> dataset_closed_f{};

            if constexpr (detail::serial_io_non_thread_safe)
            {
                // When reading multiple times from the same dataset, these reads must be serialized.
                // Otherwise multiple reads may start in parallel and from different threads.
                dataset_closed_f = hpx::when_all(
                                       detail::from_lue_finished(dataset_path, from_lue_order - 1),
                                       detail::to_lue_finished(dataset_path, to_lue_order))
                                       .share();
            }

            if constexpr (detail::serial_io_thread_safe)
            {
                dataset_closed_f = detail::to_lue_finished(dataset_path, to_lue_order);
            }

            lue_hpx_assert(dataset_closed_f.valid());
#endif

            // Iterate over all grouped partitions
            for (auto const& [locality, partition_idxs] : partition_idxs_by_locality)
            {
                // Move current partitions from input array into new collection
                std::vector<Partition> partitions(partition_idxs.size());

                for (std::size_t idx = 0; auto const partition_idx : partition_idxs)
                {
                    partitions[idx++] = std::move(array.partitions()[partition_idx]);
                }

                // Spawn a task that reads from the dataset into the current partitions. This returns a
                // collection of futures to partitions, each of which becomes ready once its data is read.
                std::vector<hpx::future<Partition>> partition_fs{};

                if constexpr (serial_io)
                {
                    partition_fs = hpx::split_future<Partition>(
                        dataset_closed_f.then(
                            [action,
                             locality,
                             policies,
                             array_pathname,
                             from_lue_order,
                             to_lue_order,
                             array_hyperslab_start,
                             object_id,
                             partitions =
                                 std::move(partitions)]([[maybe_unused]] auto const& dataset_closed_f) -> auto
                            {
                                return hpx::async(
                                    action,
                                    locality,
                                    policies,
                                    array_pathname,
                                    from_lue_order,
                                    to_lue_order,
                                    array_hyperslab_start,
                                    object_id,
                                    std::move(partitions));
                            }),
                        std::size(partition_idxs));
                }
                else
                {
                    partition_fs = hpx::split_future<Partition>(
                        hpx::async(
                            action,
                            locality,
                            policies,
                            array_pathname,
                            from_lue_order,
                            to_lue_order,
                            array_hyperslab_start,
                            object_id,
                            std::move(partitions)),
                        std::size(partition_idxs));
                }

                // Iterate over each selected partition
                for (std::size_t idx = 0; auto const partition_idx : partition_idxs)
                {
                    // Replace the current partition with a new one which becomes ready once the reading
                    // has finished. Under the hood the "new" partition is the same and the original one,
                    // but this is not relevant for the caller.
                    array.partitions()[partition_idx] = partition_fs[idx++].then(
                        [](auto&& partition_f) -> auto
                        {
                            lue_hpx_assert(partition_f.valid());
                            lue_hpx_assert(partition_f.is_ready());

                            Partition partition{partition_f.get()};

                            lue_hpx_assert(partition.valid());

                            return partition;
                        });
                }
            }

            lue_hpx_assert(
                std::all_of(
                    array.partitions().begin(),
                    array.partitions().end(),
                    [](auto const& partition) -> auto { return partition.valid(); }));

            if constexpr (serial_io)
            {
                detail::add_from_lue_finished(
                    dataset_path,
                    from_lue_order,
                    hpx::when_all(array.partitions().begin(), array.partitions().end()).share());
            }

            return array;
        }


        /*!
            @brief      Read array from dataset into @a array
            @return     The array passed in
        */
        template<typename Policies, Rank rank>
        auto from_lue(
            Policies const& policies,
            std::string const& array_pathname,
            hdf5::Offset const& array_hyperslab_start,
            data_model::ID const object_id,
            Index const time_step_idx,
            PartitionedArray<policy::OutputElementT<Policies>, rank> array)
            -> PartitionedArray<policy::OutputElementT<Policies>, rank>
        {
            AnnotateFunction const annotate{"from_lue"};

            using Element = policy::OutputElementT<Policies>;
            using Array = PartitionedArray<Element, rank>;
            using Partition = PartitionT<Array>;
            using Action = ReadPartitionsVariableAction<Policies, std::vector<Partition>>;

            // Partitions and localities
            auto const partition_idxs_by_locality{detail::partition_idxs_by_locality(array)};

            auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
                parse_array_pathname(array_pathname);
            auto const dataset_path{detail::normalize(dataset_pathname)};

            // Dependencies
            auto const from_lue_order = detail::from_lue_order(dataset_path);
            auto const to_lue_order = detail::current_to_lue_order(dataset_path);

#ifndef LUE_FRAMEWORK_WITH_PARALLEL_IO
            // Make read dependent on any previous write to the same dataset. This requires all to_lue
            // calls to be serialized so waiting on the latest one is enough.

            hpx::shared_future<void> precondition_f{};

            if constexpr (detail::serial_io_non_thread_safe)
            {
                // TODO: Revisit, given that some API calls can happen in parallel from multiple OS threads,
                //       it seems(?)

                // - When reading multiple times from the same dataset, these reads must be serialized and
                //   synchronized. Otherwise multiple reads may start in parallel and from different threads.
                // - Before reading the dataset, any write must have finished first
                precondition_f = hpx::when_all(
                                     detail::from_lue_finished(dataset_path, from_lue_order - 1),
                                     detail::to_lue_finished(dataset_path, to_lue_order))
                                     .share();
            }

            if constexpr (detail::serial_io_thread_safe)
            {
                // Before reading the dataset, any write must have finished first
                precondition_f = detail::to_lue_finished(dataset_path, to_lue_order);
            }

            lue_hpx_assert(precondition_f.valid());
#endif

            // Collection of futures, each of which becomes ready once the reading task has finished its work
            // and has closed the dataset again
            std::vector<hpx::future<void>> dataset_closed_fs{};
            dataset_closed_fs.reserve(partition_idxs_by_locality.size());

            Action action{};

            // Iterate over all grouped partitions
            for (auto const& [locality, partition_idxs] : partition_idxs_by_locality)
            {
                // Move current partitions from input array into new collection
                std::vector<Partition> partitions(partition_idxs.size());

                for (std::size_t idx = 0; auto const partition_idx : partition_idxs)
                {
                    partitions[idx++] = std::move(array.partitions()[partition_idx]);
                }

                // Spawn a task that reads from the dataset into the current partitions. This returns a
                // collection of futures to partitions, each of which becomes ready once its data is read.
                // Also, a future is returned which becomes ready once the dataset has been closed. This
                // happens later than when the partitions have become ready.
                std::vector<hpx::future<Partition>> partition_fs{};

                if constexpr (serial_io)
                {
                    // hpx::future<void> dataset_closed_f{};
                    // hpx::future<std::vector<Partition>> partitions_f{};

                    // std::tie(partitions_f, dataset_closed_f) = hpx::split_future(precondition_f.then(
                    auto [partitions_f, dataset_closed_f] = hpx::split_future(precondition_f.then(
                        [action,
                         locality,
                         policies,
                         array_pathname,
                         from_lue_order,
                         to_lue_order,
                         array_hyperslab_start,
                         object_id,
                         time_step_idx,
                         partitions = std::move(partitions)]([[maybe_unused]] auto const& precondition_f)
                            -> hpx::future<std::tuple<std::vector<Partition>, hpx::future<void>>>
                        {
                            return hpx::async(
                                action,
                                locality,
                                policies,
                                array_pathname,
                                from_lue_order,
                                to_lue_order,
                                array_hyperslab_start,
                                object_id,
                                time_step_idx,
                                std::move(partitions));
                        }));
                    partition_fs =
                        hpx::split_future<Partition>(std::move(partitions_f), std::size(partition_idxs));
                    dataset_closed_fs.push_back(std::move(dataset_closed_f));
                }

                if constexpr (parallel_io)
                {
                    partition_fs = hpx::split_future<Partition>(
                        hpx::async(
                            action,
                            locality,
                            policies,
                            array_pathname,
                            from_lue_order,
                            to_lue_order,
                            array_hyperslab_start,
                            object_id,
                            time_step_idx,
                            std::move(partitions)),
                        std::size(partition_idxs));
                }

                lue_hpx_assert(partition_fs.size() == partition_idxs.size());

                // Iterate over each selected partition
                for (std::size_t idx = 0; auto const partition_idx : partition_idxs)
                {
                    // Replace the current partition with a new one which becomes ready once the reading
                    // has finished. Under the hood the "new" partition is the same and the original one,
                    // but this is not relevant for the caller.
                    array.partitions()[partition_idx] = partition_fs[idx++].then(
                        [](auto&& partition_f) -> auto
                        {
                            lue_hpx_assert(partition_f.valid());
                            lue_hpx_assert(partition_f.is_ready());

                            Partition partition{partition_f.get()};

                            lue_hpx_assert(partition.valid());

                            return partition;
                        });
                }
            }

            lue_hpx_assert(
                std::all_of(
                    array.partitions().begin(),
                    array.partitions().end(),
                    [](auto const& partition) -> auto { return partition.valid(); }));

            if constexpr (serial_io)
            {
                detail::add_from_lue_finished(
                    dataset_path, from_lue_order, hpx::when_all(dataset_closed_fs).share());
            }

            return array;
        }


        template<typename Shape>
        auto constant_array_shape(std::string const& array_pathname) -> Shape
        {
            auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
                parse_array_pathname(array_pathname);

            namespace lh5 = lue::hdf5;
            namespace ldm = lue::data_model;

            using DatasetPtr = std::shared_ptr<ldm::Dataset>;
            using RasterView = ldm::constant::RasterView<DatasetPtr>;

            auto input_dataset_ptr =
                std::make_shared<ldm::Dataset>(ldm::open_dataset(dataset_pathname, H5F_ACC_RDONLY));

            lh5::Shape grid_shape{};

            // Open a view on the raster
            RasterView input_raster_view{input_dataset_ptr, phenomenon_name, property_set_name};

            grid_shape = lh5::Shape{input_raster_view.grid_shape()};

            lue_hpx_assert(rank<Shape> == 2);  // TODO(KDJ)

            return Shape{
                {static_cast<typename Shape::value_type>(grid_shape[0]),
                 static_cast<typename Shape::value_type>(grid_shape[1])}};
        }


        template<typename Shape>
        auto variable_array_shape(std::string const& array_pathname) -> Shape
        {
            auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
                parse_array_pathname(array_pathname);

            namespace lh5 = lue::hdf5;
            namespace ldm = lue::data_model;

            using DatasetPtr = std::shared_ptr<ldm::Dataset>;
            using RasterView = ldm::variable::RasterView<DatasetPtr>;

            auto input_dataset_ptr =
                std::make_shared<ldm::Dataset>(ldm::open_dataset(dataset_pathname, H5F_ACC_RDONLY));

            lh5::Shape grid_shape{};

            // Open a view on the raster
            RasterView input_raster_view{input_dataset_ptr, phenomenon_name, property_set_name};

            grid_shape = lh5::Shape{input_raster_view.grid_shape()};

            lue_hpx_assert(rank<Shape> == 2);  // TODO(KDJ)

            return Shape{
                {static_cast<typename Shape::value_type>(grid_shape[0]),
                 static_cast<typename Shape::value_type>(grid_shape[1])}};
        }

    }  // namespace detail


    namespace policy::from_lue {

        /*!
            @brief      Default policies
            @tparam     OutputElement Type of output elements
        */
        template<typename OutputElement>
        using DefaultPolicies =
            policy::DefaultPolicies<AllValuesWithinDomain<>, OutputElements<OutputElement>, InputElements<>>;


        /*!
            @brief      Default value policies
            @tparam     OutputElement Type of output elements
        */
        template<typename OutputElement>
        using DefaultValuePolicies = policy::
            DefaultValuePolicies<AllValuesWithinDomain<>, OutputElements<OutputElement>, InputElements<>>;

    }  // namespace policy::from_lue


    /*!
        @brief      Read a hyperslab from an array from a LUE dataset
        @tparam     Policies Policies type
        @tparam     Shape Shape type
        @param      policies Policies to use
        @param      array_pathname Pathname of the property to read from, formatted as
                    `<dataset_pathname>/<phenomenon_name>/<property_set_name>/<property_name>`
        @param      hyperslab Hyperslab to read from the array
        @param      partition_shape Shape of the array partitions to use
        @param      object_id ID of object whose property value to read
        @return     New array
    */
    template<typename Policies, typename Shape>
    auto from_lue(
        Policies const& policies,
        std::string const& array_pathname,
        hdf5::Hyperslab const& hyperslab,
        Shape const& partition_shape,
        data_model::ID const object_id) -> PartitionedArray<policy::OutputElementT<Policies>, rank<Shape>>
    {
        using Element = policy::OutputElementT<Policies>;
        using Array = PartitionedArray<Element, rank<Shape>>;
        using Functor = InstantiateDefaultInitialized<Element, rank<Shape>>;

        Shape const array_shape{detail::hyperslab_to_shape<Shape>(hyperslab)};
        Array array{create_partitioned_array(policies, array_shape, partition_shape, Functor{})};

        return detail::from_lue(policies, array_pathname, hyperslab.start(), object_id, std::move(array));
    }


    /*!
        @overload

        Default policies will be used.
    */
    template<typename Element, typename Shape>
    auto from_lue(
        std::string const& array_pathname,
        hdf5::Hyperslab const& hyperslab,
        Shape const& partition_shape,
        data_model::ID const object_id) -> PartitionedArray<Element, rank<Shape>>
    {
        using Policies = policy::from_lue::DefaultPolicies<Element>;

        return from_lue(Policies{}, array_pathname, hyperslab, partition_shape, object_id);
    }


    /*!
        @overload

        The whole array will be read.
    */
    template<typename Policies, typename Shape>
    auto from_lue(
        Policies const& policies,
        std::string const& array_pathname,
        Shape const& partition_shape,
        data_model::ID const object_id) -> PartitionedArray<policy::OutputElementT<Policies>, rank<Shape>>
    {
        Shape const array_shape{detail::constant_array_shape<Shape>(array_pathname)};

        return from_lue(
            policies, array_pathname, detail::shape_to_hyperslab(array_shape), partition_shape, object_id);
    }


    /*!
        @overload

        Default policies will be used and the whole array will be read.
    */
    template<typename Element, typename Shape>
    auto from_lue(
        std::string const& array_pathname, Shape const& partition_shape, data_model::ID const object_id)
        -> PartitionedArray<Element, rank<Shape>>
    {
        using Policies = policy::from_lue::DefaultPolicies<Element>;

        return from_lue(Policies{}, array_pathname, partition_shape, object_id);
    }


    // -------------------------------------------------------------------------


    /*!
        @brief      Read a hyperslab from a temporal array from a LUE dataset
        @tparam     Policies Policies type
        @tparam     Shape Shape type
        @param      policies Policies to use
        @param      array_pathname Pathname of the property to read from, formatted as
                    `<dataset_pathname>/<phenomenon_name>/<property_set_name>/<property_name>`
        @param      hyperslab Hyperslab to read from the array
        @param      partition_shape Shape of the array partitions to use
        @param      object_id ID of object whose property value to read
        @param      time_step_idx Index of time step to read
        @return     New array
    */
    template<typename Policies, typename Shape>
    auto from_lue(
        Policies const& policies,
        std::string const& array_pathname,
        hdf5::Hyperslab const& hyperslab,
        Shape const& partition_shape,
        data_model::ID const object_id,
        Index const time_step_idx) -> PartitionedArray<policy::OutputElementT<Policies>, rank<Shape>>
    {
        using Element = policy::OutputElementT<Policies>;
        using Array = PartitionedArray<Element, rank<Shape>>;
        using Functor = InstantiateDefaultInitialized<Element, rank<Shape>>;

        Shape const array_shape{detail::hyperslab_to_shape<Shape>(hyperslab)};
        Array array{create_partitioned_array(policies, array_shape, partition_shape, Functor{})};

        return detail::from_lue(
            policies, array_pathname, hyperslab.start(), object_id, time_step_idx, std::move(array));
    }


    /*!
        @overload

        Default policies will be used.
    */
    template<typename Element, typename Shape>
    auto from_lue(
        std::string const& array_pathname,
        hdf5::Hyperslab const& hyperslab,
        Shape const& partition_shape,
        data_model::ID const object_id,
        Index const time_step_idx) -> PartitionedArray<Element, rank<Shape>>
    {
        using Policies = policy::from_lue::DefaultPolicies<Element>;

        return from_lue(Policies{}, array_pathname, hyperslab, partition_shape, object_id, time_step_idx);
    }


    /*!
        @overload

        The whole array will be read.
    */
    template<typename Policies, typename Shape>
    auto from_lue(
        Policies const& policies,
        std::string const& array_pathname,
        Shape const& partition_shape,
        data_model::ID const object_id,
        Index const time_step_idx) -> PartitionedArray<policy::OutputElementT<Policies>, rank<Shape>>
    {
        Shape const array_shape{detail::variable_array_shape<Shape>(array_pathname)};

        return from_lue(
            policies,
            array_pathname,
            detail::shape_to_hyperslab(array_shape),
            partition_shape,
            object_id,
            time_step_idx);
    }


    /*!
        @overload

        Default policies will be used and the whole array will be read.
    */
    template<typename Element, typename Shape>
    auto from_lue(
        std::string const& array_pathname,
        Shape const& partition_shape,
        data_model::ID const object_id,
        Index const time_step_idx) -> PartitionedArray<Element, rank<Shape>>
    {
        using Policies = policy::from_lue::DefaultPolicies<Element>;

        return from_lue(Policies{}, array_pathname, partition_shape, object_id, time_step_idx);
    }

}  // namespace lue
