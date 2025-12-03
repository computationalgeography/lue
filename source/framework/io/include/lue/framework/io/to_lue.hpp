#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/core/annotate.hpp"
#include "lue/framework/core/component.hpp"
#include "lue/framework/io/configure.hpp"
#include "lue/framework/io/dataset.hpp"
#include "lue/framework/io/lue.hpp"
#include "lue/framework/io/util.hpp"
#include "lue/data_model/hl/util.hpp"

// #include <hpx/iostream.hpp>


/*!
    @file

    Thiѕ header contains the implementation of the various to_lue function overloads.

    The goal is to return an array as soon as possible, allowing the caller to create more work, while
    the write-tasks start the writing.
*/


// LUE dataset is an HDF5 file
// LUE array is an HDF5 dataset

// All I/O:
// - Writing to a dataset must happen after any previous calls to reading to that same dataset have
//   finished. See usage of from_lue_order.

// Serial I/O:
// - No two processes can have the same dataset open for writing
// - Writing to an array can be done concurrently. The HDF5 library is threadsafe so any serialization
//   happens there.

// Parallel I/O:
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

            // Asynchronously write all partitions. Let HDF5 do the serialization.
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
            // hpx::cout << std::format("DEBUG: to_lue/open {} (start trying)\n", to_lue_order) << std::flush;

            auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
                parse_array_pathname(array_pathname);

            std::filesystem::path const dataset_path{normalize(dataset_pathname)};
            hpx::promise<void> to_lue_open_dataset_p =
                to_lue_open_dataset_promise_for(dataset_path, to_lue_order);
            hpx::shared_future<void> to_lue_open_dataset_when_predecessor_done_f =
                to_lue_open_dataset_when_predecessor_done(dataset_path, to_lue_order);

            if (from_lue_order > 0)
            {
                // Due to scheduling of tasks we may get here before a previous call to from_lue has even
                // started
                while (!from_lue_close_dataset_done_available(dataset_path, from_lue_order))
                {
                    // hpx::cout << std::format(
                    //                  "DEBUG: to_lue/open {} (sleep for {})\n", to_lue_order,
                    //                  from_lue_order)
                    //           << std::flush;
                    hpx::this_thread::sleep_for(std::chrono::seconds(1));
                }
            }

            hpx::shared_future<void> from_lue_close_dataset_done_f =
                from_lue_close_dataset_done(dataset_path, from_lue_order);

            // TODO: How to make all I/O happen on the I/O thread? Passing executor to continuation works in
            // parallel I/O case but serial case hangs. Currently, the nested I/O ends up on the compute
            // thread it seems.
            // hpx::execution::experimental::io_pool_executor executor;

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

#ifdef LUE_FRAMEWORK_WITH_PARALLEL_IO
                // TODO:
                // Done with the collective calls. Writing partitions can happen indepenently now.
                // to_lue_open_dataset_p.set_value();
