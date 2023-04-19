#pragma once
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/io/configure.hpp"
#include "lue/framework/io/util.hpp"
#include "lue/data_model/hl/raster_view.hpp"
#include "lue/data_model/hl/util.hpp"
#include "lue/data_model.hpp"
#ifdef LUE_USE_PARALLEL_IO
// Only available in case MPI is used in HPX
#include <hpx/mpi_base/mpi_environment.hpp>
#endif


namespace lue {
    namespace detail {

        template<typename Element, Rank rank>
        Array<Element, rank> read_block(
            data_model::Array const& array,
            hdf5::Dataset::TransferPropertyList const& transfer_property_list,
            hdf5::Hyperslab const& block_hyperslab,
            ShapeT<Array<Element, rank>> const& block_shape)
        {
            using Block = Array<Element, rank>;

            Block block{block_shape};
            hdf5::Datatype const memory_datatype{hdf5::native_datatype<Element>()};

            // Read values into the block
            array.read(memory_datatype, block_hyperslab, transfer_property_list, block.data());

            // TODO Use no-data policy
            // If no-data in the dataset, write no-data to the partition

            return block;
        }


        template<typename Partition>
        void copy_partition_elements(
            Array<ElementT<Partition>, rank<Partition>> const& block,
            Offset<Index, rank<Partition>> const& block_offset,
            PartitionTuples<OffsetT<Partition>, ShapeT<Partition>, ElementT<Partition>> const&
                partition_tuples)
        {
            // block_offset: Offset of the block within the array
            // hyperslab. It is required to be able to transpate partition
            // offsets, which are relative to the array hyperslab, to
            // offsets within the block read.

            using Offset = OffsetT<Partition>;
            using Slice = typename Array<ElementT<Partition>, rank<Partition>>::Slice;

            // Copy hyperslabs from block into the individual partitions
            for (auto& [partition_offset, partition_shape, partition_buffer] : partition_tuples)
            {
                // Select hyperslab corresponding to this partition from
                // block and copy the values to the partition's buffer

                // Offset of the partition relative to the block!
                Offset partition_block_offset{
                    std::get<0>(partition_offset) - std::get<0>(block_offset),
                    std::get<1>(partition_offset) - std::get<1>(block_offset)};

                Slice partition_row_slice{
                    std::get<0>(partition_block_offset),
                    std::get<0>(partition_block_offset) + std::get<0>(partition_shape)};
                Slice partition_col_slice{
                    std::get<1>(partition_block_offset),
                    std::get<1>(partition_block_offset) + std::get<1>(partition_shape)};

                copy(submdspan(block.span(), partition_row_slice, partition_col_slice), partition_buffer);
            }
        }


