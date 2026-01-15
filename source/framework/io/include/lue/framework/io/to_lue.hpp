#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/core/annotate.hpp"
#include "lue/framework/core/component.hpp"
#include "lue/framework/io/dataset.hpp"
#include "lue/framework/io/lue.hpp"
#include "lue/framework/io/util.hpp"
#include "lue/data_model/hl/util.hpp"
#include "lue/configure.hpp"


/*!
    @file

    Thiѕ header contains the implementation of the various to_lue function overloads.

    The goal is to return an array as soon as possible, allowing the caller to create more work, while
    the write-tasks start the writing.
*/


// Terminology:
// - A LUE dataset is represented by a HDF5 file
// - A LUE array is represented by a HDF5 dataset

// All I/O:
// - Writing to a dataset must happen after any previous calls to writing or reading to that same dataset
//   have finished. See usage of to_lue_order and from_lue_order.

// Serial I/O:
// - Notes:
//     - Don't write while a read is still ongoing
//     - Don't write from multiple processes at the same time
//     - When the last read is done, we can assume that no reading is ongoing anymore (dataset is closed / not
//       locked)
//     - When a previous write is done, we can assume that no writing is ongoing anymore (dataset is closed /
//       not locked)
//     - In the case of serial I/O icw non-threadsafe HDF5, all HDF5 API calls must be done from the same OS
//       thread
//
// Root process:
//     - Group partitions by process
//     - Obtain to_lue_order to be able to serialize open / close calls to the same dataset in worker
//       processes
//     - Obtain from_lue_order to be able to wait on last from_lue call (if any) to the same dataset
//     - Asynchronously wait on previous to_lue_call and last from_lue call to finish
//     - Spawn a task per process to write from its partitions. Only spawn a new task once the previous write
//       has finished (the dataset is closed again)
//
// Worker process:
//     - Obtain a promise to be able to signal any successors once we are done opening the file
//     - Obtain a future representing a previous to_lue open dataset call done by the same process
//     - Obtain a future representing a previous from_lue close dataset call (if any) done by the same process
//     - Asynchronously wait on previous from_lue close dataset call, previous to_lue open dataset call, and
//       one of the partitions to become ready
//     - Open dataset for writing
//     - Set value of open dataset promise
//     - Asynchronously write each partition individually to the dataset, once it becomes ready and the task
//       gets scheduled.
//     - Asynchronously wait for all partitions to be written
//     - Obtain a promise to be able to signal any successors once we are done closing the file
//     - Obtain a future representing a previous to_lue close dataset call done by the same process
//     - Asynchronously wait on previous to_lue close dataset call
//     - Close dataset
//     - Set value of close dataset promise
//
//
//
//
//
// - No two processes can have the same dataset open for writing
// - HDF5 is threadsafe:
//     - Writing to an array can be done concurrently. Any serialization happens in the HDF5 library.
// - HDF5 is not threadsafe:
//     - Writing to an array must be done serialized.
//
// Parallel I/O:
// - Notes:
//     - ...
//
// Root process:
//     - ...
//
// Worker process:
//     - ...
//
//
// - Opening and closing a LUE dataset (HDF5 file) are collective operations. They need to happen in the same
//   order in all processes. See usage of to_lue_order.
// - Writing to a LUE array (HDF5 dataset) can be done independently


namespace lue {
    namespace detail {

        template<typename Policies, typename Partition, typename CreateHyperslab>
        void write_partition(
            [[maybe_unused]] Policies const& policies,
            Partition const& partition,
            CreateHyperslab create_hyperslab,
            data_model::Array& array)
        {
            AnnotateFunction const annotate{"to_lue: partition"};

            // Open value. Configure for use of parallel I/O if necessary.
            hdf5::Dataset::TransferPropertyList transfer_property_list{};

#ifdef LUE_FRAMEWORK_WITH_PARALLEL_IO
            transfer_property_list.set_transfer_mode(H5FD_MPIO_INDEPENDENT);
#endif

            using Element = ElementT<Partition>;

            auto partition_ptr{detail::ready_component_ptr(partition)};
            auto& partition_server{*partition_ptr};
            Element* buffer{partition_server.data().data()};

            array.write(create_hyperslab(partition_server), transfer_property_list, buffer);
        }


