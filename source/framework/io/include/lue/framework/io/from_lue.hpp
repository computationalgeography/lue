#pragma once
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/core/component.hpp"
#include "lue/framework/io/dataset.hpp"
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


// All tasks can perform independent I/O, asynchronous → independent is OK when blocks are
// non-overlapping / non-interleaved

// NOTE:
// Depending on the expected data size and number of your I/O requests, having it INDEPENDENT or
// COLLECTIVE can help or hinder. Independent is good if the data size of I/O is sufficiently
// large (hopefully a few file system blocksize), otherwise, Collective is better

// - Is there some heuristic we can use to choose between INDEPENDENT / COLLECTIVE?


namespace lue {
    namespace detail {

        template<typename Policies, typename Property, typename CreateHyperslab, typename Partitions>
        auto read_partitions(
            Policies const& policies,
            Property const& property,
            CreateHyperslab create_hyperslab,
            data_model::ID const object_id,
            Partitions&& partitions) -> std::tuple<Partitions, hpx::future<void>>
        {
            // Open value. Configure for use of parallel I/O if necessary.
            hdf5::Dataset::TransferPropertyList transfer_property_list{};

#if LUE_FRAMEWORK_WITH_PARALLEL_IO
            transfer_property_list.set_transfer_mode(H5FD_MPIO_INDEPENDENT);
#endif

            // Iterate over partitions and read each partition's piece from the dataset
            auto const& value{property.value()};
            auto const array{value[object_id]};

            using Partition = typename Partitions::value_type;
            using Element = ElementT<Partition>;

            // For each partition, spawn a task that will read from the dataset into the array. This task
            // returns a partition component client which becomes ready once the reading has finished. Use
            // continuations to serialize the reads.

            Partitions new_partitions(std::size(partitions));
            hdf5::Datatype const memory_datatype{hdf5::native_datatype<Element>()};

            std::vector<hpx::future<void>> partitions_read_f(std::size(partitions));

            for (std::size_t partition_idx = 0; partition_idx < std::size(partitions); ++partition_idx)
            {
                Partition const& partition{partitions[partition_idx]};
                lue_hpx_assert(partition.is_ready());

                auto read_partition =
                    [policies, partition, create_hyperslab, array, memory_datatype, transfer_property_list]()
                    -> Partition
                {
                    auto partition_ptr{detail::ready_component_ptr(partition)};
                    auto& partition_server{*partition_ptr};
                    Element* buffer{partition_server.data().data()};

                    array.read(
                        memory_datatype, create_hyperslab(partition_server), transfer_property_list, buffer);

                    // TODO Use no-data policy
                    // If no-data in the dataset, write no-data to the partition

                    return partition;
                };

                // TODO In parallel I/O context: spawn as fast as possible

                if (partition_idx == 0)
                {
                    std::tie(new_partitions[partition_idx], partitions_read_f[partition_idx]) =
                        hpx::split_future(hpx::async(
                            [read_partition]() -> std::tuple<Partition, bool> {
                                return {read_partition(), true};
                            }));
                }
                else
                {
                    lue_hpx_assert(new_partitions[partition_idx - 1].valid());
                    std::tie(new_partitions[partition_idx], partitions_read_f[partition_idx]) =
                        hpx::split_future(
                            new_partitions[partition_idx - 1]
                                .then(/* TODO sync? */
                                      [read_partition](
                                          [[maybe_unused]] Partition const& previous_new_partition)
                                          -> std::tuple<Partition, bool> {
                                          return {read_partition(), true};
                                      }));
                }

                lue_hpx_assert(new_partitions[partition_idx].valid());
                lue_hpx_assert(partitions_read_f[partition_idx].valid());
            }

            return {std::move(new_partitions), hpx::when_all(std::move(partitions_read_f))};
        }


        auto keep_dataset_open_until_all_partitions_read(
            hpx::future<void>&& all_partitions_read_f, hpx::future<data_model::Dataset>&& dataset_f)
        {
            // Once the partitions are ready, the dataset is not needed anymore. Until then, we need to keep
            // the dataset instance alive.

            all_partitions_read_f.then(
                [dataset_f = std::move(dataset_f)]([[maybe_unused]] auto&& all_partitions_read_f) mutable
                {
                    HPX_UNUSED(dataset_f);

                    // Just let the dataset_f go out of scope. We are the only one still holding on
                    // to it.
                    // We only get here once the partitions are ready, so by definition the dataset_f
                    // is ready.
                    lue_hpx_assert(dataset_f.is_ready());
                });
        }


