#pragma once
#include "lue/framework/io/util.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"
#include "lue/data_model.hpp"
// #ifdef HDF5_IS_PARALLEL
// Only available in case MPI is used in HPX
// #include <hpx/mpi_base/mpi_environment.hpp>
// #endif


namespace lue {
namespace detail {


template<
    typename Policies,
    typename Partitions>
void write_into_partitions(
    Policies const& /* policies */,
    Partitions const& partitions,
    std::string const& array_pathname,
    data_model::ID const object_id)
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

    auto dataset{data_model::open_dataset(dataset_pathname, H5F_ACC_RDWR, access_property_list)};

    // Open phenomenon
    auto& phenomenon{dataset.phenomena()[phenomenon_name]};

    // Open property-set
    auto& property_set{phenomenon.property_sets()[property_set_name]};

    // Open property
    lue_assert(property_set.properties().contains(property_name));
    lue_assert(property_set.properties().shape_per_object(property_name) ==
        data_model::ShapePerObject::different);
    lue_assert(property_set.properties().value_variability(property_name) ==
        data_model::ValueVariability::constant);
    using Properties = data_model::different_shape::Properties;
    auto& property{property_set.properties().collection<Properties>()[property_name]};

    // Open value. Configure for use of parallel I/O if necessary.
    hdf5::Dataset::TransferPropertyList transfer_property_list{};

    if(hdf5::BuildOptions::hdf5_is_parallel)
    {
        // TODO
        // Use collective I/O
        // transfer_property_list.set_transfer_mode(::H5FD_mpio_xfer_t xfer_mode);
    }

    auto& value{property.value()};
    auto array{value[object_id]};

    // Iterate over partitions and write each partition's piece to the dataset
    using Partition = typename Partitions::value_type;
    using PartitionServer = typename Partition::Server;
    using Element = ElementT<Partition>;

    for(Partition const& partition: partitions)
    {
        auto partition_ptr{ready_component_ptr(partition)};
        PartitionServer const& partition_server{*partition_ptr};
        Element const* buffer{partition_server.data().data()};

        array.write(hyperslab(partition_server), transfer_property_list, buffer);
    }
}


template<
    typename Policies,
    typename Partitions>
struct WriteIntoPartitionsAction:
    hpx::actions::make_action<
            decltype(&write_into_partitions<Policies, Partitions>),
            &write_into_partitions<Policies, Partitions>,
            WriteIntoPartitionsAction<Policies, Partitions>
        >
{};

}  // namespace detail


namespace policy {
namespace write_into {

template<
    typename InputElement>
using DefaultPolicies = policy::DefaultPolicies<
    OutputElements<>,
    InputElements<InputElement>>;

}  // namespace write_into
}  // namespace policy


template<
    typename Policies,
    typename Element,
    Rank rank>
[[nodiscard]] hpx::future<void> write_into(
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
    std::vector<hpx::future<void>> writes_per_locality;
    {
        using Action = detail::WriteIntoPartitionsAction<Policies, std::vector<Partition>>;
        Action action{};

        writes_per_locality.reserve(std::size(partitions_by_locality));

        for(auto& [locality, partitions]: partitions_by_locality)
        {
            writes_per_locality.push_back(
                hpx::dataflow(
                    hpx::launch::async,
                    hpx::util::unwrapping(

                            [locality, action, policies, array_pathname, object_id](
                                std::vector<Partition> const& partitions)
                            {
                                return action(locality, policies, partitions, array_pathname, object_id);
                            }

                        ),

                    hpx::when_all(partitions.begin(), partitions.end())));
        }
    }

    return detail::when_all_get(std::move(writes_per_locality));
}


template<
    typename Element,
    Rank rank>
[[nodiscard]] hpx::future<void> write_into(
    PartitionedArray<Element, rank> const& array,
    std::string const& array_pathname,
    data_model::ID const object_id)
{
    using Policies = policy::write_into::DefaultPolicies<Element>;

    return write_into(Policies{}, array, array_pathname, object_id);
}

}  // namespace lue