        template<typename Policies, typename Partitions>
        void read_into_partitions(
            Policies const& /* policies */,
            std::string const& array_pathname,
            hdf5::Offset const& array_hyperslab_start,  // Only needed to offset block read from array
            data_model::ID const object_id,
            Partitions&& partitions)
        {
            auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
                parse_array_pathname(array_pathname);

            // Open dataset. Configure for use of parallel I/O if necessary.
            hdf5::File::AccessPropertyList access_property_list{};

#ifdef LUE_USE_PARALLEL_IO
            if (hpx::util::mpi_environment::enabled())
            {
                // // ::MPI_Comm communicator{hpx::util::mpi_environment::communicator()};
                // ::MPI_Comm communicator{MPI_COMM_WORLD};
                // ::MPI_Info info{MPI_INFO_NULL};
                // access_property_list.use_mpi_communicator(communicator, info);
            }
#endif

            auto const dataset{
                data_model::open_dataset(dataset_pathname, H5F_ACC_RDONLY, access_property_list)};

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

            // Open value. Configure for use of parallel I/O if necessary.
            hdf5::Dataset::TransferPropertyList transfer_property_list{};

#ifdef LUE_USE_PARALLEL_IO
            // if(hpx::util::mpi_environment::enabled())
            // {
            //     // Use collective I/O
            //     transfer_property_list.set_transfer_mode(::H5FD_MPIO_COLLECTIVE);
            // }
#endif

            auto const& value{property.value()};
            auto const array{value[object_id]};

            // Iterate over partitions and read each partition's piece from the dataset
            using Partition = typename Partitions::value_type;
            using Element = ElementT<Partition>;
            Rank const rank{lue::rank<Partition>};

            // Read the whole block containing the partitions and
            // distribute parts of it over the partitions. This is faster
            // than reading small blocks per individual partition. At the
            // cost of reading too much and having to allocate more memory.
            // NOTE: We assume here that the partitions are clustered...
            {
                using Block = Array<Element, rank>;

                auto [partition_tuples, block_offset, block_shape] = detail::partition_tuples(partitions);
                Block block{read_block<Element, rank>(
                    array,
                    transfer_property_list,
                    block_hyperslab(array_hyperslab_start, block_offset, block_shape),
                    block_shape)};
                copy_partition_elements<Partition>(block, block_offset, partition_tuples);
            }


            // Approach reading partitions individually
            // for(Partition const& partition: partitions)
            // {
            //     auto partition_ptr{detail::ready_component_ptr(partition)};
            //     PartitionServer& partition_server{*partition_ptr};
            //     Element* buffer{partition_server.data().data()};

            //     array.read(
            //         memory_datatype,
            //         hyperslab(array_hyperslab_start, partition_server),
            //         transfer_property_list, buffer);

            //     // TODO Use no-data policy
            //     // If no-data in the dataset, write no-data to the partition
            // }
        }


        template<typename Policies, typename Partitions>
        void read_into_partitions2(
            Policies const& /* policies */,
            std::string const& array_pathname,
            hdf5::Offset const& array_hyperslab_start,
            data_model::ID const object_id,
            Index const time_step_idx,
            Partitions&& partitions)
        {
            auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
                parse_array_pathname(array_pathname);

            // Open dataset. Configure for use of parallel I/O if necessary.
            hdf5::File::AccessPropertyList access_property_list{};

#ifdef LUE_USE_PARALLEL_IO
            // if(hpx::util::mpi_environment::enabled())
            // {
            //     // ::MPI_Comm communicator{hpx::util::mpi_environment::communicator()};
            //     ::MPI_Comm communicator{MPI_COMM_WORLD};
            //     ::MPI_Info info{MPI_INFO_NULL};
            //     access_property_list.use_mpi_communicator(communicator, info);
            // }
#endif

            auto const dataset{
                data_model::open_dataset(dataset_pathname, H5F_ACC_RDONLY, access_property_list)};

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
            auto const& property{property_set.properties().collection<Properties>()[property_name]};

            // Open value. Configure for use of parallel I/O if necessary.
            hdf5::Dataset::TransferPropertyList transfer_property_list{};

#ifdef LUE_USE_PARALLEL_IO
            // if(hpx::util::mpi_environment::enabled())
            // {
            //     // Use collective I/O
            //     transfer_property_list.set_transfer_mode(::H5FD_MPIO_COLLECTIVE);
            // }
#endif

            // Iterate over partitions and read each partition's piece from the dataset
            auto const& value{property.value()};
            auto const array{value[object_id]};

            using Partition = typename Partitions::value_type;
            using Element = ElementT<Partition>;
            Rank const rank{lue::rank<Partition>};

            {
                using Block = Array<Element, rank>;

                auto [partition_tuples, block_offset, block_shape] = detail::partition_tuples(partitions);
                Block block{read_block<Element, rank>(
                    array,
                    transfer_property_list,
                    block_hyperslab(array_hyperslab_start, time_step_idx, block_offset, block_shape),
                    block_shape)};

                copy_partition_elements<Partition>(block, block_offset, partition_tuples);
            }


            // Approach reading partitions individually
            // for(Partition const& partition: partitions)
            // {
            //     auto partition_ptr{detail::ready_component_ptr(partition)};
            //     PartitionServer& partition_server{*partition_ptr};
            //     Element* buffer{partition_server.data().data()};

            //     array.read(
            //         memory_datatype,
            //         hyperslab(array_hyperslab_start, partition_server, 0, time_step_idx),
            //         transfer_property_list, buffer);

            //     // TODO Use no-data policy
            //     // If no-data in the dataset, write no-data to the partition
            // }
        }


