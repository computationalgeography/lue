#pragma once
#include "lue/framework/core/component/array_partition.hpp"
// #include "lue/framework/core/component/server/partitioned_array_metadata.hpp"
#include "lue/framework/core/array_partition_data.hpp"
#include "lue/framework/core/array_partition_definition.hpp"
#include "lue/framework/core/debug.hpp"
#include "lue/framework/core/domain_decomposition.hpp"
#include "lue/framework/core/spatial_index.hpp"
#include <hpx/components/containers/container_distribution_policy.hpp>

// #include "lue/framework/test/stream.hpp"


namespace lue {

/*!
    @brief      Component client class associated with a collection of
                distributed array partitions
*/
template<
    typename Element,
    std::size_t rank>
class PartitionedArray // :
    // public hpx::components::client_base<
    //     PartitionedArray<Element, Data>,
    //     server::PartitionedArray<Element, Data>>

        // hpx::components::server::distributed_metadata_base<
        //     server::PartitionedArrayMetadata<
        //         typename Data::Definition::Index, Data::rank>>>

{

public:

    using PartitionClient = ArrayPartition<Element, rank>;

    using PartitionServer = typename PartitionClient::Server;

    using Partition = PartitionClient;

    using Partitions = ArrayPartitionData<Partition, rank>;

    using Shape = typename Partitions::Shape;

    // using Definition = lue::ArrayPartitionDefinition<typename Data::Index, Data::rank>;

    using Index = typename Partitions::Index;

private:

    // using Base = hpx::components::client_base<
    //     PartitionedArray<Element, Data>,
    //     server::PartitionedArray<Element, Data>>;

    // using Base = hpx::components::client_base<
    //     PartitionedArray<Element, Data>,
    //     hpx::components::server::distributed_metadata_base<
    //         server::PartitionedArrayMetadata<
    //             typename Data::Definition::Index, Data::rank>>>;


    // class Partition:
    //     public server::PartitionedArrayMetadata<Index, Data::rank>::Partition
    // {

    // private:

    //     using Base =
    //         typename server::PartitionedArrayMetadata<Index, Data::rank>::
    //             Partition;

    // public:

    //     Partition():

    //         Base{},
    //         _local_data{}

    //     {
    //     }

    //     /*!
    //         @brief      .
    //         @param      partition_id Global id of the partition server
    //                     object
    //         @param      definition Array definition of the partition
    //         @param      locality_nr Number of locality the server is
    //                     located on
    //         @return     .
    //         @exception  .
    //     */
    //     Partition(
    //         hpx::id_type const& id,
    //         Definition const& definition,
    //         std::uint32_t const locality):

    //         Base{id, definition, locality},
    //         _local_data{}

    //     {
    //     }

    //     Partition(
    //         Base&& other):

    //         Base{std::move(other)},
    //         _local_data{}

    //     {
    //     }

    //     void set_local_data(
    //         std::shared_ptr<PartitionServer> const& local_data)
    //     {
    //         _local_data = local_data;
    //     }

    // private:

    //     std::shared_ptr<PartitionServer> _local_data;

    // };

    //! Shape of the partitioned array
    Shape          _shape;

    // //! Shape of most partitions (possibly except for the ones at the border)
    // Shape          _partition_shape;

    //! Array of array partitions
    Partitions     _partitions;

    // Envelope<double, 2> _envelope;

    // Index          _index;

public:

    using Iterator = typename Partitions::Iterator;

    using ConstIterator = typename Partitions::ConstIterator;

    // using Element = std::tuple<
    //     Envelope<double, 2>,
    //     hpx::id_type,
    //     ArrayPartition<T, 2>>;

    // using Index = SpatialIndex<Element>;

    // using const_iterator = typename Index::const_iterator;

    // static auto const& envelope(Element const& v) { return std::get<0>(v); }

    // static auto locality(Element const& v) { return std::get<1>(v); }

    // static auto const& partition(Element const& v) { return std::get<2>(v); }

                   PartitionedArray    ();

    explicit       PartitionedArray    (Shape const& shape);

                   PartitionedArray    (Shape const& shape,
                                        Shape const& max_partition_shape);

                   PartitionedArray    (Shape const& shape,
                                        Partitions&& partitions);

    // template<
    //     typename DistributionPolicy>
    //                PartitionedArray    (Definition const& definition,
    //                                     DistributionPolicy const& distribution_policy);

    //                PartitionedArray    (Definition const& definition,
    //                                     Envelope<double, 2> const& envelope);

    //                PartitionedArray    (PartitionedArray const& other)=default;

    //                PartitionedArray    (PartitionedArray&& other)=default;

    //                ~PartitionedArray   ()=default;

    // PartitionedArray& operator=        (PartitionedArray const& other)=default;

    // PartitionedArray& operator=        (PartitionedArray&& other)=default;

    // void        insert              (Element const& value);

    Index          nr_elements         () const;

    // Definition const& definition       () const;

    Shape const&   shape               () const;

    Index          nr_partitions       () const;

    Partitions&    partitions          ();

    Partitions const& partitions       () const;

    // Envelope<double, 2> const&
    //                envelope            () const;

    // // void           wait                ();

    // bool           valid               () const;

    ConstIterator  begin               () const;

    Iterator       begin               ();

    ConstIterator  end                 () const;

    Iterator       end                 ();

private:

    void           create              ();

    void           create              (Shape const& max_partition_shape);

    void           create              (Shape const& shape_in_partitions,
                                        Shape const& max_partition_shape);

    void           assert_invariants   () const;

    //! For a locality ID a list of array partition component IDs
    using BulkLocalityResult =
        std::pair<hpx::id_type, std::vector<hpx::id_type>>;


    // template<
    //     typename DistributionPolicy,
    //     typename Creator>
    // void           create              (DistributionPolicy const&
    //                                         distribution_policy,
    //                                     Creator&& creator,
    //                                     Index const nr_partitions,
    //                                     Shape const& max_partition_shape);

    // template<
    //     typename DistributionPolicy,
    //     typename Creator>
    // void           create              (DistributionPolicy const&
    //                                         distribution_policy,
    //                                     Creator&& creator);

    // template<
    //     typename DistributionPolicy>
    // void           create              (DistributionPolicy const&
    //                                         distribution_policy);

};


template<
    typename Element,
    std::size_t rank>
PartitionedArray<Element, rank>::PartitionedArray():