#endif

                    using Partition = typename Partitions::value_type;
                    using PartitionServer = Partition::Server;

                    auto create_hyperslab =
                        [array_hyperslab_start](PartitionServer const& partition_server) -> auto
                    { return hyperslab(array_hyperslab_start, partition_server); };

                    auto [partition_idx, partitions] = when_any_result_f.get();

                    // Blocks
                    write_partitions(policies, partitions, create_hyperslab, array);

                    // TODO: handle parallel case: close is collective

                    // The dataset must go out of scope before we set the promise's value
                    [dataset = std::move(dataset) /* , to_lue_order */]() -> auto
                    {
                        dataset.flush();
                        // hpx::cout << std::format("DEBUG: to_lue/close {}\n", to_lue_order) <<
                        // std::flush;
                    }();

                    to_lue_open_dataset_p.set_value();
                },
                from_lue_close_dataset_done_f,
                to_lue_open_dataset_when_predecessor_done_f,
                hpx::when_any(partitions));
        }


        template<typename Policies, typename Partitions>
        auto write_partitions_variable(
            Policies const& policies,
            hdf5::Offset const& array_hyperslab_start,  // Only needed to offset block written to array
            Partitions const& partitions,
            std::string const& array_pathname,
            Count const to_lue_order,
            Count const from_lue_order,
            data_model::ID const object_id,
            Index const time_step_idx) -> hpx::future<void>
        {
            // hpx::cout << std::format("DEBUG: to_lue/open {} (start trying)\n", to_lue_order) << std::flush;

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
            //         hpx::cout << std::format(
            //                          "DEBUG: to_lue/open {} (sleep for {})\n", to_lue_order,
            //                          from_lue_order)
            //                   << std::flush;
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
                 object_id,
                 time_step_idx](
                    [[maybe_unused]] auto const& from_lue_close_dataset_done_f,
                    [[maybe_unused]] auto const& to_lue_open_dataset_when_predecessor_done_f,
                    auto&& when_any_result_f) mutable -> auto
                {
                    AnnotateFunction const annotate{"to_lue: partitions variable"};

                    // hpx::cout << std::format("DEBUG: to_lue/open {}\n", to_lue_order) << std::flush;

                    // Open the dataset once the first partition is ready to be written
                    auto dataset = open_dataset(dataset_path.string(), H5F_ACC_RDWR);

                    // Done with collective call
                    to_lue_open_dataset_p.set_value();

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
                        data_model::ValueVariability::variable);
                    lue_hpx_assert(
                        property_set.properties().shape_variability(property_name) ==
                        data_model::ShapeVariability::constant);
                    using Properties = data_model::different_shape::constant_shape::Properties;
                    // data_model::different_shape::constant_shape::Property
                    auto const& property{property_set.properties().collection<Properties>()[property_name]};

                    // variable: data_model::different_shape::constant_shape::Value
                    const auto& value{property.value()};

                    // variable: data_model::same_shape::constant_shape::Value: data_model::Array:
                    // hdf5::Dataset
                    auto array{value[object_id]};

                    auto create_hyperslab = [array_hyperslab_start,
                                             time_step_idx](PartitionServer const& partition_server) -> auto
                    { return hyperslab(array_hyperslab_start, partition_server, 0, time_step_idx); };

                    auto [partition_idx, partitions] = when_any_result_f.get();

                    // Synchronous
                    write_partitions(policies, partitions, create_hyperslab, array);

                    // Closing a dataset is a collective operation: only close the dataset (let it go out
                    // of scope) when it is our turn to do so.
                    hpx::promise<void> to_lue_close_dataset_p =
                        to_lue_close_dataset_promise_for(dataset_path, to_lue_order);
                    hpx::shared_future<void> to_lue_close_dataset_predecessor_f =
                        to_lue_close_dataset_when_predecessor_done(dataset_path, to_lue_order);

                    return to_lue_close_dataset_predecessor_f.then(
                        [dataset = std::move(dataset),
                         // to_lue_order,
                         to_lue_close_dataset_p = std::move(to_lue_close_dataset_p),
                         partitions = std::move(partitions)](
                            [[maybe_unused]] auto const& close_for_read_file_predecessor_f) mutable -> auto
                        {
                            // The dataset must go out of scope before we set the promise's value
                            [dataset = std::move(dataset)
                             // to_lue_order
                            ]() -> auto
                            {
                                HPX_UNUSED(dataset);
                                // hpx::cout << std::format("DEBUG: to_lue/close {}\n", to_lue_order)
                                //           << std::flush;
                            }();

                            to_lue_close_dataset_p.set_value();
                        });

                    // // TODO: handle parallel case: close is collective
                    //
                    // // The dataset must go out of scope before we set the promise's value
                    // [dataset = std::move(dataset), to_lue_order]() -> auto
                    // {
                    //     dataset.flush();
                    //     hpx::cout << std::format("DEBUG: to_lue/close {}\n", to_lue_order) << std::flush;
                    // }();
                    //
                    // to_lue_open_dataset_p.set_value();
                },
                from_lue_close_dataset_done_f,
                to_lue_open_dataset_when_predecessor_done_f,
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
    [[nodiscard]] auto to_lue(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies>, rank> const& array,
        std::string const& array_pathname,
        data_model::ID const object_id) -> hpx::future<void>
    {
        AnnotateFunction const annotate{"to_lue"};

        using Element = policy::InputElementT<Policies>;
        using Array = PartitionedArray<Element, rank>;
        using Partition = PartitionT<Array>;

        // Group partitions by locality
        auto const partition_idxs_by_locality{detail::partition_idxs_by_locality(array)};
        lue_hpx_assert(partition_idxs_by_locality.size() <= hpx::find_all_localities().size());

        using Action = detail::WritePartitionsConstantAction<Policies, std::vector<Partition>>;
        Action action{};

        auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
            parse_array_pathname(array_pathname);
        auto const dataset_path{detail::normalize(dataset_pathname)};
        auto const to_lue_order = detail::to_lue_order(dataset_path);
        auto const from_lue_order = detail::current_from_lue_order(dataset_path);

        std::vector<hpx::future<void>> localities_finished{};
        localities_finished.reserve(partition_idxs_by_locality.size() + 1);

#ifndef LUE_FRAMEWORK_WITH_PARALLEL_IO
        // Make write dependent on previous write to the same dataset, if done so
        localities_finished.emplace_back(
            detail::to_lue_finished(dataset_path, to_lue_order - 1)
                .then([]([[maybe_unused]] auto const& future) -> auto {}));
#endif

        for (auto const& [locality, partition_idxs] : partition_idxs_by_locality)
        {
            // Copy current partitions from input array to a new collection
            std::vector<Partition> partitions{};
            partitions.resize(partition_idxs.size());

            for (std::size_t idx = 0; auto const partition_idx : partition_idxs)
            {
                partitions[idx++] = array.partitions()[partition_idx];
            }

            // Spawn a task that writes the current partitions to the dataset. This returns a future which
            // becomes ready once these partitions have been written.
#ifndef LUE_FRAMEWORK_WITH_PARALLEL_IO
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
#else
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
#endif
        }

#ifndef LUE_FRAMEWORK_WITH_PARALLEL_IO
        // When the last process has finished, all partitions have been written
        auto to_lue_finished = localities_finished.back().share();
        lue_hpx_assert(to_lue_finished.valid());
        detail::add_to_lue_finished(dataset_path, to_lue_order, to_lue_finished);

        // Convert shared_future to future
        return to_lue_finished.then([]([[maybe_unused]] auto const& future) -> auto {});
#else
        return hpx::when_all(localities_finished.begin(), localities_finished.end());
#endif
    }


    /*!
        @overload

        Default policies will be used.
    */
    template<typename Element, Rank rank>
    [[nodiscard]] auto to_lue(
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
        @return     A future which becomes ready once the writing is done
    */
    template<typename Policies, Rank rank>
    [[nodiscard]] auto to_lue(
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

        // Group partitions by locality
        auto const partition_idxs_by_locality{detail::partition_idxs_by_locality(array)};
        lue_hpx_assert(partition_idxs_by_locality.size() <= hpx::find_all_localities().size());

        using Action = detail::WritePartitionsVariableAction<Policies, std::vector<Partition>>;
        Action action{};

        auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
            parse_array_pathname(array_pathname);
        auto const dataset_path{detail::normalize(dataset_pathname)};
        auto const to_lue_order = detail::to_lue_order(dataset_path);
        auto const from_lue_order = detail::current_from_lue_order(dataset_path);

        std::vector<hpx::future<void>> localities_finished{};
        localities_finished.reserve(partition_idxs_by_locality.size() + 1);

#ifndef LUE_FRAMEWORK_WITH_PARALLEL_IO
        // Make write dependent on any previous write or read to the same dataset, if done so
        localities_finished.emplace_back(
            hpx::when_all(
                detail::to_lue_finished(dataset_path, to_lue_order - 1),
                detail::from_lue_finished(dataset_path, from_lue_order)));
#endif

        for (auto const& [locality, partition_idxs] : partition_idxs_by_locality)
        {
            // Copy current partitions from input array to a new collection
            std::vector<Partition> partitions{};
            partitions.resize(partition_idxs.size());

            for (std::size_t idx = 0; auto const partition_idx : partition_idxs)
            {
                partitions[idx++] = array.partitions()[partition_idx];
            }

            // Spawn a task that writes the current partitions to the dataset. This returns a future which
            // becomes ready once these partitions have been written.
#ifndef LUE_FRAMEWORK_WITH_PARALLEL_IO
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
                 object_id,
                 time_step_idx]([[maybe_unused]] auto const& previous_locality_finished) -> auto
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
#else
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
#endif
        }

#ifndef LUE_FRAMEWORK_WITH_PARALLEL_IO
        // When the last process has finished, all partitions have been written
        auto to_lue_finished = localities_finished.back().share();
        lue_hpx_assert(to_lue_finished.valid());
        detail::add_to_lue_finished(dataset_path, to_lue_order, to_lue_finished);

        // Convert shared_future to future
        return to_lue_finished.then([]([[maybe_unused]] auto const& future) -> auto {});
#else
        return hpx::when_all(localities_finished.begin(), localities_finished.end());
#endif
    }


    /*!
        @overload

        Default policies will be used.
    */
    template<typename Element, Rank rank>
    [[nodiscard]] auto to_lue(
        PartitionedArray<Element, rank> const& array,
        std::string const& array_pathname,
        data_model::ID const object_id,
        Index const time_step_idx) -> hpx::future<void>
    {
        using Policies = policy::to_lue::DefaultPolicies<Element>;

        return to_lue(Policies{}, array, array_pathname, object_id, time_step_idx);
    }

}  // namespace lue