        template<typename Policies, typename Partitions, typename CreateHyperslab>
        void write_partitions(
            Policies const& policies,
            Partitions const& partitions,
            CreateHyperslab create_hyperslab,
            data_model::Array& array)
        {
            // This function blocks until all partitions have been written. This is required because local
            // variables are passed into the function that does the writing and which is called from an
            // asynchronous task.
            using Partition = typename Partitions::value_type;

            auto write_partition = detail::write_partition<
                std::remove_reference_t<std::remove_cv_t<Policies>>,
                std::remove_reference_t<std::remove_cv_t<Partition>>,
                std::remove_reference_t<std::remove_cv_t<decltype((create_hyperslab))>>>;

#if defined(LUE_FRAMEWORK_WITH_PARALLEL_IO) || defined(HDF5_IS_THREADSAFE)
            // Asynchronously write all partitions. Let HDF5 figure out how to efficiently do this.

            std::vector<hpx::future<void>> partitions_written{};
            partitions_written.reserve(partitions.size());

            for (auto& partition : partitions)
            {
                // Pass in references to objects. This is fine, since we are waiting after
                // the loop. Local variables will not go out of scope too soon.
                partitions_written.emplace_back(partition.then(
                    [write_partition, &policies, create_hyperslab, &array](auto const& partition) -> auto
                    { write_partition(policies, partition, create_hyperslab, array); }));
            }

            // Don't return before the writing has finished
            hpx::wait_all(partitions_written);
#else
            // Synchronously write all partitions, from the same OS thread
            // TODO: I don't think we can be certain this task keeps running on the same OS thread
            //       We may have to wait on all partitions first and then to the write in one go,
            //       from the thread we are scheduled to run on. Could be an OS thread as well.
            // auto remaining_partitions = partitions;

            // hpx::wait_all(remaining_partitions);

            lue_hpx_assert(
                std::all_of(
                    partitions.begin(),
                    partitions.end(),
                    [](auto const& partition) -> auto { return partition.is_ready(); }));

            for (auto& partition : partitions)
            {
                write_partition(policies, partition, create_hyperslab, array);
            }

            // while (!remaining_partitions.empty())
            // {
            //     auto [partition_idx, partitions_] = hpx::when_any(remaining_partitions).get();
            //
            //     write_partition(policies, partitions_[partition_idx], create_hyperslab, array);
            //
            //     partitions_.erase(partitions_.begin() + partition_idx);
            //
            //     remaining_partitions = std::move(partitions_);
            // }
#endif
        }


