#pragma once
#include "lue/framework/io/configure.hpp"
#include "lue/framework/io/util.hpp"
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/core/component.hpp"
#include "lue/data_model.hpp"
#include "lue/data_model/hl/util.hpp"
#ifdef LUE_USE_PARALLEL_IO
// Only available in case MPI is used in HPX
#include <hpx/mpi_base/mpi_environment.hpp>
#endif


namespace lue {
    namespace detail {


        // // this function will be executed by a dedicated OS thread
        // void do_async_io(char const* string_to_write, int& result)
        // {
        //     // This IO operation will possibly block the IO thread in the
        //     // kernel.
        //     std::cout << "OS-thread: " << string_to_write << std::endl;

        //     result = 1;
        // }

        // // This function will be executed by an HPX thread
        // int async_io(char const* string_to_write)
        // {
        //     int result = 0;

        //     {
        //         // Get a reference to one of the IO specific HPX io_service objects ...
        //         hpx::parallel::execution::io_pool_executor executor;

        //         // ... and schedule the handler to run on one of its OS-threads.
        //         hpx::async(executor, &do_async_io, string_to_write, std::ref(result))
        //             .get();
        //     }

        //     HPX_ASSERT(result == 1);

        //     return result;    // this will be executed only after result has been set
        // }



///         template<
///             typename Policies,
///             typename Partitions>
///         void write_partitions_on_os_thread(
///             Policies const& /* policies */,
///             Partitions const& partitions,
///             std::string const& array_pathname,
///             data_model::ID const object_id)
///         {
///             auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
///                 parse_array_pathname(array_pathname);
/// 
///             // Open dataset. Configure for use of parallel I/O if necessary.
///             hdf5::File::AccessPropertyList access_property_list{};
/// 
/// #ifdef LUE_USE_PARALLEL_IO
///             if(hpx::util::mpi_environment::enabled())
///             {
///                 // // ::MPI_Comm communicator{hpx::util::mpi_environment::communicator()};
///                 // ::MPI_Comm communicator{MPI_COMM_WORLD};
///                 // ::MPI_Info info{MPI_INFO_NULL};
///                 // access_property_list.use_mpi_communicator(communicator, info);
///             }
/// #endif
/// 
///             auto dataset{data_model::open_dataset(dataset_pathname, H5F_ACC_RDWR, access_property_list)};
/// 
///             // Open phenomenon
///             auto& phenomenon{dataset.phenomena()[phenomenon_name]};
/// 
///             // Open property-set
///             auto& property_set{phenomenon.property_sets()[property_set_name]};
/// 
///             // Open property
///             lue_hpx_assert(property_set.properties().contains(property_name));
///             lue_hpx_assert(property_set.properties().shape_per_object(property_name) ==
///                 data_model::ShapePerObject::different);
///             lue_hpx_assert(property_set.properties().value_variability(property_name) ==
///                 data_model::ValueVariability::constant);
///             using Properties = data_model::different_shape::Properties;
///             auto& property{property_set.properties().collection<Properties>()[property_name]};
/// 
///             // Open value. Configure for use of parallel I/O if necessary.
///             hdf5::Dataset::TransferPropertyList transfer_property_list{};
/// 
/// #ifdef LUE_USE_PARALLEL_IO
///             if(hpx::util::mpi_environment::enabled())
///             {
///                 // // Use collective I/O
///                 // transfer_property_list.set_transfer_mode(::H5FD_MPIO_COLLECTIVE);
///             }
/// #endif
/// 
///             auto& value{property.value()};
///             auto array{value[object_id]};
/// 
///             // Iterate over partitions and write each partition's piece to the dataset
///             using Partition = typename Partitions::value_type;
///             using PartitionServer = typename Partition::Server;
///             using Element = ElementT<Partition>;
/// 
///             for(Partition const& partition: partitions)
///             {
///                 auto partition_ptr{detail::ready_component_ptr(partition)};
///                 PartitionServer const& partition_server{*partition_ptr};
///                 Element const* buffer{partition_server.data().data()};
/// 
///                 array.write(hyperslab(partition_server), transfer_property_list, buffer);
///             }
///         }
/// 
/// 
///         template<
///             typename Policies,
///             typename Partitions>
///         void write_partitions(
///             Policies const& policies,
///             Partitions const& partitions,
///             std::string const& array_pathname,
///             data_model::ID const object_id)
///         {
///             // Get a reference to one of the IO specific HPX io_service objects ...
///             hpx::parallel::execution::io_pool_executor executor;
/// 
///             // ... and schedule the handler to run on one of its OS-threads.
///             hpx::async(
///                 executor, &write_partitions_on_os_thread<Policies, Partitions>,
///                 policies, partitions, array_pathname, object_id).get();
///         }


