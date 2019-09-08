#pragma once
#include "lue/framework/core/component/array_partition.hpp"
#include "lue/framework/core/array_partition_data.hpp"
#include "lue/framework/core/array_partition_visitor.hpp"
#include "lue/framework/core/debug.hpp"  // describe
#include "lue/framework/core/domain_decomposition.hpp"
#include "lue/framework/core/math.hpp"


namespace lue {

/*!
    @brief      Component client class associated with a collection of
                distributed array partitions
*/
template<
    typename Element,
    std::size_t rank>
class PartitionedArray
{

private:

    using PartitionClient = ArrayPartition<Element, rank>;

    using PartitionServer = typename PartitionClient::Server;

public:

    using Partition = PartitionClient;

    using Partitions = ArrayPartitionData<Partition, rank>;

    using Iterator = typename Partitions::Iterator;

    using ConstIterator = typename Partitions::ConstIterator;

    using Shape = typename Partitions::Shape;

    using Index = typename Partitions::Index;

    using Size = typename Partitions::Size;

                   PartitionedArray    ();

    explicit       PartitionedArray    (Shape const& shape);

                   PartitionedArray    (Shape const& shape,
                                        Shape const& max_partition_shape);

                   PartitionedArray    (Shape const& shape,
                                        Partitions&& partitions);

                   ~PartitionedArray   ()=default;

    Size           nr_elements         () const;

    Shape const&   shape               () const;

    Size           nr_partitions       () const;

    Partitions&    partitions          ();

    Partitions const& partitions       () const;

    ConstIterator  begin               () const;

    Iterator       begin               ();

    ConstIterator  end                 () const;

    Iterator       end                 ();

private:

    //! Shape of the partitioned array
    Shape          _shape;

    //! Array of partitions
    Partitions     _partitions;

    void           shrink_partitions   (Shape const& begin_indices,
                                        Shape const& end_indices,
                                        Shape const& new_shape);

    void           clamp_array         (Shape const& shape_in_partitions,
                                        Shape const& max_partition_shape);

    void           create              ();

    void           create              (Shape const& max_partition_shape);

    void           create              (Shape const& shape_in_partitions,
                                        Shape const& max_partition_shape);

    void           print_partitions    () const;

    void           assert_invariants   () const;

};


namespace detail {

// Specialization for PartitionedArray::Partitions
template<
    typename E,
    std::size_t r>
class ArrayTraits<ArrayPartitionData<ArrayPartition<E, r>, r>>
{

public:

    using Element = E;

    constexpr static std::size_t rank = r;

    using Shape = typename ArrayPartitionData<ArrayPartition<E, r>, r>::Shape;

    template<
        typename E_,
        std::size_t r_>
    using Partition = ArrayPartition<E_, r_>;

    template<
        typename E_,
        std::size_t r_>
    using Partitions = ArrayPartitionData<ArrayPartition<E_, r_>, r_>;

};


// Specialization for PartitionedArray
template<
    typename E,
    std::size_t r>
class ArrayTraits<PartitionedArray<E, r>>
{

public:

    using Element = E;

    constexpr static std::size_t rank = r;

    using Shape = typename PartitionedArray<E, r>::Shape;

    template<
        typename E_,
        std::size_t r_>
    using Partition = typename PartitionedArray<E_, r_>::Partition;

    template<
        typename E_,
        std::size_t r_>
    using Partitions = typename PartitionedArray<E_, r_>::Partitions;

    template<
        typename E_,
        std::size_t r_>
    using PartitionedArray = PartitionedArray<E_, r_>;

};

}  // namespace detail


template<
    typename Element,
    std::size_t rank>
PartitionedArray<Element, rank>::PartitionedArray():

    _shape{},
    _partitions{}

{
}


/*!
    @brief      Construct an instance
    @param      shape Shape of the array
*/
template<
    typename Element,
    std::size_t rank>
PartitionedArray<Element, rank>::PartitionedArray(
    Shape const& shape):

