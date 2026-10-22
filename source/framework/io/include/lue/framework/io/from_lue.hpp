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
// #include <hpx/runtime_local/service_executors.hpp>


/*!
    @file

    Thiѕ header contains the implementation of the various from_lue function overloads.

    The goal is to return an array as soon as possible, allowing the caller to create more work, while
    the read-tasks start the reading. The idea is that, as soon as partitions have been read, they can
    participate in computations, while other partitions are still being read.
*/

namespace lue {
    namespace detail {

        template<typename Policies, typename CreateHyperslab, typename Partitions>
        void read_partitions(
            [[maybe_unused]] Policies const& policies,
            data_model::Array const& array,
            CreateHyperslab create_hyperslab,
            Partitions const& partitions)
        {
            // TODO: Use no-data policy
            // If no-data in the HDF5 dataset, write no-data to the LUE partition

            // Synchronously write all partitions, from the same OS thread
            lue_hpx_assert(
                std::all_of(
                    partitions.begin(),
                    partitions.end(),
                    [](auto const& partition) -> auto { return partition.is_ready(); }));

            // Open value. Configure for use of parallel I/O if relevant.
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

                auto partition_data = partition.data(hpx::launch::sync);
                Element* buffer{partition_data.data()};

                array.read(memory_datatype, create_hyperslab(partition), transfer_property_list, buffer);
            }
        }