        template<
            typename Element,
            Rank rank>
        void write_block(
            Array<Element, rank> const& block,
            hdf5::Dataspace const& block_dataspace,
            hdf5::Hyperslab const& array_hyperslab,
            data_model::Array& array,
            hdf5::Dataset::TransferPropertyList const& transfer_property_list)
        {
            // Configure selection corresponding with the subset of the block that must be written
            hdf5::Offset block_offset{0, 0};
            ::herr_t status{::H5Sselect_hyperslab(
                block_dataspace.id(), H5S_SELECT_SET,
                block_offset.data(), nullptr, array_hyperslab.count().data(),
                nullptr)};

            if(status < 0) {
                throw std::runtime_error("Cannot create hyperslab");
            }

            // Write values from block to dataset
            array.write(block_dataspace, array_hyperslab, transfer_property_list, block.data());

            // TODO Use no-data policy
            // If no-data in the dataset, write no-data to the partition
        }


        // Sort partitions according to the row index first and
        // the column index after that
        auto compare_by_partition_offset =
            [](
                auto const& partition_tuple1,
                auto const& partition_tuple2)
            {
                auto const& offset1{std::get<0>(partition_tuple1)};
                auto const& offset2{std::get<0>(partition_tuple2)};

                // Every partition is an individual (except for Brian)
                lue_hpx_assert(offset1 != offset2);

                // Sort by row. In case the rows are equal, sort by column.
                return std::get<0>(offset1) != std::get<0>(offset2)
                    ? std::get<0>(offset1) < std::get<0>(offset2)
                    : std::get<1>(offset1) < std::get<1>(offset2)
                    ;
            };