        template<typename Policies, typename Partitions>
        auto write_partitions_constant(
            Policies const& policies,
            hdf5::Offset const& array_hyperslab_start,  // Only needed to offset block written to array
            Partitions const& partitions,
            std::string const& array_pathname,
            Count const to_lue_order,
            Count const from_lue_order,
            data_model::ID const object_id) -> hpx::future<void>
        {
            // print_debug("to_lue/open {} (start trying)", to_lue_order);

            using Partition = typename Partitions::value_type;
            using PartitionServer = Partition::Server;

            auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
                parse_array_pathname(array_pathname);

            std::filesystem::path const dataset_path{normalize(dataset_pathname)};
            hpx::promise<void> to_lue_open_dataset_p =
                to_lue_open_dataset_promise_for(dataset_path, to_lue_order);
            hpx::shared_future<void> to_lue_open_dataset_when_predecessor_done_f =
                to_lue_open_dataset_when_predecessor_done(dataset_path, to_lue_order);

            // TODO:
            // if (from_lue_order > 0)
            // {
            //     // Due to scheduling of tasks we may get here before a previous call to from_lue has even
            //     // started
            //     while (!from_lue_close_dataset_done_available(dataset_path, from_lue_order))
            //     {
            //         // hpx::cout << std::format(
            //         //                  "DEBUG: to_lue/open {} (sleep for {})\n", to_lue_order,
            //         //                  from_lue_order)
            //         //           << std::flush;
            //         hpx::this_thread::sleep_for(std::chrono::seconds(1));
            //     }
            // }

            // TODO: Only needed in parallel I/O case?
            lue_hpx_assert(
                from_lue_order == 0 || from_lue_close_dataset_done_available(dataset_path, from_lue_order));
            hpx::shared_future<void> from_lue_close_dataset_done_f =
                from_lue_close_dataset_done(dataset_path, from_lue_order);
            lue_hpx_assert(from_lue_close_dataset_done_f.is_ready());

            return hpx::dataflow(
                hpx::launch::async,
                [policies,
                 array_hyperslab_start,
                 dataset_path,
                 to_lue_order,
                 to_lue_open_dataset_p = std::move(to_lue_open_dataset_p),
                 phenomenon_name,
                 property_set_name,
                 property_name,
                 object_id](
                    [[maybe_unused]] auto const& from_lue_close_dataset_done_f,
                    [[maybe_unused]] auto const& to_lue_open_dataset_when_predecessor_done_f,
                    auto&& when_any_result_f) mutable -> auto
                {
                    AnnotateFunction const annotate{"to_lue: partitions constant"};

                    // hpx::cout << std::format("DEBUG: to_lue/open {}\n", to_lue_order) << std::flush;

                    auto [partition_idx, partitions] = when_any_result_f.get();

                    if constexpr (serial_io_non_thread_safe)
                    {
                        // Wait before opening the dataset. We need to make sure all HDF5 API calls are
                        // done from the same OS thread. We therefore make sure we don't have to wait anywhere
                        // else (and potentially get rescheduled on another OS thread) during the write.
                        hpx::wait_all(partitions);
                    }

                    // Open the dataset once the first partition is ready to be written
                    auto dataset = open_dataset(dataset_path.string(), H5F_ACC_RDWR);

                    // Open phenomenon
                    auto& phenomenon{dataset.phenomena()[phenomenon_name]};

                    // Open property-set
                    auto& property_set{phenomenon.property_sets()[property_set_name]};

                    // Open property
                    lue_hpx_assert(property_set.properties().contains(property_name));
                    lue_hpx_assert(
                        property_set.properties().shape_per_object(property_name) ==
                        data_model::ShapePerObject::different);
                    lue_hpx_assert(
                        property_set.properties().value_variability(property_name) ==
                        data_model::ValueVariability::constant);
                    using Properties = data_model::different_shape::Properties;
                    // data_model::different_shape::Property
                    auto& property{property_set.properties().collection<Properties>()[property_name]};

                    // constant: data_model::different_shape::Value
                    auto const& value{property.value()};

                    // constant: data_model::Array: hdf5::Dataset
                    auto array{value[object_id]};

                    // Done with the collective calls. Note that H5DOpen is collective in case of write.
                    // Writing partitions can happen independently now.
                    to_lue_open_dataset_p.set_value();

                    auto create_hyperslab =
                        [array_hyperslab_start](PartitionServer const& partition_server) -> auto
                    { return hyperslab(array_hyperslab_start, partition_server); };

                    // Synchronous
                    write_partitions(policies, partitions, create_hyperslab, array);

                    // Closing a dataset is a collective operation: only close the dataset (let it go out
                    // of scope) when it is our turn to do so.
                    hpx::promise<void> to_lue_close_dataset_p =
                        to_lue_close_dataset_promise_for(dataset_path, to_lue_order);
                    hpx::shared_future<void> to_lue_close_dataset_predecessor_f =
                        to_lue_close_dataset_when_predecessor_done(dataset_path, to_lue_order);

                    return to_lue_close_dataset_predecessor_f.then(
                        ternary_if<serial_io_non_thread_safe>(hpx::launch::sync, hpx::launch::async),
                        [dataset = std::move(dataset),
                         to_lue_close_dataset_p = std::move(to_lue_close_dataset_p),
                         partitions = std::move(partitions)](
                            [[maybe_unused]] auto const& close_for_read_file_predecessor_f) mutable -> auto
                        {
                            // The dataset must go out of scope before we set the promise's value
                            [](auto&& dataset) -> auto { HPX_UNUSED(dataset); }(std::move(dataset));
                            to_lue_close_dataset_p.set_value();
                        });
                },
                from_lue_close_dataset_done_f,
                to_lue_open_dataset_when_predecessor_done_f,
                hpx::when_any(partitions));
        }


