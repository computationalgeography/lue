#pragma once
#include "lue/framework/core/component/array_partition.hpp"
#include "lue/framework/core/array_partition_data.hpp"
#include "lue/framework/core/array_partition_visitor.hpp"
#include "lue/framework/core/debug.hpp"  // describe
#include "lue/framework/core/domain_decomposition.hpp"
#include "lue/framework/core/math.hpp"
#include <initializer_list>


namespace lue {

template<
    typename Element,
    Rank rank>
class PartitionedArray
{

private:

    using PartitionClient = ArrayPartition<Element, rank>;

    using PartitionServer = typename PartitionClient::Server;

public:

    using Partition = PartitionClient;

    using Partitions = ArrayPartitionData<Partition, rank>;

    using Count = typename Partitions::Count;

    using Shape = typename Partitions::Shape;

    using Iterator = typename Partitions::Iterator;

    using ConstIterator = typename Partitions::ConstIterator;

                   PartitionedArray    ();

                   PartitionedArray    (PartitionedArray const& other)=delete;

                   PartitionedArray    (PartitionedArray&& other)=default;

    explicit       PartitionedArray    (Shape const& shape);

                   PartitionedArray    (Shape const& shape,
                                        Shape const& max_partition_shape);

                   PartitionedArray    (Shape const& shape,
                                        Partitions&& partitions);

                   ~PartitionedArray   ()=default;

    PartitionedArray& operator=        (PartitionedArray const& other)=delete;

    PartitionedArray& operator=        (PartitionedArray&& other)=default;

    Count          nr_elements         () const;

    Shape const&   shape               () const;

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
    Rank rank>
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

        partition = hpx::dataflow(
            hpx::launch::async,
            // hpx::util::annotated_function(

                [new_shape=this->_new_shape](
                    Partition&& partition,
                    hpx::future<Shape>&& f_partition_shape) // -> Partition
                {
                    Shape partition_shape = f_partition_shape.get();
                    Count const rank = static_cast<Count>(lue::rank<Partitions>);

                    for(Index i = 0; i < rank; ++i) {
                        partition_shape[i] =
                            std::min(partition_shape[i], new_shape[i]);
                    }

                    // FIXME Blocks current thread... Is this a problem?
                    partition.reshape(partition_shape).wait();

                    return partition;

                },

                // "shrink_partition),
            partition,
            partition.shape());

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
void PartitionedArray<Element, rank>::clamp_array(
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
    Rank rank>
void PartitionedArray<Element, rank>::create(
    Shape const& shape_in_partitions,
    Shape const& max_partition_shape)
{
    // Create the array partitions that, together make up the partitioned
    // array. Note that the extent of this array might be too large,
    // given that we use max_partition_shape. This will be fixed using
    // clamp_array.

    Count const nr_partitions = lue::nr_elements(shape_in_partitions);

    // Create array containing partitions. Each of these partitions will be
    // a component client instance referring to a, possibly remote,
    // component server instance.
    _partitions = Partitions{shape_in_partitions, scattered_target_index()};
    assert(_partitions.nr_elements() == nr_partitions);

    std::vector<hpx::naming::id_type> const localities =
        hpx::find_all_localities();
    Count const nr_localities = localities.size();

    assert(nr_localities > 0);

    if(nr_partitions < nr_localities) {
        throw std::runtime_error(fmt::format(
            "Not enough partitions to use all localities ({} < {})",
            nr_partitions, nr_localities));
    }

    assert(hpx::find_here() == hpx::find_root_locality());
    assert(localities[0] == hpx::find_root_locality());

    // create_partitions(max_partition_shape);
    {
        auto locality_idx =
            [nr_partitions, nr_localities](
                Index const p) -> Index
            {
                return map_to_range(
                    Index{0}, nr_partitions - 1,
                    Index{0}, nr_localities - 1,
                    p);
            };

        // Create array partitions. Each of them will be located on a certain
        // locality. Which one exactly is determined by locality_idx.
        for(Index partition_idx = 0; partition_idx < nr_partitions;
                ++partition_idx) {

            auto idx = locality_idx(partition_idx);
            auto locality = localities[idx];

            // _partitions[partition_idx] = Partition{
            //     localities[idx], max_partition_shape};

            // Assign a future to a partition component to a partition
            // component. The partition will become ready once it is
            // allocated.
            _partitions[partition_idx] = hpx::async(
                    hpx::util::annotated_function(

                        [locality, max_partition_shape]()
                        {
                            return Partition{locality, max_partition_shape};
                        },

                    "instantiate_partition")
                );

        }

    }

    clamp_array(shape_in_partitions, max_partition_shape);

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

    create(shape_in_partitions, max_partition_shape);
}


template<
    typename Element,
    Rank rank>
void PartitionedArray<Element, rank>::create(
    Shape const& max_partition_shape)
{
    // Given the shape of the array and the shape of the array partitions,
    // determine the shape of the array in partitions

    auto const shape_in_partitions =
        lue::shape_in_partitions(_shape, max_partition_shape);

    create(shape_in_partitions, max_partition_shape);
}


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
    Rank rank>
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