        template<
            typename Policies,
            typename Partitions>
        void write_partitions(
            Policies const& /* policies */,
            Partitions&& partitions,
            std::string const& array_pathname,
            data_model::ID const object_id)
        {
            auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
                parse_array_pathname(array_pathname);

            // Open dataset. Configure for use of parallel I/O if necessary.
            hdf5::File::AccessPropertyList access_property_list{};

#ifdef LUE_USE_PARALLEL_IO
            if(hpx::util::mpi_environment::enabled())
            {
                // // ::MPI_Comm communicator{hpx::util::mpi_environment::communicator()};
                // ::MPI_Comm communicator{MPI_COMM_WORLD};
                // ::MPI_Info info{MPI_INFO_NULL};
                // access_property_list.use_mpi_communicator(communicator, info);
            }
#endif

            auto dataset{data_model::open_dataset(dataset_pathname, H5F_ACC_RDWR, access_property_list)};

            // Open phenomenon
            auto& phenomenon{dataset.phenomena()[phenomenon_name]};

            // Open property-set
            auto& property_set{phenomenon.property_sets()[property_set_name]};

            // Open property
            lue_hpx_assert(property_set.properties().contains(property_name));
            lue_hpx_assert(property_set.properties().shape_per_object(property_name) ==
                data_model::ShapePerObject::different);
            lue_hpx_assert(property_set.properties().value_variability(property_name) ==
                data_model::ValueVariability::constant);
            using Properties = data_model::different_shape::Properties;
            auto& property{property_set.properties().collection<Properties>()[property_name]};

            // Open value. Configure for use of parallel I/O if necessary.
            hdf5::Dataset::TransferPropertyList transfer_property_list{};

#ifdef LUE_USE_PARALLEL_IO
            if(hpx::util::mpi_environment::enabled())
            {
                // // Use collective I/O
                // transfer_property_list.set_transfer_mode(::H5FD_MPIO_COLLECTIVE);
            }
#endif

            auto& value{property.value()};
            auto array{value[object_id]};

            using Partition = typename Partitions::value_type;
            using Shape = ShapeT<Partition>;
            using Offset = OffsetT<Partition>;
            using Element = ElementT<Partition>;
            Rank const rank{lue::rank<Partition>};

            // For partitions that are located next to each other, we
            // create a single block. We then copy the values for these
            // partitions into this block and write the block in one go to
            // the dataset. Then we move on to the next set of partitions
            // that are located next to each other. This is faster than
            // writing individual partitions, at the cost of the memory
            // needed for the block.
            {
                auto [partition_tuples, _, block_shape] =
                    detail::partition_tuples(partitions, compare_by_partition_offset);


                // The block we need for storing the partition data for
                // a single row of adjacent partitions has a maximum shape
                // of (2 * partition.nr_rows - 1, block_shape.nr_cols)
                lue_hpx_assert(!partition_tuples.empty());
                Count const max_nr_rows_block{(2 * std::get<0>(std::get<1>(partition_tuples[0]))) - 1};
                Count const max_nr_cols_block{std::get<1>(block_shape)};
                Shape const partitions_block_shape{max_nr_rows_block, max_nr_cols_block};

                using Block = Array<Element, rank>;
                using Slice = typename Block::Slice;

                Block block{partitions_block_shape};

                // Dataspace corresponding with the whole block
                hdf5::Count block_count{
                    static_cast<::hsize_t>(block.shape()[0]),
                    static_cast<::hsize_t>(block.shape()[1])};
                hdf5::Dataspace const block_dataspace{hdf5::create_dataspace(
                    hdf5::Shape{block_count.begin(), block_count.end()})};

                // Iterate over the sets of adjacent partitions. Copy
                // the values of each of these partitions into the
                // block row.
                Index partition_idx{0};

                while(partition_idx < static_cast<Index>(partition_tuples.size()))
                {
                    // Start with a new set of adjacent partitions that
                    // are located in the same row in the partitioned array
                    auto [first_partition_offset, partition_shape, partition_buffer] =
                        partition_tuples[partition_idx];

                    lue_hpx_assert(std::get<0>(partition_shape) <= max_nr_rows_block);

                    // Offset relative to array
                    auto [current_partition_row, current_partition_col] = first_partition_offset;

                    // Select hyperslab corresponding to this partition
                    // from the block and copy the elements from the
                    // current partition's buffer

                    // Offset of the current partition, relative to
                    // the block. First partition always starts at (0, 0).
                    Offset partition_block_offset{0, 0};


                    Slice partition_row_slice{
                        std::get<0>(partition_block_offset),
                        std::get<0>(partition_block_offset) + std::get<0>(partition_shape)};
                    Slice partition_col_slice{
                        std::get<1>(partition_block_offset),
                        std::get<1>(partition_block_offset) + std::get<1>(partition_shape)};

                    lue_hpx_assert(std::get<1>(partition_row_slice) <= max_nr_rows_block);
                    lue_hpx_assert(std::get<1>(partition_col_slice) <= max_nr_cols_block);

                    copy(partition_buffer,
                        subspan(block.span(), partition_row_slice, partition_col_slice));

                    // Update offset for next partition. Only update the column.
                    std::get<1>(partition_block_offset) += std::get<1>(partition_shape);



                    // Copy adjacent partitions to the block. An
                    // adjacent partition is located at the same
                    // row offset as the current one and at the next
                    // column offset.
                    while(
                        ++partition_idx < static_cast<Index>(partition_tuples.size()) &&
                        std::get<0>(std::get<0>(partition_tuples[partition_idx])) == current_partition_row &&
                        std::get<1>(std::get<0>(partition_tuples[partition_idx])) == current_partition_col + std::get<1>(partition_shape))
                    {
                        auto [partition_offset, partition_shape, partition_buffer] =
                            partition_tuples[partition_idx];



                        partition_row_slice = Slice{
                            std::get<0>(partition_block_offset),
                            std::get<0>(partition_block_offset) + std::get<0>(partition_shape)};
                        partition_col_slice = Slice{
                            std::get<1>(partition_block_offset),
                            std::get<1>(partition_block_offset) + std::get<1>(partition_shape)};

                        lue_hpx_assert(std::get<1>(partition_row_slice) <= max_nr_rows_block);
                        lue_hpx_assert(std::get<1>(partition_col_slice) <= max_nr_cols_block);

                        copy(partition_buffer,
                            subspan(block.span(), partition_row_slice, partition_col_slice));

                        // Update offset for next partition
                        current_partition_col += std::get<1>(partition_shape);
                        std::get<1>(partition_block_offset) += std::get<1>(partition_shape);


                    }


                    // Write current block contents to the dataset
                    write_block<Element, rank>(
                        block,
                        block_dataspace,
                        hyperslab(
                            // Offset of the first partition in the array
                            first_partition_offset,
                            // Shape of the row of partitions in the block
                            Shape{
                                std::get<0>(partition_shape),
                                static_cast<Count>(std::get<1>(partition_block_offset))}),
                        array, transfer_property_list);
                }

                lue_hpx_assert(partition_idx == static_cast<Index>(partition_tuples.size()));
            }


            // Approach writing partitions individually
            // // Iterate over partitions and write each partition's piece to the dataset
            // using Partition = typename Partitions::value_type;
            // using PartitionServer = typename Partition::Server;
            // using Element = ElementT<Partition>;

            // for(Partition const& partition: partitions)
            // {
            //     auto partition_ptr{detail::ready_component_ptr(partition)};
            //     PartitionServer const& partition_server{*partition_ptr};
            //     Element const* buffer{partition_server.data().data()};

            //     array.write(hyperslab(partition_server), transfer_property_list, buffer);
            // }
        }