        template<typename Policies, typename Partitions>
        auto read_partitions_constant(
            Policies const& policies,
            std::string const& array_pathname,
            hdf5::Offset const& array_hyperslab_start,  // Only needed to offset block read from array
            data_model::ID const object_id,
            Partitions const& partitions) -> hpx::future<std::tuple<Partitions, hpx::future<void>>>
        {
            using Partition = typename Partitions::value_type;

            auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
                parse_array_pathname(array_pathname);

            std::filesystem::path const dataset_path{root::normalize(dataset_pathname)};

            // Wait for all partitions to have become ready before opening the dataset. In the serial /
            // non-threadsafe context it is important that all HDF5 API calls are done from the same OS
            // thread. We therefore make sure we don't have to wait anywhere else (and potentially get
            // rescheduled on another OS thread) during the read. In other contexts it is a fine approach as
            // well. It doesn't help to read in parallel from within a process.

            // // Get a reference to one of the IO specific HPX io_service objects
            // hpx::execution::experimental::io_pool_executor executor;
            // // Schedule the handler to run on one of the io_service's OS-threads

            return hpx::dataflow(
                // executor,
                hpx::launch::async,
                [policies,
                 array_hyperslab_start,
                 dataset_path,
                 phenomenon_name,
                 property_set_name,
                 property_name,
                 object_id](auto&& partitions_f) mutable -> std::tuple<Partitions, hpx::future<void>>
                {
                    AnnotateFunction const annotate{"from_lue: partitions constant"};

                    Partitions partitions = partitions_f.get();

                    auto dataset = open_dataset(dataset_path.string(), H5F_ACC_RDONLY);

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
                            data_model::ValueVariability::constant);
                        using Properties = data_model::different_shape::Properties;
                        auto const& property{
                            property_set.properties().collection<Properties>()[property_name]};
                        // constant: data_model::different_shape::Value
                        auto const& value{property.value()};
                        // constant: data_model::Array: hdf5::Dataset
                        auto array{value[object_id]};

                        auto create_hyperslab = [array_hyperslab_start](Partition const& partition) -> auto
                        { return hyperslab(array_hyperslab_start, partition); };

                        // Synchronous
                        read_partitions(policies, array, create_hyperslab, partitions);
                    }

                    return {std::move(partitions), hpx::make_ready_future()};
                },
                hpx::when_all(partitions));
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
            hdf5::Offset const& array_hyperslab_start,
            data_model::ID const object_id,
            Index const time_step_idx,
            Partitions const& partitions) -> hpx::future<std::tuple<Partitions, hpx::future<void>>>
        {
            using Partition = typename Partitions::value_type;

            auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
                parse_array_pathname(array_pathname);

            std::filesystem::path const dataset_path{root::normalize(dataset_pathname)};

            // Wait for all partitions to have become ready before opening the dataset. In the serial /
            // non-threadsafe context it is important that all HDF5 API calls are done from the same OS
            // thread. We therefore make sure we don't have to wait anywhere else (and potentially get
            // rescheduled on another OS thread) during the read. In other contexts it is a fine approach as
            // well. It doesn't help to read in parallel from within a process.

            // // Get a reference to one of the IO specific HPX io_service objects
            // hpx::execution::experimental::io_pool_executor executor;
            // // Schedule the handler to run on one of the io_service's OS-threads

            return hpx::dataflow(
                // executor,
                hpx::launch::async,
                [policies,
                 array_hyperslab_start,
                 dataset_path,
                 phenomenon_name,
                 property_set_name,
                 property_name,
                 object_id,
                 time_step_idx](auto&& partitions_f) mutable -> std::tuple<Partitions, hpx::future<void>>
                {
                    AnnotateFunction const annotate{"from_lue: partitions variable"};

                    Partitions partitions = partitions_f.get();

                    auto dataset = open_dataset(dataset_path.string(), H5F_ACC_RDONLY);

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

                        auto create_hyperslab = [array_hyperslab_start,
                                                 time_step_idx](Partition const& partition) -> auto
                        { return hyperslab(array_hyperslab_start, partition, 0, time_step_idx); };

                        // Synchronous
                        read_partitions(policies, array, create_hyperslab, partitions);
                    }

                    return {std::move(partitions), hpx::make_ready_future()};
                },
                hpx::when_all(partitions));
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
            AnnotateFunction const annotate{"from_lue"};

            using Element = policy::OutputElementT<Policies>;
            using Array = PartitionedArray<Element, rank>;
            using Partition = PartitionT<Array>;
            using Action = ReadPartitionsConstantAction<Policies, std::vector<Partition>>;

            auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
                parse_array_pathname(array_pathname);
            auto const dataset_path{root::normalize(dataset_pathname)};

            // Dependencies
            auto const from_lue_order = root::from_lue_order(dataset_path);
            auto const to_lue_order = root::current_to_lue_order(dataset_path);

            // Make this from_lue call dependent on any previous calls to to_lue / from_lue to the same
            // dataset, if done so. This ensures the dataset is closed.
            auto precondition_f = hpx::when_all(
                root::from_lue_finished(dataset_path, from_lue_order - 1),
                root::to_lue_finished(dataset_path, to_lue_order));

            // Partitions and localities
            auto const partition_idxs_by_locality{detail::partition_idxs_by_locality(array)};

            // Grab partitions from the array. They will be returned again by the task.
            std::vector<Partition> array_partitions(array.nr_partitions());
            std::move(array.partitions().begin(), array.partitions().end(), array_partitions.begin());

            // std::tuple< hpx::future<std::vector<Partition>>, hpx::future<hpx::future<void>> >
            auto [partitions_f, from_lue_finished_ff] = hpx::split_future(precondition_f.then(
                [policies,
                 array_partitions = std::move(array_partitions),
                 partition_idxs_by_locality = std::move(partition_idxs_by_locality),
                 array_pathname,
                 dataset_path,
                 array_hyperslab_start,
                 object_id]([[maybe_unused]] auto const& precondition_f) mutable
                    -> std::tuple<std::vector<Partition>, hpx::future<void>>
                {
                    std::vector<hpx::future<void>> localities_finished{};
                    localities_finished.reserve(partition_idxs_by_locality.size());

                    Action action{};

                    // Iterate over all grouped partitions
                    for (auto const& [locality, partition_idxs] : partition_idxs_by_locality)
                    {
                        // Move subset of partitions into a new collection
                        std::vector<Partition> locality_partitions(partition_idxs.size());

                        for (std::size_t idx = 0; auto const partition_idx : partition_idxs)
                        {
                            lue_hpx_assert(!locality_partitions[idx].valid());
                            lue_hpx_assert(array_partitions[partition_idx].valid());

                            locality_partitions[idx++] = std::move(array_partitions[partition_idx]);

                            lue_hpx_assert(locality_partitions[idx - 1]);
                            lue_hpx_assert(!array_partitions[partition_idx].valid());
                        }

                        // Spawn a task that reads from the dataset into the partitions. This returns a
                        // collection of futures to partitions, each of which becomes ready once its data
                        // is read. Also, a future is returned which becomes ready once the dataset has
                        // been closed. This happens later than when the partitions have become ready.

                        // A future to a future to a tuple with partitions and a future
                        hpx::future<hpx::future<std::tuple<std::vector<Partition>, hpx::future<void>>>>
                            future_of_future = hpx::async(
                                action,
                                locality,
                                policies,
                                array_pathname,
                                array_hyperslab_start,
                                object_id,
                                std::move(locality_partitions));

                        // To be able to split the future into a collection of partitions and a future,
                        // the two outer futures have to be collapsed into one, so we end up with a future
                        // to a tuple.
                        auto [partitions_f, dataset_closed_f] = hpx::split_future(future_of_future.then(
                            [](auto&& future_of_future)
                                -> hpx::future<std::tuple<std::vector<Partition>, hpx::future<void>>>
                            { return future_of_future.get(); }));

                        std::vector<hpx::future<Partition>> partition_fs =
                            hpx::split_future<Partition>(std::move(partitions_f), std::size(partition_idxs));
                        localities_finished.push_back(std::move(dataset_closed_f));

                        lue_hpx_assert(partition_fs.size() == partition_idxs.size());

                        // Iterate over each partition in the subset
                        for (std::size_t idx = 0; auto const partition_idx : partition_idxs)
                        {
                            // Replace the current partition with a new one which becomes ready once the
                            // reading has finished. Under the hood the "new" partition is the same and
                            // the original one, but this is not relevant for the caller.

                            // TODO: Is it? Since we are reusing the original partition (which is already
                            //       ready), the caller may think the partition is already ready(?).

                            lue_hpx_assert(!array_partitions[partition_idx].valid());
                            lue_hpx_assert(partition_fs[idx].valid());

                            array_partitions[partition_idx] = partition_fs[idx++].then(
                                [](auto&& partition_f) -> Partition
                                {
                                    lue_hpx_assert(partition_f.valid());
                                    lue_hpx_assert(partition_f.is_ready());

                                    Partition partition{partition_f.get()};

                                    lue_hpx_assert(partition.valid());

                                    return partition;
                                });

                            lue_hpx_assert(array_partitions[partition_idx].valid());
                        }
                    }

                    // All partitions have been replaced by one which becomes ready once the reading has
                    // finished
                    lue_hpx_assert(
                        std::all_of(
                            array_partitions.begin(),
                            array_partitions.end(),
                            [](auto const& partition) -> auto { return partition.valid(); }));

                    hpx::future<void> from_lue_finished_f =
                        hpx::when_all(localities_finished.begin(), localities_finished.end());

                    lue_hpx_assert(from_lue_finished_f.valid());

                    return std::make_tuple(std::move(array_partitions), std::move(from_lue_finished_f));
                }));

            root::add_from_lue_finished(
                dataset_path,
                from_lue_order,
                from_lue_finished_ff.then(
                    [](hpx::future<hpx::future<void>>&& from_lue_finished_ff)
                    {
                        return from_lue_finished_ff.get().then(
                            []([[maybe_unused]] hpx::future<void>&& from_lue_finished_f) { return; });
                    }));

            root::from_lue_finished(dataset_path, from_lue_order)
                .then(
                    [dataset_path,
                     from_lue_order]([[maybe_unused]] hpx::shared_future<void> const& finished_f) -> void
                    { root::from_lue_handled(dataset_path, from_lue_order - 1); });

            std::vector<hpx::future<Partition>> partition_fs =
                hpx::split_future<Partition>(std::move(partitions_f), array.nr_partitions());

            lue_hpx_assert(static_cast<Count>(partition_fs.size()) == array.nr_partitions());
            lue_hpx_assert(
                static_cast<Count>(partition_fs.size()) ==
                static_cast<Count>(std::distance(array.partitions().begin(), array.partitions().end())));

            std::move(partition_fs.begin(), partition_fs.end(), array.partitions().begin());

            lue_hpx_assert(
                std::all_of(
                    array.partitions().begin(),
                    array.partitions().end(),
                    [](auto const& partition) -> auto { return partition.valid(); }));

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

            auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
                parse_array_pathname(array_pathname);
            auto const dataset_path{root::normalize(dataset_pathname)};

            // Dependencies
            auto const from_lue_order = root::from_lue_order(dataset_path);
            auto const to_lue_order = root::current_to_lue_order(dataset_path);

            // Make this from_lue call dependent on any previous calls to to_lue / from_lue to the same
            // dataset, if done so. This ensures the dataset is closed.
            auto precondition_f = hpx::when_all(
                root::from_lue_finished(dataset_path, from_lue_order - 1),
                root::to_lue_finished(dataset_path, to_lue_order));

            // Partitions and localities
            auto const partition_idxs_by_locality{detail::partition_idxs_by_locality(array)};

            // Grab partitions from the array. They will be returned again by the task.
            std::vector<Partition> array_partitions(array.nr_partitions());
            std::move(array.partitions().begin(), array.partitions().end(), array_partitions.begin());

            // std::tuple< hpx::future<std::vector<Partition>>, hpx::future<hpx::future<void>> >
            auto [partitions_f, from_lue_finished_ff] = hpx::split_future(precondition_f.then(
                [policies,
                 array_partitions = std::move(array_partitions),
                 partition_idxs_by_locality = std::move(partition_idxs_by_locality),
                 array_pathname,
                 dataset_path,
                 array_hyperslab_start,
                 object_id,
                 time_step_idx]([[maybe_unused]] auto const& precondition_f) mutable
                    -> std::tuple<std::vector<Partition>, hpx::future<void>>
                {
                    std::vector<hpx::future<void>> localities_finished{};
                    localities_finished.reserve(partition_idxs_by_locality.size());

                    Action action{};

                    // Iterate over all grouped partitions
                    for (auto const& [locality, partition_idxs] : partition_idxs_by_locality)
                    {
                        // Move subset of partitions into a new collection
                        std::vector<Partition> locality_partitions(partition_idxs.size());

                        for (std::size_t idx = 0; auto const partition_idx : partition_idxs)
                        {
                            lue_hpx_assert(!locality_partitions[idx].valid());
                            lue_hpx_assert(array_partitions[partition_idx].valid());

                            locality_partitions[idx++] = std::move(array_partitions[partition_idx]);

                            lue_hpx_assert(locality_partitions[idx - 1]);
                            lue_hpx_assert(!array_partitions[partition_idx].valid());
                        }

                        // Spawn a task that reads from the dataset into the partitions. This returns a
                        // collection of futures to partitions, each of which becomes ready once its data
                        // is read. Also, a future is returned which becomes ready once the dataset has
                        // been closed. This happens later than when the partitions have become ready.

                        // A future to a future to a tuple with partitions and a future
                        hpx::future<hpx::future<std::tuple<std::vector<Partition>, hpx::future<void>>>>
                            future_of_future = hpx::async(
                                action,
                                locality,
                                policies,
                                array_pathname,
                                array_hyperslab_start,
                                object_id,
                                time_step_idx,
                                std::move(locality_partitions));

                        // To be able to split the future into a collection of partitions and a future,
                        // the two outer futures have to be collapsed into one, so we end up with a future
                        // to a tuple.
                        auto [partitions_f, dataset_closed_f] = hpx::split_future(future_of_future.then(
                            [](auto&& future_of_future)
                                -> hpx::future<std::tuple<std::vector<Partition>, hpx::future<void>>>
                            { return future_of_future.get(); }));

                        std::vector<hpx::future<Partition>> partition_fs =
                            hpx::split_future<Partition>(std::move(partitions_f), std::size(partition_idxs));
                        localities_finished.push_back(std::move(dataset_closed_f));

                        lue_hpx_assert(partition_fs.size() == partition_idxs.size());

                        // Iterate over each partition in the subset
                        for (std::size_t idx = 0; auto const partition_idx : partition_idxs)
                        {
                            // Replace the current partition with a new one which becomes ready once the
                            // reading has finished. Under the hood the "new" partition is the same and
                            // the original one, but this is not relevant for the caller.

                            // NOTE: Is it? Since we are reusing the original partition (which is already
                            //       ready), the caller may think the partition is already ready(?).

                            lue_hpx_assert(!array_partitions[partition_idx].valid());
                            lue_hpx_assert(partition_fs[idx].valid());

                            array_partitions[partition_idx] = partition_fs[idx++].then(
                                [](auto&& partition_f) -> Partition
                                {
                                    lue_hpx_assert(partition_f.valid());
                                    lue_hpx_assert(partition_f.is_ready());

                                    Partition partition{partition_f.get()};

                                    lue_hpx_assert(partition.valid());

                                    return partition;
                                });

                            lue_hpx_assert(array_partitions[partition_idx].valid());
                        }
                    }

                    // All partitions have been replaced by one which becomes ready once the reading has
                    // finished
                    lue_hpx_assert(
                        std::all_of(
                            array_partitions.begin(),
                            array_partitions.end(),
                            [](auto const& partition) -> auto { return partition.valid(); }));

                    hpx::future<void> from_lue_finished_f =
                        hpx::when_all(localities_finished.begin(), localities_finished.end());

                    lue_hpx_assert(from_lue_finished_f.valid());

                    return std::make_tuple(std::move(array_partitions), std::move(from_lue_finished_f));
                }));

            root::add_from_lue_finished(
                dataset_path,
                from_lue_order,
                from_lue_finished_ff.then(
                    [](hpx::future<hpx::future<void>>&& from_lue_finished_ff)
                    {
                        return from_lue_finished_ff.get().then(
                            []([[maybe_unused]] hpx::future<void>&& from_lue_finished_f) { return; });
                    }));

            root::from_lue_finished(dataset_path, from_lue_order)
                .then(
                    [dataset_path,
                     from_lue_order]([[maybe_unused]] hpx::shared_future<void> const& finished_f) -> void
                    { root::from_lue_handled(dataset_path, from_lue_order - 1); });

            std::vector<hpx::future<Partition>> partition_fs =
                hpx::split_future<Partition>(std::move(partitions_f), array.nr_partitions());

            lue_hpx_assert(static_cast<Count>(partition_fs.size()) == array.nr_partitions());
            lue_hpx_assert(
                static_cast<Count>(partition_fs.size()) ==
                static_cast<Count>(std::distance(array.partitions().begin(), array.partitions().end())));

            std::move(partition_fs.begin(), partition_fs.end(), array.partitions().begin());

            lue_hpx_assert(
                std::all_of(
                    array.partitions().begin(),
                    array.partitions().end(),
                    [](auto const& partition) -> auto { return partition.valid(); }));

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

            lue_hpx_assert(rank<Shape> == 2);

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

            lue_hpx_assert(rank<Shape> == 2);

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
