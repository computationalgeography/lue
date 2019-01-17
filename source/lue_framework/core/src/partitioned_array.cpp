#include "lue/framework/core/partitioned_array.hpp"
#include "lue/framework/core/domain_decomposition.hpp"
// #include "raster_partition.hpp"
#include <hpx/components/containers/container_distribution_policy.hpp>
// #include <hpx/traits/is_distribution_policy.hpp>


namespace lue {

template<
    typename Index,
    typename Value,
    std::size_t rank>
PartitionedArray<Index, Value, rank>::PartitionedArray(
    Definition const& definition):

    _definition{definition},
    _partitions{}

{
    create(hpx::container_layout);
}


template<
    typename Index,
    typename Value,
    std::size_t rank>
template<
    typename DistributionPolicy,
    typename Creator>
void PartitionedArray<Index, Value, rank>::create(
    DistributionPolicy const& distribution_policy,
    Creator&& creator)
{
    // The distribution_policy contains a collection of localities to place
    // partitions on and a strategy for distributing these partitions
    // over them. It does not know anything about array partitions and
    // how to create them. That is what the creator is for.

    std::size_t const nr_partitions =
        hpx::traits::num_container_partitions<DistributionPolicy>::call(
           distribution_policy);

    // TODO Determine the Shape of the majority of the partitions and
    //      pass this info to the creator
    typename Definition::Shape partition_shape{{40, 50}};

    auto shape_in_partitions =
        lue::shape_in_partitions(_definition.shape(), partition_shape);

    // std::size_t partition_size = (size_ + nr_partitions - 1) / nr_partitions;
    // std::cout << "partition_size: " << partition_size << std::endl;

    // For all localities a collection of partitions
    hpx::future<std::vector<BulkLocalityResult>> f =
        creator(distribution_policy, nr_partitions, partition_shape);

    // Next adjust the border partitions and position them

    // // now initialize our data structures
    // std::uint32_t this_locality = get_locality_id();
    // std::vector<future<void>> ptrs;

    // std::size_t num_part = 0;
    // std::size_t allocated_size = 0;

    // std::size_t l = 0;

    _partitions = Partitions{
        Definition{_definition.start(), shape_in_partitions}};

    // // Fixing the size of partitions to avoid race conditions between
    // // possible reallocations during push back and the continuation
    // // to set the local partition data
    // partitions_.resize(nr_partitions);
    // for (bulk_locality_result const& r : f.get())
    // {
    //     using naming::get_locality_id_from_id;
    //     std::uint32_t locality = get_locality_id_from_id(r.first);
    //     for (hpx::id_type const& id : r.second)
    //     {
    //         std::size_t size = (std::min)(partition_size, size_ - allocated_size);
    //         partitions_[l] = partition_data(id, size, locality);

    //         if (locality == this_locality)
    //         {
    //             ptrs.push_back(
    //                 get_ptr<partitioned_vector_partition_server>(id).then(
    //                     util::bind_front(&partitioned_vector::get_ptr_helper,
    //                         l, std::ref(partitions_))));
    //         }
    //         ++l;

    //         allocated_size += size;
    //         if (++num_part == nr_partitions)
    //         {
    //             HPX_ASSERT(allocated_size == size_);

    //             // shrink last partition, if appropriate
    //             if (size != partition_size)
    //             {
    //                 partitioned_vector_partition_client(
    //                     partitions_[l - 1].partition_)
    //                     .resize(size);
    //             }
    //             break;
    //         }
    //         else
    //         {
    //             HPX_ASSERT(size == partition_size);
    //         }
    //         HPX_ASSERT(l < nr_partitions);
    //     }
    // }
    // HPX_ASSERT(l == nr_partitions);

    // when_all(ptrs).get();

    // // cache our partition size
    // partition_size_ = get_partition_size();
}


template<
    typename Index,
    typename Value,
    std::size_t rank>
template<
    typename DistributionPolicy>
void PartitionedArray<Index, Value, rank>::create(
    DistributionPolicy const& distribution_policy)
{
    // Use distribution_policy to instantiate count array partitions of
    // shape shape. Return a future to a collection containing, for each
    // locality used, a collection of GIDs of the array partitions
    // instantiated

    // The elements in the array partitions will not be initialized. Use
    // this creator for partitions that will be assigned to later.
    auto creator =
        [](
            DistributionPolicy const& distribution_policy,
            std::size_t const count,
            Shape const& shape)
        {
            return distribution_policy.template
                bulk_create<PartitionServer>(count, shape);
        };

    create(distribution_policy, creator);
}



// template<
//     typename T>
// typename PartitionedArray<T>::const_iterator PartitionedArray<T>::begin() const
// {
//     return _index.begin();
// }
// 
// 
// template<
//     typename T>
// typename PartitionedArray<T>::const_iterator PartitionedArray<T>::end() const
// {
//     return _index.end();
// }
// 
// 
// template<
//     typename T>
// void PartitionedArray<T>::insert(
//     Value const& value)
// {
//     _index.insert(value);
// }


template<
    typename Index,
    typename Value,
    std::size_t rank>
Index PartitionedArray<Index, Value, rank>::nr_elements() const
{
    return _definition.nr_elements();
}


template<
    typename Index,
    typename Value,
    std::size_t rank>
typename PartitionedArray<Index, Value, rank>::Definition const&
    PartitionedArray<Index, Value, rank>::definition() const
{
    return _definition;
}


template<
    typename Index,
    typename Value,
    std::size_t rank>
Index PartitionedArray<Index, Value, rank>::nr_partitions() const
{
    return _partitions.definition().nr_elements();
}


template<
    typename Index,
    typename Value,
    std::size_t rank>
typename PartitionedArray<Index, Value, rank>::Partitions const&
    PartitionedArray<Index, Value, rank>::partitions() const
{
    return _partitions;
}


// template<
//     typename T>
// Envelope<double, 2> const& PartitionedArray<T>::envelope() const
// {
//     return _envelope;
// }
// 
// 
// // template<
// //     typename T>
// // void PartitionedArray<T>::wait()
// // {
// //     wait_all(_index);
// // }
// 
// 
// template<
//     typename T>
// bool PartitionedArray<T>::valid() const
// {
//     return all_valid(_index);
// }


#define INSTANTIATE(                                           \
        Index,                                                 \
        Value,                                                 \
        rank)                                                  \
template class PartitionedArray<Index, Value, rank>;

INSTANTIATE(std::uint64_t, bool, 2)
INSTANTIATE(std::uint64_t, std::int32_t, 2)
INSTANTIATE(std::uint64_t, double, 2)

#undef INSTANTIATE

}  // namespace lue
