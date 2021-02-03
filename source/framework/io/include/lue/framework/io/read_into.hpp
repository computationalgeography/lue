#pragma once
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/io/util.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"
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
struct ReadIntoPartitionsAction:
    hpx::actions::make_action<
            decltype(&read_into_partitions<Policies, Partitions>),
            &read_into_partitions<Policies, Partitions>,
            ReadIntoPartitionsAction<Policies, Partitions>
        >
{};

}  // namespace detail


namespace policy {
namespace read_into {

template<
    typename OutputElement>
using DefaultPolicies = policy::DefaultPolicies<
    OutputElements<OutputElement>,
    InputElements<>>;

}  // namespace read_into
}  // namespace policy


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
    typename Element,
    Rank rank>
[[nodiscard]] hpx::future<void> read_into(
    std::string const& array_pathname,
    data_model::ID const object_id,
    PartitionedArray<Element, rank>& array)
{
    using Policies = policy::read_into::DefaultPolicies<Element>;

    return read_into(Policies{}, array_pathname, object_id, array);
}


// template<
//     typename Policies,
//     typename Partition>
// class ReadPartition
// {
//     public:
// 
//         static constexpr bool instantiate_per_locality{true};
// 
//         // ReadPartition():
// 
//     private:
// 
// };
// 
// 
// template<
//     typename Policies,
//     typename Element,
//     Rank rank>
// PartitionedArray<Element, rank> read(
//     Policies const& policies,
//     std::string const& array_pathname,
//     ShapeT<PartitionedArray<Element, rank>> const& partition_shape,
//     data_model::ID const object_id)
// {
//     // Open dataset and figure out what the properties are of the
//     // array. Return a partitioned array corresponding with these properties.
// 
//     // Array array{array_shape, partition_shape};
// 
//     // Given array_shape, partition_shape and a functor that instantiates
//     // a new partitions and fills it with values, create a new array. Very
//     // similar to the PartitionedArray constructor, except:
//     // - Value to put in the partition are created by the functor
//     //     - Uniform
//     //     - Fill
//     //     - Read from dataset
//     //     - Do nothing (uninitialized)
//     //     - ...
//     // - In the case of read, the actions that create partitions must
//     //     execute collectively per locality, not individually per partition.
// 
// 
//     using Array = PartitionedArray<Element, rank>;
//     using Partition = PartitionT<Array>;
//     using Functor = ReadPartition<Policies, Partition>;
//     using Shape = ShapeT<Array>;
// 
//     Shape const array_shape{array_shape(array_pathname)};
// 
//     return create_partitioned_array(
//         policies, array_shape, partition_shape, Functor{array_pathname, object_id});
// }

}  // namespace lue


// https://www.hdfgroup.org/2020/06/webinar-followup-parallel-i-o-with-hdf5-and-performance-tuning-techniques/
// https://www.hdfgroup.org/2020/06/webinar-followup-parallel-i-o-with-hdf5-and-performance-tuning-techniques/


// https://portal.hdfgroup.org/display/HDF5/Parallel+HDF5

// Just use parallel I/O if it is available


// Not for shared memory programming


// collective
// - all processes must participate in the right order
// - independent means not collective
// - collective is not necessarily synchronous
// - most PHDF5 APIs are collective. E.g.:
//     - file operations: Fcreate, Fopen, Fclose
//     - object creation: Dcreate, Dopen, Dclose
//     - object structure: Dextend (increase dimension sizes)
//     - Array data transfer can be collective or independent


// Each process (locality)
// - setup MPI-IO access template
//     H5Pset_fapl_mpio(...)
// - open file
//     H5Fcreate(filename, ..., access_property_list)
// - close file
//     H5Fclose(file_id)



/*
   // Per process do something like this:

   hdf5::File::AccessPropertyList access_property_list{};

   if(hdf5::BuildOptions::parallel_io)
   {
       ::MPI_Comm communicator{...};  // MPI_COMM_WORLD
       ::MPI_Info info{...};  // MPI_INFO_NULL

       access_property_list.use_mpi_communicator(communicator, info);
   }

   hdf5::File file{name, access_property_list};


   // Per process create / open datasets
   // Per process extend dataset with unlimited dimensions before writing to it

   // If all processes must do the same thing collectively, they must
   // have more global knowledge than they might have. → What knowledge?

   // Reading:
   // - All processes that have opened a dataset may do collective I/O
   // - Each process may do independent and arbitrary number of I/O access calls


Accessing a dataset:


Dataset::TransferPropertyList transfer_property_list{};

if(hdf5::BuildOptions::parallel_io)
{
    ::H5FD_mpio_xfer_t transfer_mode{...};  // H5D_MPIO_COLLECTIVE / H5D_MPIO_INDEPENDENT (default)

    transfer_property_list.set_transfer_mode();

    // Pass this property_list to dataset.read / dataset.write
}


Each process opens a file. What if multiple read operations are performed
on the same file? This calls for some I/O service per process that keeps
track of this? Once all processes have opened the file for some I/O
operation, the file doesn't need to be opened again for another I/O
operation. Alternative: aggregate I/O operations that access the same
file and be done with it.

read(filename, ...)



All functions that modify structural metadata are collective!!!



On the root locality:

template<
    typename Array>
Array& read_into(
    Array& array,
    std::string const& raster_pathname,
    typename data_model::constant::RasterView<DatasetPtr>::Layer const& layer)

Multiple of these calls may be done. How to make sure they use the same
HDF5 file thingy? Maybe, on the localities, have some static global
thing containing open files? These must be cleared once nobody accesses
them anymore though.


Collectiveness is indicated by function parameters, not by function names
(as in MPI API)

Collective I/O will combine many small independent calls into few but
bigger calls.



Three options (see what works better)
- Operate on all chunks in one collective I/O operation: "linked chunk I/O"
- Operate on each chunk collectively: "multi-chunk I/O"
- Break collective I/O and perform I/O on each chunk independently:
    also in "multi-chunk I/O" algorithm

Collective chunk mode
- One linked-chunk I/O? (optional user input)
    - yes: One collective I/O call for all chunks
    - no: Multi-chunk I/O
        Collective I/O? (optional user input)
        - yes: Collective I/O per chunk
        - no: Independent I/O


Sources for chunking:
- Partition size used by framework (memory)
- Chunks used by data model (file)
- Chunking used by parallel filesystem (file)

Perform I/O per thread or per process?
- If per thread, then each of them must perform all collective calls. This
    seem unfeasible.

Metadata operations
- Create / remove dataset, group, attribute, ...
- Extent dataset dimensions
- Modify group hierarchy
- ...

All metadata operations are collective: all processes have to call that operation.
→ When running 1000 processes and one needs to create a dataset, *all*
    processes must call H5Dcreate to create 1 dataset...!!!

Flushing the metadata cache
- Single process (process 0) write
- Distributed write

h5perf
- Provides indication of I/O speed upper limits


autotuning!


Use h5c++ -show to see command line options to use. (Or query CMake variables?)
    /usr/lib/x86_64-linux-gnu/hdf5/openmpi/libhdf5.settings

Collective I/O: attempts to combine multiple smaller independent I/O
ops into fewer larger ops

Most PHDF5 APIs are collective.
*/