        template<
            typename Policies,
            typename Partitions>
        void write_partitions2(
            Policies const& /* policies */,
            Partitions&& partitions,
            std::string const& array_pathname,
            data_model::ID const object_id,
            Index const time_step_idx)
        {
            auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
                parse_array_pathname(array_pathname);

            // Open dataset. Configure for use of parallel I/O if necessary.
            hdf5::File::AccessPropertyList access_property_list{};

#ifdef LUE_USE_PARALLEL_IO
            if(hpx::util::mpi_environment::enabled())
            {
                // // ::MPI_Comm communicator{hpx::util::mpi_environment::communicator()};
                // ::MPI_Comm communicator{MPI_COMM_WORLD};
                // ::MPI_Info info{MPI_INFO_NULL};
                // access_property_list.use_mpi_communicator(communicator, info);
            }
#endif

            auto dataset{data_model::open_dataset(dataset_pathname, H5F_ACC_RDWR, access_property_list)};

            // Open phenomenon
            auto& phenomenon{dataset.phenomena()[phenomenon_name]};

            // Open property-set
            auto& property_set{phenomenon.property_sets()[property_set_name]};

            // Open property
            lue_hpx_assert(property_set.properties().contains(property_name));
            lue_hpx_assert(property_set.properties().shape_per_object(property_name) ==
                data_model::ShapePerObject::different);
            lue_hpx_assert(property_set.properties().value_variability(property_name) ==
                data_model::ValueVariability::variable);
            lue_hpx_assert(property_set.properties().shape_variability(property_name) ==
                data_model::ShapeVariability::constant);
            using Properties = data_model::different_shape::constant_shape::Properties;
            auto const& property{property_set.properties().collection<Properties>()[property_name]};

            // Open value. Configure for use of parallel I/O if necessary.
            hdf5::Dataset::TransferPropertyList transfer_property_list{};

#ifdef LUE_USE_PARALLEL_IO
            if(hpx::util::mpi_environment::enabled())
            {
                // // Use collective I/O
                // transfer_property_list.set_transfer_mode(::H5FD_MPIO_COLLECTIVE);
            }
#endif

            auto& value{property.value()};
            auto array{value[object_id]};

            // TODO
            /// using Partition = typename Partitions::value_type;
            /// using Element = ElementT<Partition>;
            /// Rank const rank{lue::rank<Partition>};

            /// // For partitions that are located next to each other, we
            /// // create a single block. We then copy the values for these
            /// // partitions into this block and write the block in one go to
            /// // the dataset. Then we move on to the next set of partitions
            /// // that are located next to each other. This is faster than
            /// // writing individual partitions, at the cost of the memory
            /// // needed for the block.
            /// {
            ///     auto [partition_tuples, _, block_shape] =
            ///         detail::partition_tuples(partitions, compare_by_partition_offset);


            ///     // Block block{read_block<Element, rank>(
            ///     //     array, transfer_property_list,
            ///     //     block_hyperslab(array_hyperslab_start, block_offset, block_shape), block_shape)};
            ///     // copy_partition_elements<Partition>(block, block_offset, partition_tuples);





            ///     // The block we need for storing the partition data for
            ///     // a single row of adjacent partitions has a maximum shape
            ///     // of (2 * partition.nr_rows - 1, block_shape.nr_cols)
            ///     lue_hpx_assert(!partition_tuples.empty());
            ///     Count const max_nr_rows_block{(2 * std::get<0>(std::get<1>(partition_tuples[0]))) - 1};
            ///     Count const max_nr_cols_block{std::get<1>(block_shape)};
            ///     Shape const partitions_block_shape{max_nr_rows_block, max_nr_cols_block};

            ///     using Block = Array<Element, rank>;
            ///     using Slice = typename Block::Slice;

            ///     Block block{partitions_block_shape};

            ///     // Dataspace corresponding with the whole block
            ///     hdf5::Count block_count{
            ///         // TODO static_cast<::hsize_t>(1),
            ///         static_cast<::hsize_t>(block.shape()[0]),
            ///         static_cast<::hsize_t>(block.shape()[1])};
            ///     hdf5::Dataspace const block_dataspace{hdf5::create_dataspace(
            ///         hdf5::Shape{block_count.begin(), block_count.end()})};

            ///     // Iterate over the sets of adjacent partitions. Copy
            ///     // the values of each of these partitions into the
            ///     // block row.
            ///     Index partition_idx{0};

            ///     while(partition_idx < static_cast<Index>(partition_tuples.size()))
            ///     {
            ///         // Start with a new set of adjacent partitions that
            ///         // are located in the same row in the partitioned array
            ///         auto [first_partition_offset, partition_shape, partition_buffer] =
            ///             partition_tuples[partition_idx];

            ///         lue_hpx_assert(std::get<0>(partition_shape) <= max_nr_rows_block);

            ///         // Offset relative to array
            ///         auto [current_partition_row, current_partition_col] = first_partition_offset;

            ///         // Select hyperslab corresponding to this partition
            ///         // from the block and copy the elements from the
            ///         // current partition's buffer

            ///         // Offset of the current partition, relative to
            ///         // the block. First partition always starts at (0, 0).
            ///         Offset partition_block_offset{0, 0};


            ///         Slice partition_row_slice{
            ///             std::get<0>(partition_block_offset),
            ///             std::get<0>(partition_block_offset) + std::get<0>(partition_shape)};
            ///         Slice partition_col_slice{
            ///             std::get<1>(partition_block_offset),
            ///             std::get<1>(partition_block_offset) + std::get<1>(partition_shape)};

            ///         lue_hpx_assert(std::get<1>(partition_row_slice) <= max_nr_rows_block);
            ///         lue_hpx_assert(std::get<1>(partition_col_slice) <= max_nr_cols_block);

            ///         copy(partition_buffer,
            ///             subspan(block.span(), partition_row_slice, partition_col_slice));

            ///         // Update offset for next partition. Only update the column.
            ///         std::get<1>(partition_block_offset) += std::get<1>(partition_shape);



            ///         // Copy adjacent partitions to the block. An
            ///         // adjacent partition is located at the same
            ///         // row offset as the current one and at the next
            ///         // column offset.
            ///         while(
            ///             ++partition_idx < static_cast<Index>(partition_tuples.size()) &&
            ///             std::get<0>(std::get<0>(partition_tuples[partition_idx])) == current_partition_row &&
            ///             std::get<1>(std::get<0>(partition_tuples[partition_idx])) == current_partition_col + std::get<1>(partition_shape))
            ///         {
            ///             auto [partition_offset, partition_shape, partition_buffer] =
            ///                 partition_tuples[partition_idx];



            ///             partition_row_slice = Slice{
            ///                 std::get<0>(partition_block_offset),
            ///                 std::get<0>(partition_block_offset) + std::get<0>(partition_shape)};
            ///             partition_col_slice = Slice{
            ///                 std::get<1>(partition_block_offset),
            ///                 std::get<1>(partition_block_offset) + std::get<1>(partition_shape)};

            ///             lue_hpx_assert(std::get<1>(partition_row_slice) <= max_nr_rows_block);
            ///             lue_hpx_assert(std::get<1>(partition_col_slice) <= max_nr_cols_block);

            ///             copy(partition_buffer,
            ///                 subspan(block.span(), partition_row_slice, partition_col_slice));

            ///             // Update offset for next partition
            ///             current_partition_col += std::get<1>(partition_shape);
            ///             std::get<1>(partition_block_offset) += std::get<1>(partition_shape);


            ///         }

            ///         // Offset of the first partition in the array
            ///         Offset block_offset{first_partition_offset};

            ///         // Shape of the current row of partitions in the block
            ///         Shape block_shape{
            ///             std::get<0>(partition_shape),
            ///             static_cast<Count>(std::get<1>(partition_block_offset))};

            ///         // Write current block contents to the dataset
            ///         write_block<Element, rank>(
            ///             block,
            ///             block_dataspace,
            ///             block_hyperslab(time_step_idx, block_offset, block_shape),
            ///             array, transfer_property_list);
            ///     }

            ///     lue_hpx_assert(partition_idx == static_cast<Index>(partition_tuples.size()));
            /// }


            // Approach writing partitions individually
            // Iterate over partitions and write each partition's piece to the dataset
            using Partition = typename Partitions::value_type;
            using PartitionServer = typename Partition::Server;
            using Element = ElementT<Partition>;

            for(Partition const& partition: partitions)
            {
                lue_hpx_assert(partition.is_ready());
                auto partition_ptr{detail::ready_component_ptr(partition)};
                PartitionServer const& partition_server{*partition_ptr};
                Element const* buffer{partition_server.data().data()};

                array.write(hyperslab(partition_server, 0, time_step_idx), transfer_property_list, buffer);

                // TODO Use no-data policy
                // If no-data in the partition, write no-data to the dataset
            }
        }


