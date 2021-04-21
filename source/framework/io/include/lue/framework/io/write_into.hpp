#pragma once
#include "lue/framework/io/util.hpp"
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/core/component.hpp"
#include "lue/data_model.hpp"
#ifdef HDF5_IS_PARALLEL
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



        template<
            typename Policies,
            typename Partitions>
        void write_partitions_on_os_thread(
            Policies const& /* policies */,
            Partitions const& partitions,
            std::string const& array_pathname,
            data_model::ID const object_id)
        {
            auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
                parse_array_pathname(array_pathname);

            // Open dataset. Configure for use of parallel I/O if necessary.
            hdf5::File::AccessPropertyList access_property_list{};

#ifdef HDF5_IS_PARALLEL
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

#ifdef HDF5_IS_PARALLEL
            if(hpx::util::mpi_environment::enabled())
            {
                // // Use collective I/O
                // transfer_property_list.set_transfer_mode(::H5FD_MPIO_COLLECTIVE);
            }
#endif

            auto& value{property.value()};
            auto array{value[object_id]};

            // Iterate over partitions and write each partition's piece to the dataset
            using Partition = typename Partitions::value_type;
            using PartitionServer = typename Partition::Server;
            using Element = ElementT<Partition>;

            for(Partition const& partition: partitions)
            {
                auto partition_ptr{detail::ready_component_ptr(partition)};
                PartitionServer const& partition_server{*partition_ptr};
                Element const* buffer{partition_server.data().data()};

                array.write(hyperslab(partition_server), transfer_property_list, buffer);
            }
        }


        template<
            typename Policies,
            typename Partitions>
        void write_partitions(
            Policies const& policies,
            Partitions const& partitions,
            std::string const& array_pathname,
            data_model::ID const object_id)
        {
            // Get a reference to one of the IO specific HPX io_service objects ...
            hpx::parallel::execution::io_pool_executor executor;

            // ... and schedule the handler to run on one of its OS-threads.
            hpx::async(
                executor, &write_partitions_on_os_thread<Policies, Partitions>,
                policies, partitions, array_pathname, object_id).get();
        }




//         template<
//             typename Policies,
//             typename Partitions>
//         void write_partitions(
//             Policies const& /* policies */,
//             Partitions const& partitions,
//             std::string const& array_pathname,
//             data_model::ID const object_id)
//         {
//             auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
//                 parse_array_pathname(array_pathname);
// 
//             // Open dataset. Configure for use of parallel I/O if necessary.
//             hdf5::File::AccessPropertyList access_property_list{};
// 
// #ifdef HDF5_IS_PARALLEL
//             if(hpx::util::mpi_environment::enabled())
//             {
//                 // // ::MPI_Comm communicator{hpx::util::mpi_environment::communicator()};
//                 // ::MPI_Comm communicator{MPI_COMM_WORLD};
//                 // ::MPI_Info info{MPI_INFO_NULL};
//                 // access_property_list.use_mpi_communicator(communicator, info);
//             }
// #endif
// 
//             auto dataset{data_model::open_dataset(dataset_pathname, H5F_ACC_RDWR, access_property_list)};
// 
//             // Open phenomenon
//             auto& phenomenon{dataset.phenomena()[phenomenon_name]};
// 
//             // Open property-set
//             auto& property_set{phenomenon.property_sets()[property_set_name]};
// 
//             // Open property
//             lue_hpx_assert(property_set.properties().contains(property_name));
//             lue_hpx_assert(property_set.properties().shape_per_object(property_name) ==
//                 data_model::ShapePerObject::different);
//             lue_hpx_assert(property_set.properties().value_variability(property_name) ==
//                 data_model::ValueVariability::constant);
//             using Properties = data_model::different_shape::Properties;
//             auto& property{property_set.properties().collection<Properties>()[property_name]};
// 
//             // Open value. Configure for use of parallel I/O if necessary.
//             hdf5::Dataset::TransferPropertyList transfer_property_list{};
// 
// #ifdef HDF5_IS_PARALLEL
//             if(hpx::util::mpi_environment::enabled())
//             {
//                 // // Use collective I/O
//                 // transfer_property_list.set_transfer_mode(::H5FD_MPIO_COLLECTIVE);
//             }
// #endif
// 
//             auto& value{property.value()};
//             auto array{value[object_id]};
// 
//             // Iterate over partitions and write each partition's piece to the dataset
//             using Partition = typename Partitions::value_type;
//             using PartitionServer = typename Partition::Server;
//             using Element = ElementT<Partition>;
// 
//             for(Partition const& partition: partitions)
//             {
//                 auto partition_ptr{detail::ready_component_ptr(partition)};
//                 PartitionServer const& partition_server{*partition_ptr};
//                 Element const* buffer{partition_server.data().data()};
// 
//                 array.write(hyperslab(partition_server), transfer_property_list, buffer);
//             }
//         }


        template<
            typename Policies,
            typename Partitions>
        void write_partitions2(
            Policies const& /* policies */,
            Partitions const& partitions,
            std::string const& array_pathname,
            data_model::ID const object_id,
            Index const time_step_idx)
        {
            auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
                parse_array_pathname(array_pathname);

            // Open dataset. Configure for use of parallel I/O if necessary.
            hdf5::File::AccessPropertyList access_property_list{};

#ifdef HDF5_IS_PARALLEL
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

#ifdef HDF5_IS_PARALLEL
            if(hpx::util::mpi_environment::enabled())
            {
                // // Use collective I/O
                // transfer_property_list.set_transfer_mode(::H5FD_MPIO_COLLECTIVE);
            }
#endif

            auto& value{property.value()};
            auto array{value[object_id]};

            // Iterate over partitions and write each partition's piece to the dataset
            using Partition = typename Partitions::value_type;
            using PartitionServer = typename Partition::Server;
            using Element = ElementT<Partition>;

            for(Partition const& partition: partitions)
            {
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

        // -------------------------------------------------------------------------
        // Iterate over each locality and attach a continuation to all its
        // partitions to read into.

        {
            using Action = detail::WritePartitionsAction<Policies, std::vector<Partition>>;
            Action action{};

            for(auto& [locality, partitions]: partitions_by_locality)
            {
                hpx::dataflow(
                    hpx::launch::async,
                    hpx::util::unwrapping(

                            [locality, action, policies, array_pathname, object_id](
                                std::vector<Partition> const& partitions)
                            {
                                return action(locality, policies, partitions, array_pathname, object_id);
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
        //                 hpx::util::unwrapping(

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

        // -------------------------------------------------------------------------
        // Iterate over each locality and attach a continuation to all its
        // partitions to read into.

        {
            using Action = detail::WritePartitionsAction2<Policies, std::vector<Partition>>;
            Action action{};

            for(auto& [locality, partitions]: partitions_by_locality)
            {
                hpx::dataflow(
                    hpx::launch::async,
                    hpx::util::unwrapping(

                            [locality, action, policies, array_pathname, object_id, time_step_idx](
                                std::vector<Partition> const& partitions)
                            {
                                return action(locality, policies,
                                    partitions, array_pathname, object_id, time_step_idx);
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
        //                 hpx::util::unwrapping(

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