        template<typename Policies, typename Partitions>
        auto read_partitions_constant(
            Policies const& policies,
            std::string const& array_pathname,
            hdf5::Offset const& array_hyperslab_start,  // Only needed to offset block read from array
            data_model::ID const object_id,
            Partitions const& partitions) -> hpx::future<Partitions>
        {
            using Partition = typename Partitions::value_type;
            using PartitionServer = Partition::Server;

            auto [dataset_f, partitions_f, all_partitions_read_f] = hpx::split_future(hpx::dataflow(
                hpx::launch::async,
                hpx::unwrapping(
                    [policies, array_pathname, array_hyperslab_start, object_id](
                        std::vector<Partition>&& partitions)
                    {
                        auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
                            parse_array_pathname(array_pathname);
                        auto dataset = open_dataset(dataset_pathname, H5F_ACC_RDONLY);

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
                        auto const& property{
                            property_set.properties().collection<Properties>()[property_name]};

                        auto create_hyperslab =
                            [array_hyperslab_start](PartitionServer const& partition_server)
                        { return hyperslab(array_hyperslab_start, partition_server); };

                        auto [partitions_f, all_partitions_read_f] = read_partitions(
                            policies, property, create_hyperslab, object_id, std::move(partitions));

                        return std::make_tuple(
                            std::move(dataset), std::move(partitions_f), std::move(all_partitions_read_f));
                    }),
                hpx::when_all(partitions.begin(), partitions.end())));

            keep_dataset_open_until_all_partitions_read(
                std::move(all_partitions_read_f), std::move(dataset_f));

            return partitions_f;
        }


        template<typename Policies, typename Partitions>
        auto read_partitions_variable(
            Policies const& policies,
            std::string const& array_pathname,
            hdf5::Offset const& array_hyperslab_start,
            data_model::ID const object_id,
            Index const time_step_idx,
            Partitions const& partitions) -> hpx::future<Partitions>
        {
            using Partition = typename Partitions::value_type;
            using PartitionServer = Partition::Server;

            auto [dataset_f, partitions_f, all_partitions_read_f] = hpx::split_future(hpx::dataflow(
                hpx::launch::async,
                hpx::unwrapping(
                    [policies, array_pathname, array_hyperslab_start, object_id, time_step_idx](
                        std::vector<Partition>&& partitions)
                    {
                        auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
                            parse_array_pathname(array_pathname);
                        auto dataset = open_dataset(dataset_pathname, H5F_ACC_RDONLY);

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
                            data_model::ValueVariability::variable);
                        lue_hpx_assert(
                            property_set.properties().shape_variability(property_name) ==
                            data_model::ShapeVariability::constant);
                        using Properties = data_model::different_shape::constant_shape::Properties;
                        auto const& property{
                            property_set.properties().collection<Properties>()[property_name]};

                        auto create_hyperslab =
                            [array_hyperslab_start, time_step_idx](PartitionServer const& partition_server)
                        { return hyperslab(array_hyperslab_start, partition_server, 0, time_step_idx); };

                        auto [partitions_f, all_partitions_read_f] = read_partitions(
                            policies, property, create_hyperslab, object_id, std::move(partitions));

                        return std::make_tuple(
                            std::move(dataset), std::move(partitions_f), std::move(all_partitions_read_f));
                    }),
                hpx::when_all(partitions.begin(), partitions.end())));

            keep_dataset_open_until_all_partitions_read(
                std::move(all_partitions_read_f), std::move(dataset_f));