    _shape{shape},
    _partitions{}

{
    create();

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


/*!
    @brief      Construct an instance
    @param      shape Shape of the array
    @param      partitions Collection of array partitions

    The shape of the partitions together must equal the shape passed in
*/
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


// template<
//     typename Shape>
// void print_shape(
//     Shape const& shape)
// {
//     std::cout << "(";
// 
//     for(auto e: shape) {
//         std::cout << e << ", ";
//     }
// 
//     std::cout << ")";
// }


// template<
//     typename Partition>
// void print_partition(
//     Partition const& partition)
// {
//     std::cout << "    shape: ";
//     print_shape(partition.shape().get());
//     std::cout << "\n";
// }
// 
// 
// template<
//     typename Element,
//     std::size_t rank>
// void PartitionedArray<Element, rank>::print_partitions() const
// {
//     for(std::size_t i = 0; i < _partitions.size(); ++i) {
//         std::cout << "partition " << i << ":\n";
//         print_partition(_partitions[i]);
//     }
// }


template<
    typename Partitions>
class ShrinkVisitor:
    public PartitionVisitor<Partitions>
{

private:

    using Base = PartitionVisitor<Partitions>;

public:

    ShrinkVisitor(
        Partitions& partitions,
        ShapeT<Partitions> const& new_shape):

        Base{partitions},
        _new_shape{new_shape}

    {
    }

    void operator()()
    {
        PartitionT<Partitions>& partition = this->partition();

        ShapeT<Partitions> shrinked_partition_shape{partition.shape().get()};

        auto const rank = Partitions::rank;

        for(std::size_t i = 0; i < rank; ++i) {
            shrinked_partition_shape[i] =
                std::min(shrinked_partition_shape[i], _new_shape[i]);
        }

        // TODO Blocks current thread. Maybe wait in destructor?
        partition.resize(shrinked_partition_shape).wait();
    }

private:

    //! New shape to use for partitions visited
    ShapeT<Partitions> const _new_shape;

};


template<
    typename Element,
    std::size_t rank>
void PartitionedArray<Element, rank>::shrink_partitions(
    Shape const& begin_indices,
    Shape const& end_indices,
    Shape const& new_shape)
{
    // Iterate over all partitions and resize each of them.
    // Array shape is not changed! It is assumed that the shape of
    // the partitioned array after resizing corresponds with the cached
    // array shape (_shape).
    // The new_shape passed in is taken to be the max size to use. If
    // one or more dimensions of a partition is smaller than the extents
    // in new_shape, then those dimensions of the partition are not
    // resized. Partition extents are made smaller, not larger.

    ShrinkVisitor shrink{_partitions, new_shape};

    visit_array(begin_indices, end_indices, shrink);
}


template<
    typename Element,
    std::size_t rank>
void PartitionedArray<Element, rank>::clamp_array(
    Shape const& shape_in_partitions,
    Shape const& max_partition_shape)
{
    // Shrink partitions at the border of the array. They might be too large.
    // This happens when the extents of the array are not a multiple of the
    // corresponding extents in max_partition_shape.

    // Resize partitions at the sides of the array that are too large

    // // Offset to iterate over partition along one of the dimensions
    // Index partition_idx_offset;

    // Begin and end indices of all partitions in the array
    Shape begin_indices;
    begin_indices.fill(0);
    Shape const end_indices{shape_in_partitions};

    // Iterate over all dimensions
    for(std::size_t d = 0; d < rank; ++d) {
        // Extent of current dimension in partitioned array
        auto const array_extent = _shape[d];

        // Maximum extent of current dimension in partitioned array, given
        // the number of partitions and the max extent
        auto const max_partitions_extent =
            shape_in_partitions[d] * max_partition_shape[d];

        // Excess cells along current dimension in partitioned
        // array. These cells need to be removed.
        auto const excess_cells = max_partitions_extent - array_extent;

        // We are assuming here that only the last partition needs to
        // be resized. All other partitions must be positioned within
        // the array's extent.
        // Typo? assert(excess_cells < max_partitions_extent);
        assert(excess_cells < max_partition_shape[d]);

        if(excess_cells > 0) {
            // Resize current dimension of all partitions at currently
            // considered side of array
            // These partitions form a hyperslab of the array. We can
            // select this slab using start indices and end indices along
            // all dimensions. For the current dimension we set these
            // to the same value: the index of the last partition. In
            // case of the other dimension we set the start indices
            // to zero and the end indices to the last index along each
            // of these dimensions.

            Shape begin_indices_hyperslab{begin_indices};
            Shape end_indices_hyperslab{end_indices};

            begin_indices_hyperslab[d] = shape_in_partitions[d] - 1;
            end_indices_hyperslab[d] = shape_in_partitions[d];

            Shape partition_shape{max_partition_shape};
            partition_shape[d] -= excess_cells;

            shrink_partitions(
                begin_indices_hyperslab, end_indices_hyperslab,
                partition_shape);
        }
    }
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
    /// auto const& distribution_policy = hpx::container_layout;

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
    /// auto creator =
    ///     [](
    ///         // DistributionPolicy const& distribution_policy,
    ///         auto const& distribution_policy,
    ///         std::size_t const count,
    ///         Shape const& shape)
    ///     {
    ///         return distribution_policy.template
    ///             bulk_create<PartitionServer>(count, shape);
    ///     };

    std::size_t const nr_partitions = lue::nr_elements(shape_in_partitions);

    /// // For one or more localities a list of array partition component IDs
    /// // vector<pair<id_type, vector<id_type>>
    /// hpx::future<std::vector<BulkLocalityResult>> f =
    ///     creator(distribution_policy, nr_partitions, max_partition_shape);


    /// // Next adjust the border partitions and position them

    /// // // now initialize our data structures
    /// // std::uint32_t const this_locality_nr = hpx::get_locality_id();
    /// std::vector<hpx::future<void>> ptrs;

    /// // std::size_t num_part = 0;
    /// // std::size_t allocated_size = 0;

    /// // std::size_t l = 0;

    /// // auto const shape_in_partitions =
    /// //     lue::shape_in_partitions(_shape, max_partition_shape);
    /// // std::cout << max_partition_shape << std::endl;
    /// // std::cout << shape_in_partitions << std::endl;


    // Create array containing partitions. Each of these partitions will be
    // a component client instance referring to a, possibly remote,
    // component server instance.
    _partitions = Partitions{shape_in_partitions};
    assert(_partitions.size() == nr_partitions);


    std::vector<hpx::naming::id_type> const localities =
        hpx::find_all_localities();
    std::size_t const nr_localities = localities.size();

    assert(nr_localities > 0);
    assert(nr_partitions >= nr_localities);

    auto locality_idx =
        [nr_partitions, nr_localities](
            std::size_t const p) -> std::size_t
        {
            return map_to_range(
                0lu, nr_partitions - 1, 0lu, nr_localities - 1, p);
        };

    // Create array partitions. Each of them will be located on a certain
    // locality. Which one exactly is determined by locality_idx.
    for(std::size_t partition_idx = 0; partition_idx < nr_partitions;
            ++partition_idx) {
        _partitions[partition_idx] = Partition{
            localities[locality_idx(partition_idx)], max_partition_shape};

    }

    /// typename Partitions::Index partition_idx = 0;

    /// // Iterate over all localities that got array partition components
    /// // instantiated on them
    /// for(BulkLocalityResult& r: f.get()) {
    ///     assert(partition_idx < _partitions.size());

    ///     // r: pair<id_type, vector<id_type>>

    ///     // Obtain locality number from locality id
    ///     // TODO Why not use the locality id itself?
    ///     // std::uint32_t const locality_nr =
    ///     //     hpx::naming::get_locality_id_from_id(r.first);

    ///     // Iterate over all IDs of server components started on the
    ///     // locality currently iterated over
    ///     for(hpx::id_type const& id: r.second) {

    ///         // Assign this partition to a cell in the partitions array
    ///         _partitions.data()[partition_idx] = PartitionClient{id};
    ///         //     // Partition{5};
    ///         //     // Partition{id, max_partition_shape, locality_nr};

    ///         // if(locality_nr == this_locality_nr) {
    ///         //     ptrs.push_back(
    ///         //         hpx::get_ptr<PartitionServer>(id).then(
    ///         //             [this, partition_idx](auto&& future)
    ///         //             {
    ///         //                 _partitions.data()[partition_idx].set_local_data(
    ///         //                     future.get());
    ///         //             }));
    ///         // }

    ///         ++partition_idx;
    ///     }
    /// }

    /// HPX_ASSERT(partition_idx == nr_partitions);

    clamp_array(shape_in_partitions, max_partition_shape);






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



    /// // TODO Move up?
    /// hpx::wait_all(ptrs);


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
    Size const nr_localities = hpx::get_num_localities().get();
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
    typename Partitions>
class ValidateVisitor:
    public ConstPartitionVisitor<Partitions>
{

private:

    using Base = ConstPartitionVisitor<Partitions>;

public:

    ValidateVisitor(
        Partitions const& partitions,
        ShapeT<Partitions> const& shape):

        Base{partitions},
        _shape{shape}

    {
    }

    void operator()()
    {
        // Given a cell, verify it is located within the array's shape
        // PartitionT<Partitions> const& partition = this->partition();

        // TODO Add check

        // auto const& definition = partition.definition();

        // LUE_ASSERT(
        //     definition.is_within(_shape),
        //     "Partition at index {} is not positioned within "
        //     "the array's shape {}",
        //     _linear_idx, _shape);

    }

private:

    //! Shape of the array we are validating
    ShapeT<Partitions> const _shape;

};


template<
    typename Element,
    std::size_t rank>
void PartitionedArray<Element, rank>::assert_invariants() const
{
#ifndef NDEBUG

    // Visit all partitions and see whether they fit exactly within
    // the shape of the array

    Shape begin_indices;
    begin_indices.fill(0);
    Shape const end_indices{_partitions.shape()};

    ValidateVisitor validate{_partitions, _shape};

    visit_array(begin_indices, end_indices, validate);

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
typename PartitionedArray<Element, rank>::Size
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
typename PartitionedArray<Element, rank>::Size
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
ShapeT<PartitionedArray<Element, rank>> shape(
    PartitionedArray<Element, rank> const& array)
{
    return array.shape();
}


template<
    typename Element,
    std::size_t rank>
ShapeT<PartitionedArray<Element, rank>> shape_in_partitions(
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


template<
    typename Element,
    std::size_t rank>
typename PartitionedArray<Element, rank>::Size nr_elements(
    PartitionedArray<Element, rank> const& array)
{
    return array.nr_elements();
}


template<
    typename Element,
    std::size_t rank>
std::string describe(
    Shape<Element, rank> const& shape)
{
    // FIXME Requires fmt >= 6
    // return fmt::format("({})", shape);

    static_assert(rank > 0);

    auto it = shape.begin();
    std::string result = fmt::format("({}", *it);

    for(++it; it < shape.end(); ++it) {
        result += fmt::format(", {}", *it);
    }

    result += ")";

    return result;
}


template<
    typename Element,
    std::size_t rank>
std::string describe(
    PartitionedArray<Element, rank> const& array)
{
    return fmt::format(
        "- PartitionedArray:\n"
        "    - shape              : {} ({} elements)\n"
        "    - partitions         : {} ({} partitions)\n"
        ,
        describe(array.shape()),
        array.nr_elements(),
        describe(array.partitions().shape()),
        array.partitions().size()
    );
}

}  // namespace lue
