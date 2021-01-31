#pragma once
#include "lue/framework/core/component/array_partition.hpp"
#include "lue/framework/core/array.hpp"
#include "lue/framework/core/array_partition_data.hpp"
#include "lue/framework/core/array_partition_visitor.hpp"
#include "lue/framework/core/debug.hpp"  // describe
#include "lue/framework/core/domain_decomposition.hpp"
#include "lue/framework/core/hilbert_curve.hpp"
#include "lue/framework/core/linear_curve.hpp"
#include "lue/framework/core/math.hpp"
#include "lue/framework/core/annotate.hpp"
#include "lue/stream.hpp"
#include <fmt/ostream.h>
#include <initializer_list>


namespace lue {

template<
    Rank rank>
using Localities = Array<hpx::id_type, rank>;


template<
    typename Element,
    Rank rank>
class PartitionedArray
{

private:

    using PartitionClient = ArrayPartition<Element, rank>;

    using PartitionServer = typename PartitionClient::Server;

public:

    enum class ClampMode
    {

        //! Shrink border partitions
        shrink,

        //! Enlarge border partitions
        merge

    };


    using Partition = PartitionClient;

    using Partitions = ArrayPartitionData<Partition, rank>;

    using Count = typename Partitions::Count;

    using Offset = typename PartitionServer::Offset;

    using Shape = typename Partitions::Shape;

    using Iterator = typename Partitions::Iterator;

    using ConstIterator = typename Partitions::ConstIterator;

                   PartitionedArray    ();

                   PartitionedArray    (PartitionedArray const& other)=delete;

                   PartitionedArray    (PartitionedArray&& other)=default;

    explicit       PartitionedArray    (Shape const& shape);

                   PartitionedArray    (Shape const& shape,
                                        Shape const& max_partition_shape,
                                        ClampMode clamp_mode=ClampMode::merge);

                   PartitionedArray    (Shape const& shape,
                                        Shape const& max_partition_shape,
                                        Element const& fill_value,
                                        ClampMode clamp_mode=ClampMode::merge);

                   PartitionedArray    (Shape const& shape,
                                        Localities<rank> const& localities,
                                        Partitions&& partitions);

                   ~PartitionedArray   ()=default;

    PartitionedArray& operator=        (PartitionedArray const& other)=delete;

    PartitionedArray& operator=        (PartitionedArray&& other)=default;

    Count          nr_elements         () const;

    Shape const&   shape               () const;

    Localities<rank> const&
                   localities          () const;

    Count          nr_partitions       () const;

    Partitions&    partitions          ();

    Partitions const& partitions       () const;

    ConstIterator  begin               () const;

    Iterator       begin               ();

    ConstIterator  end                 () const;

    Iterator       end                 ();

private:

    //! Shape of the partitioned array
    Shape          _shape;

    //! Localities the partitions are located in
    Localities<rank> _localities;

    //! Array of partitions
    Partitions     _partitions;

    template<
        typename... Idxs>
    void           instantiate_partition(
                                        hpx::id_type locality_id,
                                        Shape const& partition_shape,
                                        Idxs... idxs);

    template<
        typename... Idxs>
    void           instantiate_partition(
                                        hpx::id_type locality_id,
                                        Shape const& partition_shape,
                                        Element const& fill_value,
                                        Idxs... idxs);

    void           instantiate_partitions(
                                        Shape const& shape_in_partitions,
                                        Shape const& partition_shape);

    void           instantiate_partitions(
                                        Shape const& shape_in_partitions,
                                        Shape const& partition_shape,
                                        Element const& fill_value);

    void           shrink_partitions   (Shape const& begin_indices,
                                        Shape const& end_indices,
                                        Shape const& new_shape);

    void           resize_partitions   (Shape const& begin_indices,
                                        Shape const& end_indices,
                                        Index dimension_idx,
                                        Count new_size);

    void           clamp_array_shrink  (Shape const& shape_in_partitions,
                                        Shape const& max_partition_shape);

    void           clamp_array_merge   (Shape const& shape_in_partitions,
                                        Shape const& max_partition_shape);