            return partitions_f;
        }


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
            PartitionedArray<policy::OutputElementT<Policies>, rank>&& array)
            -> PartitionedArray<policy::OutputElementT<Policies>, rank>
        {
            using Element = policy::OutputElementT<Policies>;
            using Array = PartitionedArray<Element, rank>;
            using Partition = PartitionT<Array>;

            // Group partitions by locality
            auto const partition_idxs_by_locality{detail::partition_idxs_by_locality(array)};
            lue_hpx_assert(partition_idxs_by_locality.size() <= hpx::find_all_localities().size());

            using Action = ReadPartitionsConstantAction<Policies, std::vector<Partition>>;
            Action action{};

            // Iterate over all grouped partitions
            for (auto const& [locality, partition_idxs] : partition_idxs_by_locality)
            {
                // Move current partitions from input array into new collection
                std::vector<Partition> partitions{};
                partitions.resize(partition_idxs.size());

                for (std::size_t idx = 0; auto const partition_idx : partition_idxs)
                {
                    partitions[idx++] = std::move(array.partitions()[partition_idx]);
                }

                // Spawn a task that reads from the dataset into the current partitions. This returns a
                // collection of futures to partitions, each of which becomes ready once its data is read.
                std::vector<hpx::future<Partition>> partition_fs = hpx::split_future<Partition>(
                    hpx::async(
                        action,
                        locality,
                        policies,
                        array_pathname,
                        array_hyperslab_start,
                        object_id,
                        std::move(partitions)),
                    std::size(partition_idxs));

                // Iterate over each selected partition
                for (std::size_t idx = 0; auto const partition_idx : partition_idxs)
                {
                    // Replace the current partition with a new one which becomes ready once the reading has
                    // finished. Under the hood the "new" partition is the same and the original one, but
                    // this is not relevant for the caller.
                    array.partitions()[partition_idx] = partition_fs[idx++].then(
                        [](auto&& partition_f)
                        {
                            lue_hpx_assert(partition_f.valid());
                            lue_hpx_assert(partition_f.is_ready());

                            Partition partition{partition_f.get()};

                            lue_hpx_assert(partition.valid());

                            return partition;
                        });
                }
            }

            lue_hpx_assert(std::all_of(
                array.partitions().begin(),
                array.partitions().end(),
                [](auto const& partition) { return partition.valid(); }));

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
            PartitionedArray<policy::OutputElementT<Policies>, rank>&& array)
            -> PartitionedArray<policy::OutputElementT<Policies>, rank>
        {
            using Element = policy::OutputElementT<Policies>;
            using Array = PartitionedArray<Element, rank>;
            using Partition = PartitionT<Array>;

            // Group partitions by locality
            auto const partition_idxs_by_locality{detail::partition_idxs_by_locality(array)};
            lue_hpx_assert(partition_idxs_by_locality.size() <= hpx::find_all_localities().size());

            using Action = ReadPartitionsVariableAction<Policies, std::vector<Partition>>;
            Action action{};

            // Iterate over all grouped partitions
            for (auto const& [locality, partition_idxs] : partition_idxs_by_locality)
            {
                // Move current partitions from input array into new collection
                std::vector<Partition> partitions{};
                partitions.resize(partition_idxs.size());

                for (std::size_t idx = 0; auto const partition_idx : partition_idxs)
                {
                    partitions[idx++] = std::move(array.partitions()[partition_idx]);
                }

                // Spawn a task that reads from the dataset into the current partitions. This returns a
                // collection of futures to partitions, each of which becomes ready once its data is read.
                std::vector<hpx::future<Partition>> partition_fs = hpx::split_future<Partition>(
                    hpx::async(
                        action,
                        locality,
                        policies,
                        array_pathname,
                        array_hyperslab_start,
                        object_id,
                        time_step_idx,
                        std::move(partitions)),
                    std::size(partition_idxs));

                // Iterate over each selected partition
                for (std::size_t idx = 0; auto const partition_idx : partition_idxs)
                {
                    // Replace the current partition with a new one which becomes ready once the reading has
                    // finished. Under the hood the "new" partition is the same and the original one, but
                    // this is not relevant for the caller.
                    array.partitions()[partition_idx] = partition_fs[idx++].then(
                        [](auto&& partition_f)
                        {
                            lue_hpx_assert(partition_f.valid());
                            lue_hpx_assert(partition_f.is_ready());

                            Partition partition{partition_f.get()};

                            lue_hpx_assert(partition.valid());

                            return partition;
                        });
                }
            }

            lue_hpx_assert(std::all_of(
                array.partitions().begin(),
                array.partitions().end(),
                [](auto const& partition) { return partition.valid(); }));

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