        template<typename Policies, typename Partitions>
        struct ReadIntoPartitionsAction:
            hpx::actions::make_action<
                decltype(&read_into_partitions<Policies, Partitions>),
                &read_into_partitions<Policies, Partitions>,
                ReadIntoPartitionsAction<Policies, Partitions>>::type
        {
        };


        template<typename Policies, typename Partitions>
        struct ReadIntoPartitionsAction2:
            hpx::actions::make_action<
                decltype(&read_into_partitions2<Policies, Partitions>),
                &read_into_partitions2<Policies, Partitions>,
                ReadIntoPartitionsAction2<Policies, Partitions>>::type
        {
        };

    }  // namespace detail


    template<typename Policies, typename Element, Rank rank>
    [[nodiscard]] hpx::future<void> read_into(
        Policies const& policies,
        std::string const& array_pathname,
        hdf5::Offset const& array_hyperslab_start,
        data_model::ID const object_id,
        PartitionedArray<Element, rank>& array)
    {
        // TODO Can this function be made fully asynchronous? Currently the
        //     array can only be used once all partitions have been read. It
        //     would be nice to be able to already work with those partitions
        //     that have been read already.

        using Array = PartitionedArray<Element, rank>;
        using Partition = PartitionT<Array>;

        auto partitions_by_locality{detail::partitions_by_locality(array)};
        lue_hpx_assert(partitions_by_locality.size() <= hpx::find_all_localities().size());

        // -------------------------------------------------------------------------
        // Iterate over each locality and attach a continuation to all its
        // partitions to read into.

        // Wait for each locality to have finished reading before telling
        // the next one to start reading. All this must work with a
        // non-threadsafe HDF5 library.
        {
            using Action = detail::ReadIntoPartitionsAction<Policies, std::vector<Partition>>;
            Action action{};

            for (auto& [locality, partitions] : partitions_by_locality)
            {
                hpx::dataflow(
                    hpx::launch::async,
                    hpx::unwrapping(

                        [locality = locality,
                         action,
                         policies,
                         array_pathname,
                         array_hyperslab_start,
                         object_id](std::vector<Partition>&& partitions)
                        {
                            return action(
                                locality,
                                policies,
                                array_pathname,
                                array_hyperslab_start,
                                object_id,
                                std::move(partitions));
                        }

                        ),

                    hpx::when_all(partitions.begin(), partitions.end()))
                    .get();
            }
        }

        return hpx::make_ready_future<void>();


        // std::vector<hpx::future<void>> reads_per_locality;
        // {
        //     using Action = detail::ReadIntoPartitionsAction<Policies, std::vector<Partition>>;
        //     Action action{};

        //     reads_per_locality.reserve(std::size(partitions_by_locality));

        //     for(auto& [locality, partitions]: partitions_by_locality)
        //     {
        //         reads_per_locality.push_back(
        //             hpx::dataflow(
        //                 hpx::launch::async,
        //                 hpx::unwrapping(

        //                         [locality, action, policies, array_pathname, object_id](
        //                             std::vector<Partition> const& partitions)
        //                         {
        //                             return action(locality, policies, array_pathname, object_id,
        //                             partitions);
        //                         }

        //                     ),

        //                 hpx::when_all(partitions.begin(), partitions.end())));
        //     }
        // }

        // return detail::when_all_get(std::move(reads_per_locality));
    }


