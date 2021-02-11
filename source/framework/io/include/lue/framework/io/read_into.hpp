#pragma once
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/io/util.hpp"
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"
#include "lue/framework/algorithm/policy/default_value_policies.hpp"
#include "lue/data_model/hl/raster_view.hpp"
#include "lue/data_model.hpp"
// #ifdef HDF5_IS_PARALLEL
// Only available in case MPI is used in HPX
// #include <hpx/mpi_base/mpi_environment.hpp>
// #endif
#include <map>


namespace lue {
namespace detail {

template<
    typename Policies,
    typename Partitions>
void read_into_partitions(
    Policies const& /* policies */,
    std::string const& array_pathname,
    data_model::ID const object_id,
    Partitions const& partitions)
{
    auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
        parse_array_pathname(array_pathname);

    // Open dataset. Configure for use of parallel I/O if necessary.
    hdf5::File::AccessPropertyList access_property_list{};

    if(hdf5::BuildOptions::hdf5_is_parallel)
    {
        // TODO
        // ::MPI_Comm communictor{hpx::util::mpi_environment::communicator()};
        // access_property_list.use_mpi_communicator(communicator, info);
    }

    auto const dataset{data_model::open_dataset(dataset_pathname, H5F_ACC_RDONLY, access_property_list)};

    // Open phenomenon
    auto const& phenomenon{dataset.phenomena()[phenomenon_name]};

    // Open property-set
    auto const& property_set{phenomenon.property_sets()[property_set_name]};

    // Open property
    lue_assert(property_set.properties().contains(property_name));
    lue_assert(property_set.properties().shape_per_object(property_name) ==
        data_model::ShapePerObject::different);
    lue_assert(property_set.properties().value_variability(property_name) ==
        data_model::ValueVariability::constant);
    using Properties = data_model::different_shape::Properties;
    auto const& property{property_set.properties().collection<Properties>()[property_name]};

    // Open value. Configure for use of parallel I/O if necessary.
    hdf5::Dataset::TransferPropertyList transfer_property_list{};

    if(hdf5::BuildOptions::hdf5_is_parallel)
    {
        // TODO
        // Use collective I/O
        // transfer_property_list.set_transfer_mode(::H5FD_mpio_xfer_t xfer_mode);
    }

    auto const& value{property.value()};
    auto const array{value[object_id]};

    // Iterate over partitions and read each partition's piece from the dataset
    using Partition = typename Partitions::value_type;
    using PartitionServer = typename Partition::Server;
    using Element = ElementT<Partition>;

    hdf5::Datatype const memory_datatype{hdf5::native_datatype<Element>()};

    for(Partition const& partition: partitions)
    {
        auto partition_ptr{detail::ready_component_ptr(partition)};
        PartitionServer& partition_server{*partition_ptr};
        Element* buffer{partition_server.data().data()};

        array.read(memory_datatype, hyperslab(partition_server), transfer_property_list, buffer);

        // TODO Use no-data policy
        // If no-data in the dataset, write no-data to the partition
    }
}


template<
    typename Policies,
    typename Partitions>
void read_into_partitions2(
    Policies const& /* policies */,
    std::string const& array_pathname,
    data_model::ID const object_id,
    Index const time_step_idx,
    Partitions const& partitions)
{
    auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
        parse_array_pathname(array_pathname);

    // Open dataset. Configure for use of parallel I/O if necessary.
    hdf5::File::AccessPropertyList access_property_list{};

    if(hdf5::BuildOptions::hdf5_is_parallel)
    {
        // TODO
        // ::MPI_Comm communictor{hpx::util::mpi_environment::communicator()};
        // access_property_list.use_mpi_communicator(communicator, info);
    }

    auto const dataset{data_model::open_dataset(dataset_pathname, H5F_ACC_RDONLY, access_property_list)};

    // Open phenomenon
    auto const& phenomenon{dataset.phenomena()[phenomenon_name]};

    // Open property-set
    auto const& property_set{phenomenon.property_sets()[property_set_name]};

    // Open property
    lue_assert(property_set.properties().contains(property_name));
    lue_assert(property_set.properties().shape_per_object(property_name) ==
        data_model::ShapePerObject::different);
    lue_assert(property_set.properties().value_variability(property_name) ==
        data_model::ValueVariability::variable);
    lue_assert(property_set.properties().shape_variability(property_name) ==
        data_model::ShapeVariability::constant);
    using Properties = data_model::different_shape::constant_shape::Properties;
    auto const& property{property_set.properties().collection<Properties>()[property_name]};

    // Open value. Configure for use of parallel I/O if necessary.
    hdf5::Dataset::TransferPropertyList transfer_property_list{};

    if(hdf5::BuildOptions::hdf5_is_parallel)
    {
        // TODO
        // Use collective I/O
        // transfer_property_list.set_transfer_mode(::H5FD_mpio_xfer_t xfer_mode);
    }

    auto const& value{property.value()};
    auto const array{value[object_id]};

    // Iterate over partitions and read each partition's piece from the dataset
    using Partition = typename Partitions::value_type;
    using PartitionServer = typename Partition::Server;
    using Element = ElementT<Partition>;

    hdf5::Datatype const memory_datatype{hdf5::native_datatype<Element>()};

    for(Partition const& partition: partitions)
    {
        auto partition_ptr{detail::ready_component_ptr(partition)};
        PartitionServer& partition_server{*partition_ptr};
        Element* buffer{partition_server.data().data()};

        array.read(
            memory_datatype, hyperslab(partition_server, 0, time_step_idx), transfer_property_list, buffer);

        // TODO Use no-data policy
        // If no-data in the dataset, write no-data to the partition
    }
}


template<
    typename Policies,
    typename Partitions>
struct ReadIntoPartitionsAction:
    hpx::actions::make_action<
            decltype(&read_into_partitions<Policies, Partitions>),
            &read_into_partitions<Policies, Partitions>,
            ReadIntoPartitionsAction<Policies, Partitions>
        >
{};


template<
    typename Policies,
    typename Partitions>
struct ReadIntoPartitionsAction2:
    hpx::actions::make_action<
            decltype(&read_into_partitions2<Policies, Partitions>),
            &read_into_partitions2<Policies, Partitions>,
            ReadIntoPartitionsAction2<Policies, Partitions>
        >
{};

}  // namespace detail


template<
    typename Policies,
    typename Element,
    Rank rank>
[[nodiscard]] hpx::future<void> read_into(
    Policies const& policies,
    std::string const& array_pathname,
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

    // -------------------------------------------------------------------------
    // Iterate over each locality and attach a continuation to all its
    // partitions to read into.
    std::vector<hpx::future<void>> reads_per_locality;
    {
        using Action = detail::ReadIntoPartitionsAction<Policies, std::vector<Partition>>;
        Action action{};

        reads_per_locality.reserve(std::size(partitions_by_locality));

        for(auto& [locality, partitions]: partitions_by_locality)
        {
            reads_per_locality.push_back(
                hpx::dataflow(
                    hpx::launch::async,
                    hpx::util::unwrapping(

                            [locality, action, policies, array_pathname, object_id](
                                std::vector<Partition> const& partitions)
                            {
                                return action(locality, policies, array_pathname, object_id, partitions);
                            }

                        ),

                    hpx::when_all(partitions.begin(), partitions.end())));
        }
    }

    return detail::when_all_get(std::move(reads_per_locality));
}


template<
    typename Policies,
    typename Element,
    Rank rank>
[[nodiscard]] hpx::future<void> read_into(
    Policies const& policies,
    std::string const& array_pathname,
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

    // -------------------------------------------------------------------------
    // Iterate over each locality and attach a continuation to all its
    // partitions to read into.
    std::vector<hpx::future<void>> reads_per_locality;
    {
        using Action = detail::ReadIntoPartitionsAction2<Policies, std::vector<Partition>>;
        Action action{};

        reads_per_locality.reserve(std::size(partitions_by_locality));

        for(auto& [locality, partitions]: partitions_by_locality)
        {
            reads_per_locality.push_back(
                hpx::dataflow(
                    hpx::launch::async,
                    hpx::util::unwrapping(

                            [locality, action, policies, array_pathname, object_id, time_step_idx](
                                std::vector<Partition> const& partitions)
                            {
                                return action(
                                    locality, policies, array_pathname, object_id, time_step_idx, partitions);
                            }

                        ),

                    hpx::when_all(partitions.begin(), partitions.end())));
        }
    }

    return detail::when_all_get(std::move(reads_per_locality));
}


namespace detail {

