#pragma once
#include "lue/framework/core/component/array_partition.hpp"
// #include "lue/framework/core/component/server/partitioned_array_metadata.hpp"
#include "lue/framework/core/array_partition_data.hpp"
#include "lue/framework/core/array_partition_definition.hpp"
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
    typename Value,
    typename Data>
class PartitionedArray // :
    // public hpx::components::client_base<
    //     PartitionedArray<Value, Data>,
    //     server::PartitionedArray<Value, Data>>

        // hpx::components::server::distributed_metadata_base<
        //     server::PartitionedArrayMetadata<
        //         typename Data::Definition::Index, Data::rank>>>

{

public:

    static_assert(std::is_same_v<Value, typename Data::ValueType>);
    using ValueType = typename Data::ValueType;

    using ElementType = ValueType;

    static constexpr std::size_t rank = Data::rank;

    using Shape = typename Data::Shape;

    using ShapeType = Shape;

    // using Definition = lue::ArrayPartitionDefinition<typename Data::Index, Data::rank>;

    using Index = typename Data::Index;


    using PartitionClient = client::ArrayPartition<ValueType, Data>;

    using PartitionServer = typename PartitionClient::Server;

    using Partition = PartitionClient;

    using Partitions = ArrayPartitionData<Partition, Data::rank>;
    // using Partitions = ArrayPartitionData<int, Data::rank>;

private:

    // using Base = hpx::components::client_base<
    //     PartitionedArray<Value, Data>,
    //     server::PartitionedArray<Value, Data>>;

    // using Base = hpx::components::client_base<
    //     PartitionedArray<Value, Data>,
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

    // using Value = std::tuple<
    //     Envelope<double, 2>,
    //     hpx::id_type,
    //     client::ArrayPartition<T, 2>>;

    // using Index = SpatialIndex<Value>;

    // using const_iterator = typename Index::const_iterator;

    // static auto const& envelope(Value const& v) { return std::get<0>(v); }

    // static auto locality(Value const& v) { return std::get<1>(v); }

    // static auto const& partition(Value const& v) { return std::get<2>(v); }

                   PartitionedArray    ();

    explicit       PartitionedArray    (Shape const& shape);

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

    // void        insert              (Value const& value);

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

    //! For a locality ID a list of array partition component IDs
    using BulkLocalityResult =
        std::pair<hpx::id_type, std::vector<hpx::id_type>>;

    template<
        typename DistributionPolicy,
        typename Creator>
    void           create              (DistributionPolicy const&
                                            distribution_policy,
                                        Creator&& creator);

    template<
        typename DistributionPolicy>
    void           create              (DistributionPolicy const&
                                            distribution_policy);

};


template<
    typename Value,
    typename Data>
PartitionedArray<Value, Data>::PartitionedArray():

    _shape{},
    _partitions{}

{
}


template<
    typename Value,
    typename Data>
PartitionedArray<Value, Data>::PartitionedArray(
    Shape const& shape):

    _shape{shape},
    _partitions{}

{
    create(hpx::container_layout);

    // TODO Assert shape and partitions are in sync
}


template<
    typename Value,
    typename Data>
PartitionedArray<Value, Data>::PartitionedArray(
    Shape const& shape,
    Partitions&& partitions):

    _shape{shape},
    _partitions{std::move(partitions)}

{
    // TODO Assert shape and partitions are in sync
}


template<
    typename Value,
    typename Data>
template<
    typename DistributionPolicy,
    typename Creator>
void PartitionedArray<Value, Data>::create(
    DistributionPolicy const& distribution_policy,
    Creator&& creator)
{
    // The distribution_policy contains a collection of localities to place
    // partitions on and a strategy for distributing these partitions
    // over them. It does not know anything about array partitions and
    // how to create them. That is what the creator is for.

    // Given the distribution policy, determine the number of partitions
    // to create
    Index const nr_partitions =
        hpx::traits::num_container_partitions<DistributionPolicy>::call(
           distribution_policy);

    // Given the number of partitions to create and the shape of the
    // array, determine the maximum shape of the individual partitions
    auto const max_partition_shape =
        lue::max_partition_shape(_shape, nr_partitions);

    // Create the array partitions that, together make up the partitioned
    // array. Note that the extent of this array might be too large,
    // given that we use max_partition_shape.

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

    auto const shape_in_partitions =
        lue::shape_in_partitions(_shape, max_partition_shape);
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
        }

        ++partition_idx;
    }

    HPX_ASSERT(partition_idx == nr_partitions);

    // // TODO
    // // Shrink partitions at the border of the array. They might be too large.
    // // This happens when the extents of the array are not a multiple of the
    // // corresponding extents in max_partition_shape.
    // {
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





    // TODO
    // Verify that the definitions of all partitions correspond with
    // the definition of the partitioned array. The partitions must fit
    // like a puzzle.




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
}


template<
    typename Value,
    typename Data>
template<
    typename DistributionPolicy>
void PartitionedArray<Value, Data>::create(
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
    typename Value,
    typename Data>
typename PartitionedArray<Value, Data>::Index
    PartitionedArray<Value, Data>::nr_elements() const
{
    return lue::nr_elements(_shape);
}


// template<
//     typename Value,
//     typename Data>
// typename PartitionedArray<Value, Data>::Definition const&
//     PartitionedArray<Value, Data>::definition() const
// {
//     return _definition;
// }


template<
    typename Value,
    typename Data>
typename PartitionedArray<Value, Data>::Shape const&
    PartitionedArray<Value, Data>::shape() const
{
    return _shape;
}


template<
    typename Value,
    typename Data>
typename PartitionedArray<Value, Data>::Index
    PartitionedArray<Value, Data>::nr_partitions() const
{
    return _partitions.size();
}


template<
    typename Value,
    typename Data>
typename PartitionedArray<Value, Data>::Partitions&
    PartitionedArray<Value, Data>::partitions()
{
    return _partitions;
}


template<
    typename Value,
    typename Data>
typename PartitionedArray<Value, Data>::Partitions const&
    PartitionedArray<Value, Data>::partitions() const
{
    return _partitions;
}


template<
    typename Value,
    typename Data>
typename PartitionedArray<Value, Data>::ConstIterator
    PartitionedArray<Value, Data>::begin() const
{
    return _partitions.begin();
}


template<
    typename Value,
    typename Data>
typename PartitionedArray<Value, Data>::Iterator
    PartitionedArray<Value, Data>::begin()
{
    return _partitions.begin();
}


template<
    typename Value,
    typename Data>
typename PartitionedArray<Value, Data>::ConstIterator
    PartitionedArray<Value, Data>::end() const
{
    return _partitions.end();
}


template<
    typename Value,
    typename Data>
typename PartitionedArray<Value, Data>::Iterator
    PartitionedArray<Value, Data>::end()
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

}  // namespace lue