        template<
            typename Policies,
            typename Partitions>
        struct WritePartitionsAction:
            hpx::actions::make_action<
                    decltype(&write_partitions<Policies, Partitions>),
                    &write_partitions<Policies, Partitions>,
                    WritePartitionsAction<Policies, Partitions>
                >::type
        {};


        template<
            typename Policies,
            typename Partitions>
        struct WritePartitionsAction2:
            hpx::actions::make_action<
                    decltype(&write_partitions2<Policies, Partitions>),
                    &write_partitions2<Policies, Partitions>,
                    WritePartitionsAction2<Policies, Partitions>
                >::type
        {};

    }  // namespace detail


    namespace policy::write_into {

        template<
            typename OutputElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<>,
            OutputElements<OutputElement>,
            InputElements<>>;

        template<
            typename OutputElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<>,
            OutputElements<OutputElement>,
            InputElements<>>;

    }  // namespace policy::write_into


    template<
        typename Policies,
        typename Element,
        Rank rank>
    [[nodiscard]] hpx::future<void> write(
        Policies const& policies,
        PartitionedArray<Element, rank> const& array,
        std::string const& array_pathname,
        data_model::ID const object_id)
    {
        using Array = PartitionedArray<Element, rank>;
        using Partition = PartitionT<Array>;

        auto partitions_by_locality{detail::partitions_by_locality(array)};
        lue_hpx_assert(partitions_by_locality.size() <= hpx::find_all_localities().size());

        // -------------------------------------------------------------------------
        // Iterate over each locality and attach a continuation to all its
        // partitions to write into.

        {
            using Action = detail::WritePartitionsAction<Policies, std::vector<Partition>>;
            Action action{};

            for(auto& [locality, partitions]: partitions_by_locality)
            {
                hpx::dataflow(
                    hpx::launch::async,
                    hpx::unwrapping(

                            [locality=locality, action, policies, array_pathname, object_id](
                                std::vector<Partition>&& partitions)
                            {
                                return action(locality, policies,
                                    std::move(partitions), array_pathname, object_id);
                            }

                        ),

                    hpx::when_all(partitions.begin(), partitions.end())).get();
            }
        }

        return hpx::make_ready_future<void>();


        // std::vector<hpx::future<void>> writes_per_locality;
        // {
        //     using Action = detail::WritePartitionsAction<Policies, std::vector<Partition>>;
        //     Action action{};

        //     writes_per_locality.reserve(std::size(partitions_by_locality));

        //     for(auto& [locality, partitions]: partitions_by_locality)
        //     {
        //         writes_per_locality.push_back(
        //             hpx::dataflow(
        //                 hpx::launch::async,
        //                 hpx::unwrapping(

        //                         [locality, action, policies, array_pathname, object_id](
        //                             std::vector<Partition> const& partitions)
        //                         {
        //                             return action(locality, policies, partitions, array_pathname, object_id);
        //                         }

        //                     ),

        //                 hpx::when_all(partitions.begin(), partitions.end())));
        //     }
        // }

        // return detail::when_all_get(std::move(writes_per_locality));
    }