    template<
        typename Count,
        Rank rank>
    Shape<Count, rank> constant_array_shape(
        std::string const& array_pathname)
    {
        auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
            parse_array_pathname(array_pathname);

        namespace lh5 = lue::hdf5;
        namespace ldm = lue::data_model;

        using DatasetPtr = std::shared_ptr<ldm::Dataset>;
        using RasterView = ldm::constant::RasterView<DatasetPtr>;

        auto input_dataset_ptr = std::make_shared<ldm::Dataset>(
            ldm::open_dataset(dataset_pathname, H5F_ACC_RDONLY));

        lh5::Shape grid_shape{};

        // Open a view on the raster
        RasterView input_raster_view{input_dataset_ptr, phenomenon_name, property_set_name};

        grid_shape = lh5::Shape{input_raster_view.grid_shape()};

        using Shape = lue::Shape<Count, rank>;

        lue_assert(rank == 2);  // TODO(KDJ)

        return Shape{{
            static_cast<typename Shape::value_type>(grid_shape[0]),
            static_cast<typename Shape::value_type>(grid_shape[1])}};
    }


    template<
        typename Count,
        Rank rank>
    Shape<Count, rank> variable_array_shape(
        std::string const& array_pathname)
    {
        auto const [dataset_pathname, phenomenon_name, property_set_name, property_name] =
            parse_array_pathname(array_pathname);

        namespace lh5 = lue::hdf5;
        namespace ldm = lue::data_model;

        using DatasetPtr = std::shared_ptr<ldm::Dataset>;
        using RasterView = ldm::variable::RasterView<DatasetPtr>;

        auto input_dataset_ptr = std::make_shared<ldm::Dataset>(
            ldm::open_dataset(dataset_pathname, H5F_ACC_RDONLY));

        lh5::Shape grid_shape{};

        // Open a view on the raster
        RasterView input_raster_view{input_dataset_ptr, phenomenon_name, property_set_name};

        grid_shape = lh5::Shape{input_raster_view.grid_shape()};

        using Shape = lue::Shape<Count, rank>;

        lue_assert(rank == 2);  // TODO(KDJ)

        return Shape{{
            static_cast<typename Shape::value_type>(grid_shape[0]),
            static_cast<typename Shape::value_type>(grid_shape[1])}};
    }

}  // namespace detail


namespace policy::read_into {