    void           create              ();

    void           create              (Shape const& max_partition_shape,
                                        ClampMode clamp_mode);

    void           create              (Shape const& max_partition_shape,
                                        Element const& fill_value,
                                        ClampMode clamp_mode);

    void           create              (Shape const& shape_in_partitions,
                                        Shape const& max_partition_shape,
                                        ClampMode clamp_mode);

    void           create              (Shape const& shape_in_partitions,
                                        Shape const& max_partition_shape,
                                        Element const& fill_value,
                                        ClampMode clamp_mode);

    void           print_partitions    () const;

    void           assert_invariants   () const;

};


namespace detail {

// Specialization for PartitionedArray::Partitions
template<
    typename E,
    Rank r>
class ArrayTraits<ArrayPartitionData<ArrayPartition<E, r>, r>>
{

public:

    using Element = E;

    constexpr static Rank rank = r;

    using Shape = typename ArrayPartitionData<ArrayPartition<E, r>, r>::Shape;

    template<
        typename E_,
        Rank r_>
    using Partition = ArrayPartition<E_, r_>;

    template<
        typename E_,
        Rank r_>
    using Partitions = ArrayPartitionData<ArrayPartition<E_, r_>, r_>;

};


// Specialization for PartitionedArray
template<
    typename E,
    Rank r>
class ArrayTraits<PartitionedArray<E, r>>
{

public:

    using Element = E;

    constexpr static Rank rank = r;

    using Offset = typename PartitionedArray<E, r>::Offset;

    using Shape = typename PartitionedArray<E, r>::Shape;

    template<
        typename E_,
        Rank r_>
    using Partition = typename PartitionedArray<E_, r_>::Partition;

    template<
        typename E_,
        Rank r_>
    using Partitions = typename PartitionedArray<E_, r_>::Partitions;

    template<
        typename E_,
        Rank r_>
    using PartitionedArray = PartitionedArray<E_, r_>;

    template<
        typename E_,
        Rank r_>
    using Data = typename Partition<E_, r_>::Data;

};

}  // namespace detail


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank>::PartitionedArray():

    _shape{},
    _localities{},
    _partitions{}

{
}


/*!
    @brief      Construct an instance
    @param      shape Shape of the array
*/
template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank>::PartitionedArray(
    Shape const& shape):

    _shape{shape},
    _localities{},
    _partitions{}

{
    create();

    assert_invariants();
}


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank>::PartitionedArray(
    Shape const& shape,
    Shape const& max_partition_shape,
    ClampMode const clamp_mode):

    _shape{shape},
    _localities{},
    _partitions{}

{
    create(max_partition_shape, clamp_mode);

    assert_invariants();
}


template<
    typename Element,
    Rank rank>
PartitionedArray<Element, rank>::PartitionedArray(
    Shape const& shape,
    Shape const& max_partition_shape,
    Element const& fill_value,
    ClampMode const clamp_mode):

    _shape{shape},
    _localities{},
    _partitions{}

{
    create(max_partition_shape, fill_value, clamp_mode);

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
    Rank rank>
PartitionedArray<Element, rank>::PartitionedArray(
    Shape const& shape,
    Localities<rank> const& localities,
    Partitions&& partitions):

    _shape{shape},
    _localities{localities},
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
//     Rank rank>
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
        using Partition = PartitionT<Partitions>;
        using Shape = ShapeT<Partitions>;

        Partition& partition = this->partition();

        // Only once the partition is ready we can manipulate it
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
        partition = partition.then(
                [new_shape=this->_new_shape](
                    Partition&& partition)
                {
                    // Once the current shape is obtained we can
                    // reshape the partition
                    return partition.shape().then(
                        hpx::util::unwrapping(
                            [new_shape, partition](
                                Shape current_shape) mutable
                            {
                                AnnotateFunction annotation{"shrink_partition"};

                                Count const rank =
                                    static_cast<Count>(lue::rank<Partitions>);

                                // Update current shape given requested shape
                                for(Index i = 0; i < rank; ++i) {
                                    current_shape[i] =
                                        std::min(current_shape[i], new_shape[i]);
                                }

                                // FIXME Blocks current thread... Is this a problem?
                                //     Guess not, we are doing useful stuff.
                                partition.reshape(current_shape).wait();

                                return partition;
                            }
                        ));
                }
            );
#pragma GCC diagnostic pop
    }

private:

    //! New shape to use for partitions visited
    ShapeT<Partitions> const _new_shape;

};


template<
    typename Element,
    Rank rank>
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
    Rank rank>
void PartitionedArray<Element, rank>::clamp_array_shrink(
    Shape const& shape_in_partitions,
    Shape const& max_partition_shape)
{
    // Shrink partitions at the border of the array. They might be too large.
    // This happens when the extents of the array are not a multiple of the
    // corresponding extents in max_partition_shape.

    // Resize partitions at the sides of the array that are too large

    // Offset to iterate over partition along one of the dimensions
    // Index partition_idx_offset;

    // Begin and end indices of all partitions in the array
    Shape begin_indices;
    begin_indices.fill(0);
    Shape const end_indices{shape_in_partitions};

    // Iterate over all dimensions
    for(Rank d = 0; d < rank; ++d) {
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
        lue_assert(excess_cells < max_partition_shape[d]);

        if(excess_cells > 0) {
            // Resize current dimension of all partitions at currently
            // considered side of array
            // These partitions form a hyperslab of the array. We can
            // select this slab using start indices and end indices along
            // all dimensions. For the current dimension we set these
            // to the same value: the index of the last partition.
            //
            // In case of the other dimensions we set the start
            // indices to zero and the end indices to the last index
            // along each of these dimensions.

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
    typename Partitions>
class ResizeVisitor:
    public PartitionVisitor<Partitions>
{

private:

    using Base = PartitionVisitor<Partitions>;

public:

    ResizeVisitor(
        Partitions& partitions,
        Index const dimension_idx,
        Count const new_size):

        Base{partitions},
        _dimension_idx{dimension_idx},
        _new_size{new_size}

    {
        lue_assert(_dimension_idx >= 0);
        lue_assert(_dimension_idx < static_cast<Index>(rank<Partitions>));
    }

    void operator()()
    {
        using Partition = PartitionT<Partitions>;
        using Shape = ShapeT<Partitions>;

        Partition& partition = this->partition();

        // Only once the partition is ready we can manipulate it
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
        partition = partition.then(
                [dimension_idx=this->_dimension_idx, new_size=this->_new_size](
                    Partition&& partition)
                {
                    // Once the current shape is obtained we can
                    // reshape the partition
                    return partition.shape().then(
                        hpx::util::unwrapping(
                            [dimension_idx, new_size, partition](
                                Shape current_shape) mutable
                            {
                                AnnotateFunction annotation{"resize_partition"};

                                current_shape[dimension_idx] = new_size;

                                // FIXME Blocks current thread... Is this a problem?
                                //     Guess not, we are doing useful stuff.
                                partition.reshape(current_shape).wait();

                                return partition;
                            }
                        ));
                }
            );
#pragma GCC diagnostic pop
    }

private:

    Index          _dimension_idx;

    Count          _new_size;

};


template<
    typename Element,
    Rank rank>
void PartitionedArray<Element, rank>::resize_partitions(
    Shape const& begin_indices,
    Shape const& end_indices,
    Index const dimension_idx,
    Count const new_size)
{
    // Iterate over all partitions and resize them.
    // Array shape is not changed! It is assumed that the shape of
    // the partitioned array after resizing corresponds with the cached
    // array shape (_shape).
    ResizeVisitor resize{_partitions, dimension_idx, new_size};

    visit_array(begin_indices, end_indices, resize);
}


template<
    typename Element,
    Rank rank>
void PartitionedArray<Element, rank>::clamp_array_merge(
    Shape const& shape_in_partitions,
    Shape const& max_partition_shape)
{
    // Merge partitions at the border of the array. They might be too
    // large (overflow). This happens when the extents of the array are
    // not a multiple of the corresponding extents in max_partition_shape.

    // Begin and end indices of all partitions in the array
    Shape begin_indices;
    begin_indices.fill(0);
    Shape end_indices{shape_in_partitions};

    // Iterate over all dimensions
    for(Rank d = 0; d < rank; ++d) {

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
        lue_assert(excess_cells < max_partition_shape[d]);

        if(excess_cells > 0) {

            if(shape_in_partitions[d] == 1) {

                // Only one partition along the current dimension:
                // shrink it

                // Resize current dimension of all partitions at currently
                // considered side of array
                // These partitions form a hyperslab of the array. We can
                // select this slab using start indices and end indices along
                // all dimensions. For the current dimension we set these
                // to the same value: the index of the last partition.

                // In case of the other dimensions we set the start
                // indices to zero and the end indices to the last index
                // along each of these dimensions.

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
            else {

                // Multiple partitions along the current dimension:
                // get rid of the last one and enlarge the neighbouring
                // partition (along the current dimension)

                // We can select the relevant partitions using start
                // indices and end indices along all dimensions. For
                // the current dimension we set these to the same value:
                // the index of the last partitions.
                // In case of the other dimensions we set the start
                // indices to zero and the end indices to the last index
                // along each of these dimensions.

                Shape begin_indices_hyperslab{begin_indices};
                Shape end_indices_hyperslab{end_indices};

                /// // Remove the last partitions, along the current dimension
                /// begin_indices_hyperslab[d] = shape_in_partitions[d] - 1;
                /// end_indices_hyperslab[d] = shape_in_partitions[d];

                // Resize the one before the last partitions (which just
                // got erased), along the current dimension
                begin_indices_hyperslab[d] = shape_in_partitions[d] - 2;
                end_indices_hyperslab[d] = shape_in_partitions[d] - 1;

                Shape partition_shape{max_partition_shape};
                partition_shape[d] += max_partition_shape[d] - excess_cells;

                resize_partitions(
                    begin_indices_hyperslab, end_indices_hyperslab,
                    d, partition_shape[d]);

                _localities.erase(d, shape_in_partitions[d] - 1, shape_in_partitions[d]);
                _partitions.erase(d, shape_in_partitions[d] - 1, shape_in_partitions[d]);

                end_indices[d] -= 1;
            }
        }
    }
}


template<
    typename Element,
    Rank rank>
template<
    typename... Idxs>
void PartitionedArray<Element, rank>::instantiate_partition(
    hpx::id_type const locality_id,
    Shape const& partition_shape,
    Idxs... idxs)
{
    // idxs points to a cell (partition) in the partitioned
    // array. Instantiate this partition.

    _localities(idxs...) = locality_id;

    _partitions(idxs...) = hpx::async(

            [locality_id, partition_shape, idxs...]()
            {
                AnnotateFunction annotation{"instantiate_partition"};

                Offset offset{idxs...};

                for(std::size_t d = 0; d < rank; ++d) {
                    offset[d] *= partition_shape[d];
                }

                return Partition{locality_id, offset, partition_shape};
            }

        );
}


template<
    typename Element,
    Rank rank>
template<
    typename... Idxs>
void PartitionedArray<Element, rank>::instantiate_partition(
    hpx::id_type const locality_id,
    Shape const& partition_shape,
    Element const& fill_value,
    Idxs... idxs)
{
    // idxs points to a cell (partition) in the partitioned
    // array. Instantiate this partition.

    _localities(idxs...) = locality_id;

    _partitions(idxs...) = hpx::async(

            [locality_id, partition_shape, fill_value, idxs...]()
            {
                AnnotateFunction annotation{"instantiate_partition"};

                Offset offset{idxs...};

                for(std::size_t d = 0; d < rank; ++d) {
                    offset[d] *= partition_shape[d];
                }

                return Partition{locality_id, offset, partition_shape, fill_value};
            }

        );
}


template<
    typename Element,
    Rank rank>
void PartitionedArray<Element, rank>::instantiate_partitions(
    Shape const& shape_in_partitions,
    Shape const& partition_shape)
{
    // Create array containing partitions. Each of these partitions
    // will be a component client instance referring to a, possibly
    // remote, component server instance.
    lue_assert(_localities.empty());
    _localities = Localities<rank>{shape_in_partitions};
    lue_assert(_partitions.empty());
    _partitions = Partitions{shape_in_partitions};

    Count const nr_partitions = lue::nr_elements(shape_in_partitions);
    lue_assert(_partitions.nr_elements() == nr_partitions);

    std::vector<hpx::id_type> const localities = hpx::find_all_localities();
    Count const nr_localities = localities.size();

    lue_assert(nr_localities > 0);

    if(!BuildOptions::build_test)
    {
        // In general, the number of localities should be smaller than
        // the number of partitions. Otherwise more hardware is used than
        // necessary. The exception is when we are building with tests
        // turned on. Tests may have to run on less localities than there
        // are partitions.
        if(nr_partitions < nr_localities)
        {
            throw std::runtime_error(fmt::format(
                "Not enough partitions to use all localities ({} < {})",
                nr_partitions, nr_localities));
        }
    }

    lue_assert(hpx::find_here() == hpx::find_root_locality());
    lue_assert(localities[0] == hpx::find_root_locality());

    // Map partition index to locality index
    auto locality_idx =
        [nr_partitions, nr_localities](
            Index const partition_idx)
        {
            return map_to_range(
                Index{0}, nr_partitions - 1,
                Index{0}, nr_localities - 1,
                partition_idx);
        };

    if constexpr(rank == 2) {  //  || rank == 3) {

        // Visit all cells in the array in an order defined by the
        // Hilbert curve. The lambda will receive a linear partition index
        // (along the Hilbert curve!) and the indices of the array
        // partition.
        //
        // The indices of the array partition locate the partition to
        // be instantiated. The linear index can be used to determine
        // a locality to put the partition component on. By mapping
        // nearby linear indices to the same locality, nearby partitions
        // end up at the same locality.

        hilbert_curve<rank>(shape_in_partitions,

                [this, &localities, locality_idx, partition_shape](
                    Index const partition_idx,  // Along curve
                    /* Idxs... */ auto... idxs)  // Partition in array
                {
                    instantiate_partition<decltype(idxs)...>(
                        localities[locality_idx(partition_idx)],
                        partition_shape, idxs...);
                }

            );
    }
    else {

        linear_curve<rank>(shape_in_partitions,

                [this, &localities, locality_idx, partition_shape](
                    Index const partition_idx,  // Along curve
                    /* Idxs... */ auto... idxs)  // Partition in array
                {
                    instantiate_partition<decltype(idxs)...>(
                        localities[locality_idx(partition_idx)],
                        partition_shape, idxs...);
                }

            );
    }
}


template<
    typename Element,
    Rank rank>
void PartitionedArray<Element, rank>::instantiate_partitions(
    Shape const& shape_in_partitions,
    Shape const& partition_shape,
    Element const& fill_value)
{
    // Create array containing partitions. Each of these partitions
    // will be a component client instance referring to a, possibly
    // remote, component server instance.
    lue_assert(_localities.empty());
    _localities = Localities<rank>{shape_in_partitions};
    lue_assert(_partitions.empty());
    _partitions = Partitions{shape_in_partitions};

    Count const nr_partitions = lue::nr_elements(shape_in_partitions);
    lue_assert(_partitions.nr_elements() == nr_partitions);

    std::vector<hpx::id_type> const localities = hpx::find_all_localities();
    Count const nr_localities = localities.size();

    lue_assert(nr_localities > 0);

    if(!BuildOptions::build_test)
    {
        // In general, the number of localities should be smaller than
        // the number of partitions. Otherwise more hardware is used than
        // necessary. The exception is when we are building with tests
        // turned on. Tests may have to run on less localities than there
        // are partitions.
        if(nr_partitions < nr_localities)
        {
            throw std::runtime_error(fmt::format(
                "Not enough partitions to use all localities ({} < {})",
                nr_partitions, nr_localities));
        }
    }

    lue_assert(hpx::find_here() == hpx::find_root_locality());
    lue_assert(localities[0] == hpx::find_root_locality());

    // Map partition index to locality index
    auto locality_idx =
        [nr_partitions, nr_localities](
            Index const partition_idx)
        {
            return map_to_range(
                Index{0}, nr_partitions - 1,
                Index{0}, nr_localities - 1,
                partition_idx);
        };

    if constexpr(rank == 2) {  //  || rank == 3) {

        // Visit all cells in the array in an order defined by the
        // Hilbert curve. The lambda will receive a linear partition index
        // (along the Hilbert curve!) and the indices of the array
        // partition.
        //
        // The indices of the array partition locate the partition to
        // be instantiated. The linear index can be used to determine
        // a locality to put the partition component on. By mapping
        // nearby linear indices to the same locality, nearby partitions
        // end up at the same locality.

        hilbert_curve<rank>(shape_in_partitions,

                [this, &localities, locality_idx, partition_shape, fill_value](
                    Index const partition_idx,  // Along curve
                    /* Idxs... */ auto... idxs)  // Partition in array
                {
                    instantiate_partition<decltype(idxs)...>(
                        localities[locality_idx(partition_idx)],
                        partition_shape, fill_value, idxs...);
                }

            );
    }
    else {

        linear_curve<rank>(shape_in_partitions,

                [this, &localities, locality_idx, partition_shape, fill_value](
                    Index const partition_idx,  // Along curve
                    /* Idxs... */ auto... idxs)  // Partition in array
                {
                    instantiate_partition<decltype(idxs)...>(
                        localities[locality_idx(partition_idx)],
                        partition_shape, fill_value, idxs...);
                }

            );
    }
}


template<
    typename Element,
    Rank rank>
void PartitionedArray<Element, rank>::create(
    Shape const& shape_in_partitions,
    Shape const& max_partition_shape,
    ClampMode const clamp_mode)
{
    // Create the array partitions that, together make up the partitioned
    // array. Note that the extent of this array might be too large,
    // given that we use max_partition_shape.

    instantiate_partitions(shape_in_partitions, max_partition_shape);

    // TODO Refactor with other create: clamp(clamp_mode)
    switch(clamp_mode)
    {
        case ClampMode::shrink:
        {
            // Fix too large extent by shrinking overflowing partitions. This
            // may result is partitions that are too small for the focal
            // algorithms. Therefore, the alternative approach is more robust.
            clamp_array_shrink(shape_in_partitions, max_partition_shape);
            break;
        }
        case ClampMode::merge:
        {
            // Fix too large extent by merging relevant part of overflowing
            // partitions with bordering partitions. These will become larger
            // than the requested partition size. But focal algorithms will
            // behave more robust. Partitions in the array will be at least as
            // large as the partitions shape requested.
            clamp_array_merge(shape_in_partitions, max_partition_shape);
            break;
        }
    }

    assert_invariants();
}


template<
    typename Element,
    Rank rank>
void PartitionedArray<Element, rank>::create(
    Shape const& shape_in_partitions,
    Shape const& max_partition_shape,
    Element const& fill_value,
    ClampMode const clamp_mode)
{
    // Create the array partitions that, together make up the partitioned
    // array. Note that the extent of this array might be too large,
    // given that we use max_partition_shape.

    instantiate_partitions(shape_in_partitions, max_partition_shape, fill_value);

    // TODO Refactor with other create: clamp(clamp_mode)
    switch(clamp_mode)
    {
        case ClampMode::shrink:
        {
            // Fix too large extent by shrinking overflowing partitions. This
            // may result is partitions that are too small for the focal
            // algorithms. Therefore, the alternative approach is more robust.
            clamp_array_shrink(shape_in_partitions, max_partition_shape);
            break;
        }
        case ClampMode::merge:
        {
            // Fix too large extent by merging relevant part of overflowing
            // partitions with bordering partitions. These will become larger
            // than the requested partition size. But focal algorithms will
            // behave more robust. Partitions in the array will be at least as
            // large as the partitions shape requested.
            clamp_array_merge(shape_in_partitions, max_partition_shape);
            break;
        }
    }

    assert_invariants();
}


template<
    typename Element,
    Rank rank>
void PartitionedArray<Element, rank>::create()
{
    // Given the shape of the array and the number of localities,
    // determine the maximum partition shape and the shape of the array
    // in partitions

    // TODO blocks
    Count const nr_localities = hpx::get_num_localities().get();
    Shape const max_partition_shape =
        lue::max_partition_shape(_shape, nr_localities);
    auto const shape_in_partitions =
        lue::shape_in_partitions(_shape, max_partition_shape);

    create(shape_in_partitions, max_partition_shape, ClampMode::merge);
}


template<
    typename Element,
    Rank rank>
void PartitionedArray<Element, rank>::create(
    Shape const& max_partition_shape,
    ClampMode const clamp_mode)
{
    // Given the shape of the array and the shape of the array partitions,
    // determine the shape of the array in partitions

    auto const shape_in_partitions = lue::shape_in_partitions(_shape, max_partition_shape);

    create(shape_in_partitions, max_partition_shape, clamp_mode);
}


template<
    typename Element,
    Rank rank>
void PartitionedArray<Element, rank>::create(
    Shape const& max_partition_shape,
    Element const& fill_value,
    ClampMode const clamp_mode)
{
    // Given the shape of the array and the shape of the array partitions,
    // determine the shape of the array in partitions

    auto const shape_in_partitions = lue::shape_in_partitions(_shape, max_partition_shape);

    create(shape_in_partitions, max_partition_shape, fill_value, clamp_mode);
}


template<
    typename Partitions>
class ValidateVisitor:
    public ConstPartitionVisitor<Partitions>
{

private:

    using Base = ConstPartitionVisitor<Partitions>;

    using Partition = PartitionT<Partitions>;

    using Offset = typename Partition::Offset;

    using Shape = ShapeT<Partitions>;

    using Cursor = typename Base::Cursor;

public:

    ValidateVisitor(
        Partitions const& partitions,
        Shape const& shape):

        Base{partitions},
        _array_shape{shape}

    {
        _offset.fill(0);
    }

    void operator()()
    {
        // Given a cell, verify it is located within the array's shape, at
        // the expected offset

        Partition const& partition{this->partition()};
        partition.wait();
        Offset const partition_offset{partition.offset().get()};
        _partition_shape = partition.shape().get();
        Cursor const& cursor{this->cursor()};

        if(partition_offset != _offset) {
            throw std::runtime_error(fmt::format(
                "Partition {} is not located at the expected offset {} "
                "in the array but at {}",
                cursor.current_cell(), _offset, partition_offset));
        }

        auto const dimension_idx{cursor.dimension_idx()};

        // Given the current dimension, the next partition should be
        // offset by the offset of the current partition plus its shape
        _offset[dimension_idx] += _partition_shape[dimension_idx];
    }

    void leaving_current_dimension()
    {
        // dimension_idx is still pointing to the current (inner) dimension
        // In case of a raster, cells are visited row-wise.

        // When leaving a dimension, the offset for the inner dimension
        // + the extent for that dimension of the last seen partition must
        // match the partitioned array extent. Since we just visited
        // the last cell in the current dimension, the offset is already
        // located at the one-past-the-last cell. This value must equal
        // the extent of the array's shape in the current dimension.

        Cursor const& cursor{this->cursor()};
        auto const dimension_idx{cursor.dimension_idx()};

        if(_offset[dimension_idx] != _array_shape[dimension_idx])
        {
            throw std::runtime_error(fmt::format(
                "Partition {} with shape {} does not fit within the array "
                "shape {}",
                cursor.current_cell(), _partition_shape, _array_shape));
        }

        // Prepare for leaving the current dimension. In case of a raster,
        // we must increase the row offset by the number of rows in the
        // last seen partition and set the column offset to zero (similar
        // to a carriage return). We will be iterating over the cells
        // in next row next.
        lue_assert(dimension_idx > 0);
        _offset[dimension_idx - 1] += _partition_shape[dimension_idx - 1];
        _offset[dimension_idx] = 0;
    }

private:

    //! Shape of the array we are validating
    Shape const    _array_shape;

    //! Offset during visit
    Offset         _offset;

    //! Shape of last visited partition
    Shape          _partition_shape;

};


template<
    typename Element,
    Rank rank>
void PartitionedArray<Element, rank>::assert_invariants() const
{

    lue_assert(_partitions.shape() == _localities.shape());

    // The array is either empty, or all localities are valid / known
    lue_assert(
        _localities.empty() ||
        std::all_of(_localities.begin(), _localities.end(),
            [](hpx::id_type const locality_id)
            {
                return bool{locality_id};
            }));

    // WARNING This is an expensive and synchronous operation. Never do
    //     this in production code.

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


template<
    typename Element,
    Rank rank>
typename PartitionedArray<Element, rank>::Count
    PartitionedArray<Element, rank>::nr_elements() const
{
    return lue::nr_elements(_shape);
}


template<
    typename Element,
    Rank rank>
typename PartitionedArray<Element, rank>::Shape const&
    PartitionedArray<Element, rank>::shape() const
{
    return _shape;
}


template<
    typename Element,
    Rank rank>
typename PartitionedArray<Element, rank>::Count
    PartitionedArray<Element, rank>::nr_partitions() const
{
    return _partitions.nr_elements();
}


template<
    typename Element,
    Rank rank>
typename PartitionedArray<Element, rank>::Partitions&
    PartitionedArray<Element, rank>::partitions()
{
    return _partitions;
}


template<
    typename Element,
    Rank rank>
typename PartitionedArray<Element, rank>::Partitions const&
    PartitionedArray<Element, rank>::partitions() const
{
    return _partitions;
}


template<
    typename Element,
    Rank rank>
typename PartitionedArray<Element, rank>::ConstIterator
    PartitionedArray<Element, rank>::begin() const
{
    return _partitions.begin();
}


template<
    typename Element,
    Rank rank>
typename PartitionedArray<Element, rank>::Iterator
    PartitionedArray<Element, rank>::begin()
{
    return _partitions.begin();
}


template<
    typename Element,
    Rank rank>
typename PartitionedArray<Element, rank>::ConstIterator
    PartitionedArray<Element, rank>::end() const
{
    return _partitions.end();
}


template<
    typename Element,
    Rank rank>
typename PartitionedArray<Element, rank>::Iterator
    PartitionedArray<Element, rank>::end()
{
    return _partitions.end();
}


template<
    typename Element,
    Rank rank>
Localities<rank> const& PartitionedArray<Element, rank>::localities() const
{
    return _localities;
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
    Rank rank>
ShapeT<PartitionedArray<Element, rank>> shape(
    PartitionedArray<Element, rank> const& array)
{
    return array.shape();
}


template<
    typename Element,
    Rank rank>
ShapeT<PartitionedArray<Element, rank>> shape_in_partitions(
    PartitionedArray<Element, rank> const& array)
{
    return array.partitions().shape();
}


template<
    typename Element,
    Rank rank>
Count nr_partitions(
    PartitionedArray<Element, rank> const& array)
{
    return array.nr_partitions();
}


template<
    typename Element,
    Rank rank>
Count nr_partitions(
    ArrayPartitionData<ArrayPartition<Element, rank>, rank> const& partitions)
{
    return partitions.nr_elements();
}


template<
    typename Element,
    Rank rank>
typename PartitionedArray<Element, rank>::Count nr_elements(
    PartitionedArray<Element, rank> const& array)
{
    return array.nr_elements();
}


template<
    typename Element,
    Rank rank>
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
    Rank rank>
std::string describe(
    PartitionedArray<Element, rank> const& array)
{
    return fmt::format(
        "- PartitionedArray:\n"
        "    - array of elements  : {} ({} elements)\n"
        "    - array of partitions: {} ({} partitions)\n"
        ,
        describe(array.shape()),
        array.nr_elements(),
        describe(array.partitions().shape()),
        array.partitions().nr_elements()
    );
}

}  // namespace lue