    template<
        typename Element,
        Rank rank>
    [[nodiscard]] hpx::future<void> write(
        PartitionedArray<Element, rank> const& array,
        std::string const& array_pathname,
        data_model::ID const object_id)
    {
        using Policies = policy::write_into::DefaultPolicies<Element>;

        return write(Policies{}, array, array_pathname, object_id);
    }


    template<
        typename Policies,
        typename Element,
        Rank rank>
    [[nodiscard]] hpx::future<void> write(
        Policies const& policies,
        PartitionedArray<Element, rank> const& array,
        std::string const& array_pathname,
        data_model::ID const object_id,
        Index const time_step_idx)
    {
        using Array = PartitionedArray<Element, rank>;
        using Partition = PartitionT<Array>;

        auto partitions_by_locality{detail::partitions_by_locality(array)};
        lue_hpx_assert(partitions_by_locality.size() <= hpx::find_all_localities().size());

        // -------------------------------------------------------------------------
        // Iterate over each locality and attach a continuation to all its
        // partitions to write into.

        {
            using Action = detail::WritePartitionsAction2<Policies, std::vector<Partition>>;
            Action action{};

            for(auto& [locality, partitions]: partitions_by_locality)
            {
                hpx::dataflow(
                    hpx::launch::async,
                    hpx::unwrapping(

                            [locality=locality, action, policies, array_pathname, object_id, time_step_idx](
                                std::vector<Partition>&& partitions)
                            {
                                return action(locality, policies,
                                    std::move(partitions), array_pathname, object_id, time_step_idx);
                            }

                        ),

                    hpx::when_all(partitions.begin(), partitions.end())).get();
            }
        }

        return hpx::make_ready_future<void>();


        // std::vector<hpx::future<void>> writes_per_locality;
        // {
        //     using Action = detail::WritePartitionsAction2<Policies, std::vector<Partition>>;
        //     Action action{};

        //     writes_per_locality.reserve(std::size(partitions_by_locality));

        //     for(auto& [locality, partitions]: partitions_by_locality)
        //     {
        //         writes_per_locality.push_back(
        //             hpx::dataflow(
        //                 hpx::launch::async,
        //                 hpx::unwrapping(

        //                         [locality, action, policies, array_pathname, object_id, time_step_idx](
        //                             std::vector<Partition> const& partitions)
        //                         {
        //                             return action(locality, policies,
        //                                 partitions, array_pathname, object_id, time_step_idx);
        //                         }

        //                     ),

        //                 hpx::when_all(partitions.begin(), partitions.end())));
        //     }
        // }

        // return detail::when_all_get(std::move(writes_per_locality));
    }


    template<
        typename Element,
        Rank rank>
    [[nodiscard]] hpx::future<void> write(
        PartitionedArray<Element, rank> const& array,
        std::string const& array_pathname,
        data_model::ID const object_id,
        Index const time_step_idx)
    {
        using Policies = policy::write_into::DefaultPolicies<Element>;

        return write(Policies{}, array, array_pathname, object_id, time_step_idx);
    }

}  // namespace lue