    template<
        typename OutputElement>
    using DefaultPolicies = policy::DefaultPolicies<
        OutputElements<OutputElement>,
        InputElements<>>;


    template<
        typename OutputElement>
    using DefaultValuePolicies = policy::DefaultValuePolicies<
        OutputElements<OutputElement>,
        InputElements<>>;

}  // namespace policy::read_into


template<
    typename Policies,
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> read(
    Policies const& policies,
    std::string const& array_pathname,
    ShapeT<PartitionedArray<Element, rank>> const& partition_shape,
    data_model::ID const object_id)
{
    // Open dataset and figure out what the properties are of the
    // array. Return a partitioned array corresponding with these properties.

    using Array = PartitionedArray<Element, rank>;
    using Functor = InstantiateDefaultInitialized<Element, rank>;
    using Shape = ShapeT<Array>;

    Shape const array_shape{detail::constant_array_shape<Count, rank>(array_pathname)};

    // Create partitioned array
    Array array{create_partitioned_array(policies, array_shape, partition_shape, Functor{})};

    // Read elements from dataset into array
    // TODO(KDJ) This blocks until the data is read. Can we do better?
    read_into(policies, array_pathname, object_id, array).get();

    return array;
}


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> read(
    std::string const& array_pathname,
    ShapeT<PartitionedArray<Element, rank>> const& partition_shape,
    data_model::ID const object_id)
{
    using Policies = policy::read_into::DefaultPolicies<Element>;

    return read(Policies{}, array_pathname, partition_shape, object_id);
}


template<
    typename Policies,
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> read(
    Policies const& policies,
    std::string const& array_pathname,
    ShapeT<PartitionedArray<Element, rank>> const& partition_shape,
    data_model::ID const object_id,
    Index const time_step_idx)
{
    // Open dataset and figure out what the properties are of the
    // array. Return a partitioned array corresponding with these properties.

    using Array = PartitionedArray<Element, rank>;
    using Functor = InstantiateDefaultInitialized<Element, rank>;
    using Shape = ShapeT<Array>;

    Shape const array_shape{detail::variable_array_shape<Count, rank>(array_pathname)};

    // Create partitioned array
    Array array{create_partitioned_array(policies, array_shape, partition_shape, Functor{})};

    // Read elements from dataset into array
    // TODO(KDJ) This blocks until the data is read. Can we do better?
    read_into(policies, array_pathname, object_id, time_step_idx, array).get();

    return array;
}


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank> read(
    std::string const& array_pathname,
    ShapeT<PartitionedArray<Element, rank>> const& partition_shape,
    data_model::ID const object_id,
    Index const time_step_idx)
{
    using Policies = policy::read_into::DefaultPolicies<Element>;

    return read(Policies{}, array_pathname, partition_shape, object_id, time_step_idx);
}

}  // namespace lue