    // template<
    //     typename Policies,
    //     typename Element,
    //     Rank rank>
    // [[nodiscard]] hpx::future<void> read_into(
    //     Policies const& policies,
    //     std::string const& array_pathname,
    //     data_model::ID const object_id,
    //     PartitionedArray<Element, rank>& array)
    // {
    //     // TODO Can this function be made fully asynchronous? Currently the
    //     //     array can only be used once all partitions have been read. It
    //     //     would be nice to be able to already work with those partitions
    //     //     that have been read already.

    //     using Array = PartitionedArray<Element, rank>;
    //     using Partition = PartitionT<Array>;

    //     auto partitions_by_locality{detail::partitions_by_locality(array)};
    //     lue_hpx_assert(partitions_by_locality.size() <= hpx::find_all_localities().size());

    //     // -------------------------------------------------------------------------
    //     // Iterate over each locality and attach a continuation to all its
    //     // partitions to read into.

    //     // Wait for each locality to have finished reading before telling
    //     // the next one to start reading. All this must work with a
    //     // non-threadsafe HDF5 library.
    //     {
    //         using Action = detail::ReadIntoPartitionsAction<Policies, std::vector<Partition>>;
    //         Action action{};

    //         for(auto& [locality, partitions]: partitions_by_locality)
    //         {
    //             hpx::dataflow(
    //                 hpx::launch::async,
    //                 hpx::unwrapping(

    //                         [locality, action, policies, array_pathname, object_id](
    //                             std::vector<Partition> const& partitions)
    //                         {
    //                             return action(locality, policies, array_pathname, object_id, partitions);
    //                         }

    //                     ),

    //                 hpx::when_all(partitions.begin(), partitions.end())).get();
    //         }
    //     }

    //     return hpx::make_ready_future<void>();


    //     // std::vector<hpx::future<void>> reads_per_locality;
    //     // {
    //     //     using Action = detail::ReadIntoPartitionsAction<Policies, std::vector<Partition>>;
    //     //     Action action{};

    //     //     reads_per_locality.reserve(std::size(partitions_by_locality));

    //     //     for(auto& [locality, partitions]: partitions_by_locality)
    //     //     {
    //     //         reads_per_locality.push_back(
    //     //             hpx::dataflow(
    //     //                 hpx::launch::async,
    //     //                 hpx::unwrapping(

    //     //                         [locality, action, policies, array_pathname, object_id](
    //     //                             std::vector<Partition> const& partitions)
    //     //                         {
    //     //                             return action(locality, policies, array_pathname, object_id,
    //     partitions);
    //     //                         }

    //     //                     ),

    //     //                 hpx::when_all(partitions.begin(), partitions.end())));
    //     //     }
    //     // }

    //     // return detail::when_all_get(std::move(reads_per_locality));
    // }