    _shape{},
    _partitions{}

{
}


template<
    typename Element,
    std::size_t rank>
PartitionedArray<Element, rank>::PartitionedArray(
    Shape const& shape):

    _shape{shape},
    _partitions{}

{
    create();

    // create(hpx::container_layout);

    assert_invariants();
}


template<
    typename Element,
    std::size_t rank>
PartitionedArray<Element, rank>::PartitionedArray(
    Shape const& shape,
    Shape const& max_partition_shape):

    _shape{shape},
    _partitions{}

{
    create(max_partition_shape);

    assert_invariants();
}


template<
    typename Element,
    std::size_t rank>
PartitionedArray<Element, rank>::PartitionedArray(
    Shape const& shape,
    Partitions&& partitions):

    _shape{shape},
    _partitions{std::move(partitions)}

{
    assert_invariants();
}


template<
    typename Element,
    std::size_t rank>
void PartitionedArray<Element, rank>::create(
    Shape const& shape_in_partitions,
    Shape const& max_partition_shape)
{
    // The distribution_policy contains a collection of localities to place
    // partitions on and a strategy for distributing these partitions
    // over them. It does not know anything about array partitions and
    // how to create them. That is what the creator is for.
    auto const& distribution_policy = hpx::container_layout;

    // Create the array partitions that, together make up the partitioned
    // array. Note that the extent of this array might be too large,
    // given that we use max_partition_shape.

    // Use distribution_policy to instantiate count array partitions of
    // shape shape. Return a future to a collection containing, for each
    // locality used, a collection of GIDs of the array partitions
    // instantiated

    // The elements in the array partitions will not be initialized. Use
    // this creator for partitions that will be assigned to later.
    // template<
    //     typename DistributionPolicy>
    auto creator =
        [](
            // DistributionPolicy const& distribution_policy,
            auto const& distribution_policy,
            std::size_t const count,
            Shape const& shape)
        {
            return distribution_policy.template
                bulk_create<PartitionServer>(count, shape);
        };

    auto const nr_partitions = lue::nr_elements(shape_in_partitions);

    // For one or more localities a list of array partition component IDs
    // vector<pair<id_type, vector<id_type>>
    hpx::future<std::vector<BulkLocalityResult>> f =
        creator(distribution_policy, nr_partitions, max_partition_shape);

    // Next adjust the border partitions and position them

    // // now initialize our data structures
    // std::uint32_t const this_locality_nr = hpx::get_locality_id();
    std::vector<hpx::future<void>> ptrs;

    // std::size_t num_part = 0;
    // std::size_t allocated_size = 0;

    // std::size_t l = 0;

    // auto const shape_in_partitions =
    //     lue::shape_in_partitions(_shape, max_partition_shape);
    // std::cout << max_partition_shape << std::endl;
    // std::cout << shape_in_partitions << std::endl;


    _partitions = Partitions{shape_in_partitions};
        // Definition{_definition.start(), shape_in_partitions}};
    typename Partitions::Index partition_idx = 0;

    // Iterate over all localities that got array partition components
    // instantiated on them
    for(BulkLocalityResult& r: f.get()) {
        assert(partition_idx < _partitions.size());

        // r: pair<id_type, vector<id_type>>

        // Obtain locality number from locality id
        // TODO Why not use the locality id itself?
        // std::uint32_t const locality_nr =
        //     hpx::naming::get_locality_id_from_id(r.first);

        // Iterate over all IDs of server components started on the
        // locality currently iterated over
        for(hpx::id_type const& id: r.second) {

            // Assign this partition to a cell in the partitions array
            _partitions.data()[partition_idx] = PartitionClient{id};
            //     // Partition{5};
            //     // Partition{id, max_partition_shape, locality_nr};

            // if(locality_nr == this_locality_nr) {
            //     ptrs.push_back(
            //         hpx::get_ptr<PartitionServer>(id).then(
            //             [this, partition_idx](auto&& future)
            //             {
            //                 _partitions.data()[partition_idx].set_local_data(
            //                     future.get());
            //             }));
            // }

            ++partition_idx;
        }
    }

    HPX_ASSERT(partition_idx == nr_partitions);


    // // Shrink partitions at the border of the array. They might be too large.
    // // This happens when the extents of the array are not a multiple of the
    // // corresponding extents in max_partition_shape.
    // {
    //     // Resize partitions at the sides of the array that are too large

    //     // // Offset to iterate over partition along one of the dimensions
    //     Index partition_idx_offset;

    //     // Iterate over all dimensions
    //     for(std::size_t d = 0; d < rank; ++d) {
    //         auto const array_extent = _shape[d];
    //         auto const max_partitions_extent =
    //             shape_in_partitions[d] * max_partition_shape[d];

    //         auto const excess_cells = max_partitions_extent - array_extent;

    //         if(excess_cells > 0) {
    //             // Resize current dimension of all partitions at currently
    //             // considered side of array

    //             // Iterate over all these partitions. Ask them for their
    //             // current shape, reset the current dimension's extent,
    //             // and ask them to resize according to the new shape.

    //             // Iterate over all partitions at the end of the current
    //             // dimension. This means iterating over all current d's
    //             // and maximizing the indices of the other dimensions.

    //             auto const new_extent = max_partition_shape[d] - excess_cells;
    //             auto new_shape = max_partition_shape;
    //             new_shape[d] = new_extent;

    //             Index partition_idx = 0;
    //             Index partition_idx_offset = _partitions.shape()[d];

    //             // How to iterate over the correct cells?
    //             // recursion!
    //             // [d1_min, d2_min, d2_min]
    //             // [d1_max, d2_max, d2_max]
    //             // [d1_extent, d2_extent, d3_extent]
    //             // Calculate linear indices using a recursive function
    //             // Pass in the logic that needs to be called at the
    //             // deepest level. Somehow get the result out!

    //             // Index 

    //             // nr_partitions_in_side = xxx;

    //             // for(std::size_t i = 0; i < nr_partitions_in_side; ++i) {
    //             //     partition_idx += partition_idx_offset;
    //             //     auto& partition = _partitions[partition_idx];
    //             //     // TODO blocks
    //             //     partition.reshape(new_shape).get();
    //             // }
    //         }
    //     }


    //     //     // Linear index of partition
    //     //     Index partition_idx = 0;

    //     //     // Extents of partitions untill now, in two adjacent partitions
    //     //     Index old_extent = 0;
    //     //     Index new_extent;

    //     //     // Iterate over all partitions
    //     //     for(std::size_t p = 0; p < _partitions.shape()[d]; ++p) {

    //     //         auto const partition = _partitions[partition_idx];

    //     //         // Note: blocks for shape to be retrieved
    //     //         new_extent = old_extent + partition.shape().get()[d];

    //     //         // Last element in this partition must not extent beyond
    //     //         // the array
    //     //         LUE_ASSERT(
    //     //             new_extent <= _shape[d],
    //     //             "Along dimension {}, the last element of partition {} "
    //     //             "extents beyond the array ({} > {})",
    //     //             d, p, new_extent, _shape[d]);

    //     //         old_extent = new_extent;
    //     //         partition_idx += partition_idx_offset;
    //     //     }

    //     //     // Last element of the last partition must be the last element
    //     //     // of the array
    //     //     LUE_ASSERT(
    //     //         new_extent == _shape[d],
    //     //         "exents of partitions along dimension {} "
    //     //         "do not sum up to extent of array ({} != {})",
    //     //         d, old_extent, _shape[d]);

    //     //     partition_idx_offset *= _partitions.shape()[d];


    // }


    //     // ArrayPartitionDefinition<Index, rank> partition(
    //     //     Shape<Index, rank> const& area_shape,
    //     //     Shape<Index, rank> const& partition_shape,
    //     //     Shape<Index, rank> const& shape_in_partitions,
    //     //     std::size_t const idx)

    //     // TODO Here we iterate over all partitions, calculating the
    //     //     partition shape it should have. This can be improved by
    //     //     only iterating over bordering partitions.
    //     //     Possibly integrate with logic above.

    //     for(std::size_t p = 0; p < nr_partitions; ++p) {
    //         auto const& partition{_partitions.data()[p]};
    //         auto const current_partition_definition = partition.definition();
    //         auto const required_partition_definition =
    //             lue::partition(
    //                 _definition.shape(), max_partition_shape,
    //                 _partitions.definition().shape(), p);

    //         HPX_ASSERT(
    //             current_partition_definition.start() ==
    //             required_partition_definition.start());

    //         if(current_partition_definition.shape() !=
    //                 required_partition_definition.shape()) {
    //             PartitionClient client{partition.id()};

    //             // TODO Fix on machine with multiple localities
    //             assert(false);
    //             // client.resize(required_partition_definition.shape());
    //         }

    //     }


    //     // std::vector<std::pair<std::size_t, std::size_t>> indices(Data::rank);

    //     // // Begin and end indices along each dimension of the array
    //     // for(std::size_t d = 0; d < Data::rank; ++d) {
    //     //     indices[d] = std::make_pair(0, _definition.shape()[d]);
    //     // }

    //     // // Iterate over all dimensions
    //     // for(std::size_t d = 0; d < Data::rank; ++d) {
    //     //     auto const nr_partitions = _definition.shape()[d];
    //     //     auto const required_extent = _definition.shape()[d];
    //     //     auto const current_extent = max_partition_shape[d];

    //     //     if(current_extent != required_extent) {
    //     //         auto indices2 = indices;
    //     //         indices2[d].first = indices2[d].second - 1;


    //     //     }
    //     // }
    // }



    // TODO Move up?
    hpx::wait_all(ptrs);


    // // Cache the partition shape
    // _partition_shape = max_partition_shape;







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

    assert_invariants();
}


// template<
//     typename Element,
//     std::size_t rank>
// template<
//     typename DistributionPolicy,
//     typename Creator>
// void PartitionedArray<Element, rank>::create(
//     DistributionPolicy const& distribution_policy,
//     Creator&& creator,
//     Index const nr_partitions,
//     Shape const& max_partition_shape)
// {
//     // The distribution_policy contains a collection of localities to place
//     // partitions on and a strategy for distributing these partitions
//     // over them. It does not know anything about array partitions and
//     // how to create them. That is what the creator is for.
// 
// 
// }


// template<
//     typename Element,
//     std::size_t rank>
// template<
//     typename DistributionPolicy,
//     typename Creator>
// void PartitionedArray<Element, rank>::create(
//     DistributionPolicy const& distribution_policy,
//     Creator&& creator)
// {
//     // Given the distribution policy, determine the number of partitions
//     // to create
//     Index const nr_partitions =
//         hpx::traits::num_container_partitions<DistributionPolicy>::call(
//            distribution_policy);
// 
//     // Given the number of partitions to create and the shape of the
//     // array, determine the maximum shape of the individual partitions
//     auto const max_partition_shape =
//         lue::max_partition_shape(_shape, nr_partitions);
// 
//     create(distribution_policy, creator, nr_partitions, max_partition_shape);
// }


template<
    typename Element,
    std::size_t rank>
void PartitionedArray<Element, rank>::create()
{
    // Given the shape of the array and the number of localities,
    // determine the maximum partition shape and the shape of the array
    // in partitions

    // TODO blocks
    Index const nr_localities = hpx::get_num_localities().get();
    Shape const max_partition_shape =
        lue::max_partition_shape(_shape, nr_localities);
    auto const shape_in_partitions =
        lue::shape_in_partitions(_shape, max_partition_shape);

    create(shape_in_partitions, max_partition_shape);
}


template<
    typename Element,
    std::size_t rank>
void PartitionedArray<Element, rank>::create(
    Shape const& max_partition_shape)
{
    // Given the shape of the array and the shape of the array partitions,
    // determine the shape of the array in partitions

    auto const shape_in_partitions =
        lue::shape_in_partitions(_shape, max_partition_shape);

    create(shape_in_partitions, max_partition_shape);
}


// template<
//     typename Element,
//     std::size_t rank>
// template<
//     typename DistributionPolicy>
// void PartitionedArray<Element, rank>::create(
//     DistributionPolicy const& distribution_policy)
// {
//     // Use distribution_policy to instantiate count array partitions of
//     // shape shape. Return a future to a collection containing, for each
//     // locality used, a collection of GIDs of the array partitions
//     // instantiated
// 
//     // The elements in the array partitions will not be initialized. Use
//     // this creator for partitions that will be assigned to later.
//     auto creator =
//         [](
//             DistributionPolicy const& distribution_policy,
//             std::size_t const count,
//             Shape const& shape)
//         {
//             return distribution_policy.template
//                 bulk_create<PartitionServer>(count, shape);
//         };
// 
//     create(distribution_policy, creator);
// }


template<
    typename Element,
    std::size_t rank>
void PartitionedArray<Element, rank>::assert_invariants() const
{
#ifndef NDEBUG

    // TODO
    return;

    // Offset to iterate over partition along one of the dimensions
    Index partition_idx_offset = 1;

    // Iterate over all dimensions
    for(std::size_t d = 0; d < rank; ++d) {

        // Linear index of partition
        Index partition_idx = 0;

        // Extents of partitions untill now, in two adjacent partitions
        Index old_extent = 0;
        Index new_extent;

        // Iterate over all partitions
        for(std::size_t p = 0; p < _partitions.shape()[d]; ++p) {

            auto const& partition = _partitions[partition_idx];

            // Note: blocks for shape to be retrieved
            new_extent = old_extent + partition.shape().get()[d];

            // Last element in this partition must not extent beyond
            // the array
            LUE_ASSERT(
                new_extent <= _shape[d],
                "Along dimension {}, the last element of partition {} "
                "extents beyond the array ({} > {})",
                d, p, new_extent, _shape[d]);

            old_extent = new_extent;
            partition_idx += partition_idx_offset;
        }

        // Last element of the last partition must be the last element
        // of the array
        LUE_ASSERT(
            new_extent == _shape[d],
            "exents of partitions along dimension {} "
            "do not sum up to extent of array ({} != {})",
            d, old_extent, _shape[d]);

        partition_idx_offset *= _partitions.shape()[d];
    }

#endif
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
//     Element const& value)
// {
//     _index.insert(value);
// }


template<
    typename Element,
    std::size_t rank>
typename PartitionedArray<Element, rank>::Index
    PartitionedArray<Element, rank>::nr_elements() const
{
    return lue::nr_elements(_shape);
}


// template<
//     typename Element,
//     typename Data>
// typename PartitionedArray<Element, Data>::Definition const&
//     PartitionedArray<Element, Data>::definition() const
// {
//     return _definition;
// }


template<
    typename Element,
    std::size_t rank>
typename PartitionedArray<Element, rank>::Shape const&
    PartitionedArray<Element, rank>::shape() const
{
    return _shape;
}


template<
    typename Element,
    std::size_t rank>
typename PartitionedArray<Element, rank>::Index
    PartitionedArray<Element, rank>::nr_partitions() const
{
    return _partitions.size();
}


template<
    typename Element,
    std::size_t rank>
typename PartitionedArray<Element, rank>::Partitions&
    PartitionedArray<Element, rank>::partitions()
{
    return _partitions;
}


template<
    typename Element,
    std::size_t rank>
typename PartitionedArray<Element, rank>::Partitions const&
    PartitionedArray<Element, rank>::partitions() const
{
    return _partitions;
}


template<
    typename Element,
    std::size_t rank>
typename PartitionedArray<Element, rank>::ConstIterator
    PartitionedArray<Element, rank>::begin() const
{
    return _partitions.begin();
}


template<
    typename Element,
    std::size_t rank>
typename PartitionedArray<Element, rank>::Iterator
    PartitionedArray<Element, rank>::begin()
{
    return _partitions.begin();
}


template<
    typename Element,
    std::size_t rank>
typename PartitionedArray<Element, rank>::ConstIterator
    PartitionedArray<Element, rank>::end() const
{
    return _partitions.end();
}


template<
    typename Element,
    std::size_t rank>
typename PartitionedArray<Element, rank>::Iterator
    PartitionedArray<Element, rank>::end()
{
    return _partitions.end();
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


template<
    typename Element,
    std::size_t rank>
class ArrayPartitionsTypeTraits<
    ArrayPartitionData<ArrayPartition<Element, rank>, rank>>
{

private:

    // Use template parameters to create Partitions type

    using Partitions =
        ArrayPartitionData<ArrayPartition<Element, rank>, rank>;

public:

    // Only use Partitions, not the template parameters

    using PartitionType = typename Partitions::Element;

    template<
        typename ElementType>
    using PartitionsTemplate =
        ArrayPartitionData<
            typename ArrayPartitionTypeTraits<PartitionType>::
                template PartitionTemplate<ElementType>,
            rank>;

};


template<
    typename Element,
    std::size_t rank>
class PartitionedArrayTypeTraits<PartitionedArray<Element, rank>>
{

private:

    // Use template parameters to create Array type

    using Array = PartitionedArray<Element, rank>;

public:

    using ElementType = Element;

    // Only use Partition, not the template parameters
    using ShapeType = typename Array::Shape;
    using PartitionsType = typename Array::Partitions;
    using PartitionType = typename Array::Partition;

    template<
        typename Element_>
    using PartitionsTemplate =
        typename ArrayPartitionsTypeTraits<PartitionsType>::
            template PartitionsTemplate<Element_>;

};


template<
    typename Element,
    std::size_t rank_>
constexpr std::size_t rank(
    PartitionedArray<Element, rank_> const& /* array */) noexcept
{
    // For this array type, rank is a compile time constant, so the
    // instance is not used
    return rank_;
}


template<
    typename Element,
    std::size_t rank>
typename PartitionedArrayTypeTraits<PartitionedArray<Element, rank>>::ShapeType
        shape(
    PartitionedArray<Element, rank> const& array)
{
    return array.shape();
}


template<
    typename Element,
    std::size_t rank>
typename PartitionedArrayTypeTraits<PartitionedArray<Element, rank>>::ShapeType
        shape_in_partitions(
    PartitionedArray<Element, rank> const& array)
{
    return array.partitions().shape();
}


template<
    typename Element,
    std::size_t rank>
std::size_t nr_partitions(
    PartitionedArray<Element, rank> const& array)
{
    return array.nr_partitions();
}

}  // namespace lue