        /*!
            @brief      .
            @tparam     .
            @param      .
            @return     .
            @exception  .

            The future returned becomes ready once all partitions have been written and the dataset is closed
            again.
        */
        template<typename Policies, typename Partitions>
        auto write_partitions_variable(
            Policies const& policies,
            hdf5::Offset const& array_hyperslab_start,  // Only needed to offset block written to array
            Partitions const& partitions,
            std::string const& array_pathname,
            Count const to_lue_order,
            [[maybe_unused]] Count const from_lue_order,
            data_model::ID const object_id,
            Index const time_step_idx) -> hpx::future<void>
        {
            print_debug("to_lue/open {} (start trying)", to_lue_order);

            using Partition = typename Partitions::value_type;
            using PartitionServer = Partition::Server;

            auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
                parse_array_pathname(array_pathname);

            std::filesystem::path const dataset_path{normalize(dataset_pathname)};

            hpx::promise<void> to_lue_open_dataset_p =
                to_lue_open_dataset_promise_for(dataset_path, to_lue_order);
            // hpx::shared_future<void> to_lue_open_dataset_when_predecessor_done_f =
            //     to_lue_open_dataset_when_predecessor_done(dataset_path, to_lue_order);

            // TODO:
            // if (from_lue_order > 0)
            // {
            //     // Due to scheduling of tasks we may get here before a previous call to from_lue has even
            //     // started
            //     while (!from_lue_close_dataset_done_available(dataset_path, from_lue_order))
            //     {
            //         hpx::cout << std::format(
            //                          "DEBUG: to_lue/open {} (sleep for {})\n", to_lue_order,
            //                          from_lue_order)
            //                   << std::flush;
            //         hpx::this_thread::sleep_for(std::chrono::seconds(1));
            //     }
            // }

            hpx::future<void> precondition_f{};

            if constexpr (serial_io)
            {
                // Serial I/O: we can only start writing once the dataset is closed. The dataset is closed
                // if:
                //
                // - Any previous from_lue call has finished (from_lue_order)
                //     - This is handled in the root locality and must therefore be true already
                //
                // - Any previous to_lue call has finished (to_lue_order - 1)
                //     - This is handled in the root locality and must therefore be true already
                //
                // - Any previous locality has finished writing its partitions (to_lue_close_dataset_done)
                //     - This is handled in the root locality and must therefore be true already

#ifndef NDEBUG
                lue_hpx_assert(
                    from_lue_order == 0 ||
                    from_lue_close_dataset_done_available(dataset_path, from_lue_order));
                hpx::shared_future<void> from_lue_close_dataset_done_f =
                    from_lue_close_dataset_done(dataset_path, from_lue_order);
                lue_hpx_assert(from_lue_close_dataset_done_f.is_ready());

                lue_hpx_assert(
                    to_lue_order == 1 || to_lue_close_dataset_done_available(dataset_path, to_lue_order - 1));
                hpx::shared_future<void> to_lue_close_dataset_done_f =
                    to_lue_close_dataset_done(dataset_path, to_lue_order - 1);
                lue_hpx_assert(to_lue_close_dataset_done_f.is_ready());
#endif

                if constexpr (serial_io_non_thread_safe)
                {
                    // Wait before opening the dataset. We need to make sure all HDF5 API calls are done from
                    // the same OS thread. We therefore make sure we don't have to wait anywhere else (and
                    // potentially get rescheduled on another OS thread) during the write.
                    precondition_f = hpx::when_all(partitions);
                }

                if constexpr (serial_io_thread_safe)
                {
                    // All preconditions are true already
                    precondition_f = hpx::make_ready_future();
                }
            }

            if constexpr (parallel_io)
            {
                // TODO: Only needed in parallel I/O case?
                // lue_hpx_assert(
                //     from_lue_order == 0 || from_lue_close_dataset_done_available(dataset_path,
                //     from_lue_order));
                // hpx::shared_future<void> from_lue_close_dataset_done_f =
                //     from_lue_close_dataset_done(dataset_path, from_lue_order);
                // lue_hpx_assert(from_lue_close_dataset_done_f.is_ready());
            }

            return hpx::dataflow(
                hpx::launch::async,
                [policies,
                 array_hyperslab_start,
                 dataset_path,
                 to_lue_order,
                 // to_lue_open_dataset_p = to_lue_open_dataset_promise_for(dataset_path, to_lue_order),
                 to_lue_open_dataset_p = std::move(to_lue_open_dataset_p),
                 phenomenon_name,
                 property_set_name,
                 property_name,
                 object_id,
                 time_step_idx]([[maybe_unused]] auto const& precondition_f, auto&& when_any_result_f) mutable
                    -> hpx::future<void>
                {
                    AnnotateFunction const annotate{"to_lue: partitions variable"};

                    print_debug("to_lue/open {}", to_lue_order);

                    auto [partition_idx, partitions] = when_any_result_f.get();

                    // Open the dataset once the first partition is ready to be written

                    // BUG: In some cases the dataset is still open for reading...:
                    // How can this be??? Maybe read doesn't really close the dataset is some circumstances?
                    // This happens when writing and reading from / to the same file

                    // 184: HDF5-DIAG: Error detected in HDF5 (1.10.10) thread 1:
                    // 184:   #000: ../../../src/H5F.c line 412 in H5Fopen(): unable to open file
                    // 184:     major: File accessibility
                    // 184:     minor: Unable to open file
                    // 184:   #001: ../../../src/H5Fint.c line 1698 in H5F_open(): file is already open for
                    // read-only 184:     major: File accessibility 184:     minor: Unable to open file

                    auto dataset = open_dataset(dataset_path.string(), H5F_ACC_RDWR);

                    {
                        // Find the array to write to (the HDF5 dataset)
                        auto& phenomenon{dataset.phenomena()[phenomenon_name]};
                        auto& property_set{phenomenon.property_sets()[property_set_name]};
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
                        // data_model::different_shape::constant_shape::Property
                        auto const& property{
                            property_set.properties().collection<Properties>()[property_name]};
                        // variable: data_model::different_shape::constant_shape::Value
                        auto const& value{property.value()};
                        // variable: data_model::same_shape::constant_shape::Value: data_model::Array:
                        // hdf5::Dataset
                        auto array{value[object_id]};

                        // Done with the collective calls. Note that H5DOpen is collective in case of write.
                        // Writing partitions can happen independently now.
                        to_lue_open_dataset_p.set_value();

                        auto create_hyperslab = [array_hyperslab_start, time_step_idx](
                                                    PartitionServer const& partition_server) -> auto
                        { return hyperslab(array_hyperslab_start, partition_server, 0, time_step_idx); };

                        // Synchronous
                        write_partitions(policies, partitions, create_hyperslab, array);
                    }

                    // Closing a dataset is a collective operation: only close the dataset (let it go out
                    // of scope) when it is our turn to do so.
                    hpx::promise<void> to_lue_close_dataset_p =
                        to_lue_close_dataset_promise_for(dataset_path, to_lue_order);
                    hpx::shared_future<void> to_lue_close_dataset_predecessor_f =
                        to_lue_close_dataset_when_predecessor_done(dataset_path, to_lue_order);

                    if constexpr (serial_io)
                    {
                        // No reason to go async. Any predecessor is done already.
                        lue_hpx_assert(to_lue_close_dataset_predecessor_f.is_ready());

                        // The dataset must go out of scope before we set the promise's value
                        [](auto&& dataset) -> auto
                        {
                            HPX_UNUSED(dataset);
                            // dataset.flush();
                        }(std::move(dataset));

                        to_lue_close_dataset_p.set_value();

                        lue_hpx_assert(to_lue_close_dataset_done(dataset_path, to_lue_order).is_ready());

                        print_debug("to_lue/close {}", to_lue_order);

                        return hpx::make_ready_future();
                    }

                    if constexpr (parallel_io)
                    {
                        return to_lue_close_dataset_predecessor_f.then(
                            hpx::launch::async,
                            [dataset = std::move(dataset),
                             to_lue_order,
                             to_lue_close_dataset_p = std::move(to_lue_close_dataset_p),
                             partitions = std::move(partitions)](
                                [[maybe_unused]] auto const& close_for_read_file_predecessor_f) mutable
                                -> auto
                            {
                                // The dataset must go out of scope before we set the promise's value
                                [](auto&& dataset) -> auto
                                {
                                    HPX_UNUSED(dataset);
                                    // dataset.flush();
                                }(std::move(dataset));

                                to_lue_close_dataset_p.set_value();

                                // TODO: Uncomment and refactor with previous block
                                // lue_hpx_assert(
                                //     to_lue_close_dataset_done(dataset_path, to_lue_order).is_ready());

                                print_debug("to_lue/close {}", to_lue_order);
                            });
                    }
                },
                precondition_f,
                hpx::when_any(partitions));
        }