    template<typename Policies, typename Element, Rank rank>
    [[nodiscard]] hpx::future<void> read_into(
        Policies const& policies,
        std::string const& array_pathname,
        hdf5::Offset const& array_hyperslab_start,
        data_model::ID const object_id,
        Index const time_step_idx,
        PartitionedArray<Element, rank>& array)
    {
        // TODO Can this function be made fully asynchronous? Currently the
        //     array can only be used once all partitions have been read. It
        //     would be nice to be able to already work with those partitions
        //     that have been read already.

        using Array = PartitionedArray<Element, rank>;
        using Partition = PartitionT<Array>;

        auto partitions_by_locality{detail::partitions_by_locality(array)};
        lue_hpx_assert(partitions_by_locality.size() <= hpx::find_all_localities().size());

        // -------------------------------------------------------------------------
        // Iterate over each locality and attach a continuation to all its
        // partitions to read into.

        {
            using Action = detail::ReadIntoPartitionsAction2<Policies, std::vector<Partition>>;
            Action action{};

            for (auto& [locality, partitions] : partitions_by_locality)
            {
                hpx::dataflow(
                    hpx::launch::async,
                    hpx::unwrapping(

                        [locality = locality,
                         action,
                         policies,
                         array_pathname,
                         array_hyperslab_start,
                         object_id,
                         time_step_idx](std::vector<Partition>&& partitions)
                        {
                            return action(
                                locality,
                                policies,
                                array_pathname,
                                array_hyperslab_start,
                                object_id,
                                time_step_idx,
                                std::move(partitions));
                        }

                        ),

                    hpx::when_all(partitions.begin(), partitions.end()))
                    .get();
            }
        }

        return hpx::make_ready_future<void>();


        // std::vector<hpx::future<void>> reads_per_locality;
        // {
        //     using Action = detail::ReadIntoPartitionsAction2<Policies, std::vector<Partition>>;
        //     Action action{};

        //     reads_per_locality.reserve(std::size(partitions_by_locality));

        //     for(auto& [locality, partitions]: partitions_by_locality)
        //     {
        //         reads_per_locality.push_back(
        //             hpx::dataflow(
        //                 hpx::launch::async,
        //                 hpx::unwrapping(

        //                         [locality, action, policies, array_pathname, object_id, time_step_idx](
        //                             std::vector<Partition> const& partitions)
        //                         {
        //                             return action(
        //                                 locality, policies, array_pathname, object_id, time_step_idx,
        //                                 partitions);
        //                         }

        //                     ),

        //                 hpx::when_all(partitions.begin(), partitions.end())));
        //     }
        // }

        // return detail::when_all_get(std::move(reads_per_locality));
    }


    namespace detail {

        template<typename Count, Rank rank>
        Shape<Count, rank> constant_array_shape(std::string const& array_pathname)
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

            using Shape = lue::Shape<Count, rank>;

            lue_hpx_assert(rank == 2);  // TODO(KDJ)

            return Shape{
                {static_cast<typename Shape::value_type>(grid_shape[0]),
                 static_cast<typename Shape::value_type>(grid_shape[1])}};
        }


        template<typename Count, Rank rank>
        Shape<Count, rank> variable_array_shape(std::string const& array_pathname)
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

            using Shape = lue::Shape<Count, rank>;

            lue_hpx_assert(rank == 2);  // TODO(KDJ)