        template<typename Policies, typename Partitions>
        struct WritePartitionsConstantAction:
            hpx::actions::make_action<
                decltype(&write_partitions_constant<Policies, Partitions>),
                &write_partitions_constant<Policies, Partitions>,
                WritePartitionsConstantAction<Policies, Partitions>>::type
        {
        };


        template<typename Policies, typename Partitions>
        struct WritePartitionsVariableAction:
            hpx::actions::make_action<
                decltype(&write_partitions_variable<Policies, Partitions>),
                &write_partitions_variable<Policies, Partitions>,
                WritePartitionsVariableAction<Policies, Partitions>>::type
        {
        };

    }  // namespace detail


    namespace policy::to_lue {

        template<typename InputElement>
        using DefaultPolicies =
            policy::DefaultPolicies<AllValuesWithinDomain<>, OutputElements<>, InputElements<InputElement>>;

        template<typename InputElement>
        using DefaultValuePolicies = policy::
            DefaultValuePolicies<AllValuesWithinDomain<>, OutputElements<>, InputElements<InputElement>>;

    }  // namespace policy::to_lue


    /*!
        @brief      Write an array to an array in a LUE dataset
        @tparam     Policies Policies type
        @tparam     Rank Rank of the array
        @param      policies Policies to use
        @param      array Array to write
        @param      array_pathname Pathname of the property to write to, formatted as
                    `<dataset_pathname>/<phenomenon_name>/<property_set_name>/<property_name>`
        @param      object_id ID of object whose property value to write
        @return     A future which becomes ready once the writing is done
    */
    template<typename Policies, Rank rank>
    auto to_lue(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies>, rank> const& array,
        std::string const& array_pathname,
        data_model::ID const object_id) -> hpx::future<void>
    {
        AnnotateFunction const annotate{"to_lue"};

        using Element = policy::InputElementT<Policies>;
        using Array = PartitionedArray<Element, rank>;
        using Partition = PartitionT<Array>;
        using Action = detail::WritePartitionsConstantAction<Policies, std::vector<Partition>>;

        // Partitions and localities
        auto const partition_idxs_by_locality{detail::partition_idxs_by_locality(array)};
        std::vector<hpx::future<void>> localities_finished{};
        localities_finished.reserve(partition_idxs_by_locality.size() + 1);

        Action action{};

        auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
            parse_array_pathname(array_pathname);
        auto const dataset_path{detail::normalize(dataset_pathname)};

        // Dependencies
        auto const to_lue_order = detail::to_lue_order(dataset_path);
        auto const from_lue_order = detail::current_from_lue_order(dataset_path);

        if constexpr (detail::serial_io)
        {
            // Make this to_lue call dependent on any previous call to to_lue / from_lue to the same
            // dataset, if done so. This ensures the dataset is closed.
            localities_finished.emplace_back(
                hpx::when_all(
                    detail::to_lue_finished(dataset_path, to_lue_order - 1),
                    detail::from_lue_finished(dataset_path, from_lue_order)));
        }

        for (auto const& [locality, partition_idxs] : partition_idxs_by_locality)
        {
            // Copy current partitions from input array to a new collection
            // NOTE: Use a view?
            std::vector<Partition> partitions(partition_idxs.size());

            for (std::size_t idx = 0; auto const partition_idx : partition_idxs)
            {
                partitions[idx++] = array.partitions()[partition_idx];
            }

            // Spawn a task that writes the current partitions to the dataset. This returns a future which
            // becomes ready once these partitions have been written.
            if constexpr (detail::serial_io)
            {
                hpx::future<void>& previous_locality_finished = localities_finished.back();
                localities_finished.push_back(previous_locality_finished.then(
                    [locality,
                     action,
                     policies,
                     array_hyperslab = detail::shape_to_hyperslab(array.shape()),
                     partitions = std::move(partitions),
                     array_pathname,
                     to_lue_order,
                     from_lue_order,
                     object_id]([[maybe_unused]] auto const& previous_locality_finished) -> auto
                    {
                        return hpx::async(
                            action,
                            locality,
                            std::move(policies),
                            array_hyperslab.start(),
                            std::move(partitions),
                            std::move(array_pathname),
                            to_lue_order,
                            from_lue_order,
                            object_id);
                    }));
            }
            else
            {
                localities_finished.push_back(
                    hpx::async(
                        action,
                        locality,
                        std::move(policies),
                        detail::shape_to_hyperslab(array.shape()).start(),
                        std::move(partitions),
                        std::move(array_pathname),
                        to_lue_order,
                        from_lue_order,
                        object_id));
            }
        }

        if constexpr (detail::serial_io)
        {
            // When the last process has finished, all partitions have been written
            auto to_lue_finished = localities_finished.back().share();
            lue_hpx_assert(to_lue_finished.valid());
            detail::add_to_lue_finished(dataset_path, to_lue_order, to_lue_finished);

            // Convert shared_future to future
            // return to_lue_finished.then([]([[maybe_unused]] auto const& future) -> auto {});
            return hpx::when_all(to_lue_finished);
        }
        else
        {

            return hpx::when_all(localities_finished.begin(), localities_finished.end());
        }
    }


    /*!
        @overload

        Default policies will be used.
    */
    template<typename Element, Rank rank>
    auto to_lue(
        PartitionedArray<Element, rank> const& array,
        std::string const& array_pathname,
        data_model::ID const object_id) -> hpx::future<void>
    {
        using Policies = policy::to_lue::DefaultPolicies<Element>;

        return to_lue(Policies{}, array, array_pathname, object_id);
    }


    /*!
        @brief      Write an array to an array in a LUE dataset
        @tparam     Policies Policies type
        @tparam     Rank Rank of the array
        @param      policies Policies to use
        @param      array Array to write
        @param      array_pathname Pathname of the property to write to, formatted as
                    `<dataset_pathname>/<phenomenon_name>/<property_set_name>/<property_name>`
        @param      object_id ID of object whose property value to write
        @param      time_step_idx Index of time step to write
        @return     A future which becomes ready once the writing is done and the dataset is closed again
    */
    template<typename Policies, Rank rank>
    auto to_lue(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies>, rank> const& array,
        std::string const& array_pathname,
        data_model::ID const object_id,
        Index const time_step_idx) -> hpx::future<void>
    {
        AnnotateFunction const annotate{"to_lue"};

        using Element = policy::InputElementT<Policies>;
        using Array = PartitionedArray<Element, rank>;
        using Partition = PartitionT<Array>;
        using Action = detail::WritePartitionsVariableAction<Policies, std::vector<Partition>>;

        // Partitions and localities
        auto const partition_idxs_by_locality{detail::partition_idxs_by_locality(array)};

        // Prevent re-allocation. We're using references below.
        std::vector<hpx::future<void>> localities_finished{};
        localities_finished.reserve(partition_idxs_by_locality.size() + 1);

        auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
            parse_array_pathname(array_pathname);
        auto const dataset_path{detail::normalize(dataset_pathname)};

        // Dependencies
        auto const to_lue_order = detail::to_lue_order(dataset_path);
        auto const from_lue_order = detail::current_from_lue_order(dataset_path);

        if constexpr (detail::serial_io)
        {
            // Make this to_lue call dependent on any previous calls to to_lue / from_lue to the same
            // dataset, if done so. This ensures the dataset is closed.
            localities_finished.emplace_back(
                hpx::when_all(
                    detail::to_lue_finished(dataset_path, to_lue_order - 1),
                    detail::from_lue_finished(dataset_path, from_lue_order)));
        }

        Action action{};

        for (auto const& [locality, partition_idxs] : partition_idxs_by_locality)
        {
            // Copy current selection of partitions from input array to a new collection
            std::vector<Partition> partitions(partition_idxs.size());

            for (std::size_t idx = 0; auto const partition_idx : partition_idxs)
            {
                partitions[idx++] = array.partitions()[partition_idx];
            }

            // Spawn a task that writes the partitions in the current process to the dataset. This returns a
            // future which becomes ready once these partitions have been written and the dataset has been
            // closed again.
            if constexpr (detail::serial_io)
            {
                lue_hpx_assert(localities_finished.size() < localities_finished.capacity());

                // Don't allow a process to open the dataset before the previous process has closed it again
                localities_finished.push_back(localities_finished.back().then(
                    [locality,
                     action,
                     policies,
                     array_hyperslab = detail::shape_to_hyperslab(array.shape()),
                     partitions = std::move(partitions),
                     array_pathname,
                     to_lue_order,
                     from_lue_order,
                     object_id,
                     time_step_idx]([[maybe_unused]] auto const& locality_finished) -> auto
                    {
                        return hpx::async(
                            action,
                            locality,
                            std::move(policies),
                            array_hyperslab.start(),
                            std::move(partitions),
                            std::move(array_pathname),
                            to_lue_order,
                            from_lue_order,
                            object_id,
                            time_step_idx);
                    }));
            }

            if constexpr (detail::parallel_io)
            {
                localities_finished.push_back(
                    hpx::async(
                        action,
                        locality,
                        std::move(policies),
                        detail::shape_to_hyperslab(array.shape()).start(),
                        std::move(partitions),
                        std::move(array_pathname),
                        to_lue_order,
                        from_lue_order,
                        object_id,
                        time_step_idx));
            }
        }

        if constexpr (detail::serial_io)
        {
            // When the last process has finished, all processes have finished
            auto to_lue_finished = localities_finished.back().share();
            lue_hpx_assert(to_lue_finished.valid());
            detail::add_to_lue_finished(dataset_path, to_lue_order, to_lue_finished);

            return hpx::when_all(to_lue_finished);
        }

        if constexpr (detail::parallel_io)
        {
            return hpx::when_all(localities_finished.begin(), localities_finished.end());
        }
    }


    /*!
        @overload

        Default policies will be used.
    */
    template<typename Element, Rank rank>
    auto to_lue(
        PartitionedArray<Element, rank> const& array,
        std::string const& array_pathname,
        data_model::ID const object_id,
        Index const time_step_idx) -> hpx::future<void>
    {
        using Policies = policy::to_lue::DefaultPolicies<Element>;

        return to_lue(Policies{}, array, array_pathname, object_id, time_step_idx);
    }

}  // namespace lue