            return Shape{
                {static_cast<typename Shape::value_type>(grid_shape[0]),
                 static_cast<typename Shape::value_type>(grid_shape[1])}};
        }

    }  // namespace detail


    namespace policy::read_into {

        template<typename OutputElement>
        using DefaultPolicies =
            policy::DefaultPolicies<AllValuesWithinDomain<>, OutputElements<OutputElement>, InputElements<>>;


        template<typename OutputElement>
        using DefaultValuePolicies = policy::
            DefaultValuePolicies<AllValuesWithinDomain<>, OutputElements<OutputElement>, InputElements<>>;

    }  // namespace policy::read_into


    template<typename Element, typename Policies, Rank rank>
    PartitionedArray<Element, rank> read(
        Policies const& policies,
        std::string const& array_pathname,
        hdf5::Hyperslab const& hyperslab,
        ShapeT<PartitionedArray<Element, rank>> const& partition_shape,
        data_model::ID const object_id)
    {
        using Array = PartitionedArray<Element, rank>;
        using Functor = InstantiateDefaultInitialized<Element, rank>;
        using Shape = ShapeT<Array>;

        // Create partitioned array
        Shape const array_shape{detail::hyperslab_to_shape<Shape>(hyperslab)};
        Array array{create_partitioned_array(policies, array_shape, partition_shape, Functor{})};

        // Read elements from dataset into array
        // TODO(KDJ) This blocks until the data is read. Can we do better?
        read_into(policies, array_pathname, hyperslab.start(), object_id, array).get();

        return array;
    }


    template<typename Element, Rank rank>
    PartitionedArray<Element, rank> read(
        std::string const& array_pathname,
        hdf5::Hyperslab const& hyperslab,
        ShapeT<PartitionedArray<Element, rank>> const& partition_shape,
        data_model::ID const object_id)
    {
        using Policies = policy::read_into::DefaultPolicies<Element>;

        return read<Element, Policies, rank>(
            Policies{}, array_pathname, hyperslab, partition_shape, object_id);
    }


    template<typename Element, typename Policies, Rank rank>
    PartitionedArray<Element, rank> read(
        Policies const& policies,
        std::string const& array_pathname,
        ShapeT<PartitionedArray<Element, rank>> const& partition_shape,
        data_model::ID const object_id)
    {
        using Array = PartitionedArray<Element, rank>;
        using Shape = ShapeT<Array>;

        Shape const array_shape{detail::constant_array_shape<Count, rank>(array_pathname)};

        return read<Element, Policies, rank>(
            policies, array_pathname, detail::shape_to_hyperslab(array_shape), partition_shape, object_id);
    }


    template<typename Element, Rank rank>
    PartitionedArray<Element, rank> read(
        std::string const& array_pathname,
        ShapeT<PartitionedArray<Element, rank>> const& partition_shape,
        data_model::ID const object_id)
    {
        using Policies = policy::read_into::DefaultPolicies<Element>;

        return read<Element, Policies, rank>(Policies{}, array_pathname, partition_shape, object_id);
    }


    // -------------------------------------------------------------------------


    template<typename Element, typename Policies, Rank rank>
    PartitionedArray<Element, rank> read(
        Policies const& policies,
        std::string const& array_pathname,
        hdf5::Hyperslab const& hyperslab,
        ShapeT<PartitionedArray<Element, rank>> const& partition_shape,
        data_model::ID const object_id,
        Index const time_step_idx)
    {
        using Array = PartitionedArray<Element, rank>;
        using Functor = InstantiateDefaultInitialized<Element, rank>;
        using Shape = ShapeT<Array>;

        // Create partitioned array
        Shape const array_shape{detail::hyperslab_to_shape<Shape>(hyperslab)};
        Array array{create_partitioned_array(policies, array_shape, partition_shape, Functor{})};

        // Read elements from dataset into array
        // TODO(KDJ) This blocks until the data is read. Can we do better?
        read_into(policies, array_pathname, hyperslab.start(), object_id, time_step_idx, array).get();

        return array;
    }


    template<typename Element, Rank rank>
    PartitionedArray<Element, rank> read(
        std::string const& array_pathname,
        hdf5::Hyperslab const& hyperslab,
        ShapeT<PartitionedArray<Element, rank>> const& partition_shape,
        data_model::ID const object_id,
        Index const time_step_idx)
    {
        using Policies = policy::read_into::DefaultPolicies<Element>;

        return read<Element, Policies, rank>(
            Policies{}, array_pathname, hyperslab, partition_shape, object_id, time_step_idx);
    }


    template<typename Element, typename Policies, Rank rank>
    PartitionedArray<Element, rank> read(
        Policies const& policies,
        std::string const& array_pathname,
        ShapeT<PartitionedArray<Element, rank>> const& partition_shape,
        data_model::ID const object_id,
        Index const time_step_idx)
    {
        using Array = PartitionedArray<Element, rank>;
        using Shape = ShapeT<Array>;

        Shape const array_shape{detail::variable_array_shape<Count, rank>(array_pathname)};

        return read<Element, Policies, rank>(
            policies,
            array_pathname,
            detail::shape_to_hyperslab(array_shape),
            partition_shape,
            object_id,
            time_step_idx);
    }


    template<typename Element, Rank rank>
    PartitionedArray<Element, rank> read(
        std::string const& array_pathname,
        ShapeT<PartitionedArray<Element, rank>> const& partition_shape,
        data_model::ID const object_id,
        Index const time_step_idx)
    {
        using Policies = policy::read_into::DefaultPolicies<Element>;

        return read<Element, Policies, rank>(
            Policies{}, array_pathname, partition_shape, object_id, time_step_idx);
    }

}  // namespace lue
