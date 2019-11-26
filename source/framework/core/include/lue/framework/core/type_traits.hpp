#pragma once
#include "lue/framework/core/define.hpp"
#include <type_traits>


namespace lue {
namespace detail {

/*!
    @brief      Primary template for traits of an array-like type
    @tparam     Array-like type

    Specializations add traits that are relevant for the actual array-like
    type. Some of these traits are relevant for all array-like types
    (value-type of the elements, rank, ...).
*/
template<
    typename Array>
class ArrayTraits
{

public:

};


template<class T>
class IsArrayPartition:
    public std::false_type
{};

}  // namespace detail


// Alias templates cannot be specialized, so the implementations below use
// a class template (detail::ArrayTraits), which can be specialized.


/*!
    @brief      Alias template of type aliases for the element types
    @tparam     Array Array-like type

    Given @a Array, this template generates a type alias for the
    element type.
*/
template<
    typename Array>
using ElementT = typename detail::ArrayTraits<Array>::Element;


/*!
    @brief      Rank of an array-like type
    @tparam     Array-like type
*/
template<
    typename Array>
static constexpr Rank rank = detail::ArrayTraits<Array>::rank;


/*!
    @brief      Alias template of type aliases for the shape types
    @tparam     Array Array-like type

    Given @a Array, this template generates a type alias for the
    shape type.
*/
template<
    typename Array>
using ShapeT = typename detail::ArrayTraits<Array>::Shape;


template<
    typename Array>
using SlicesT = typename detail::ArrayTraits<Array>::Slices;


template<
    typename Array>
using SliceT = typename detail::ArrayTraits<Array>::Slice;


/*!
    @brief      Alias template of type aliases for container types of
                array elements, instantiated for a given element type
                and rank
    @tparam     Array Array-like type
    @tparam     Element Element type
    @tparam     rank rank

    Given @a Array, @a Element and @a rank, this template generates a
    type alias for the container type.
*/
template<
    typename Array,
    typename Element=ElementT<Array>,
    Rank rank=rank<Array>>
using DataT =
    typename detail::ArrayTraits<Array>::template Data<Element, rank>;


/*!
    @brief      Alias template of type aliases for partition types,
                instantiated for a given element type
    @tparam     Array Partitioned array-like type
    @tparam     Element Element type
    @tparam     rank rank

    Given @a Array, @a Element and @a rank, this template generates a type
    alias for the partition type.
*/
template<
    typename Array,
    typename Element=ElementT<Array>,
    Rank rank=rank<Array>>
using PartitionT =
    typename detail::ArrayTraits<Array>::template Partition<Element, rank>;


/*!
    @brief      Alias template of type aliases for partition collection
                types, instantiated for a given element type and rank
    @tparam     Array Partitioned array-like type
    @tparam     Element Element type
    @tparam     rank rank

    Given @a Array, @a Element and @a rank, this template generates a
    type alias for the partition collection.
*/
template<
    typename Array,
    typename Element=ElementT<Array>,
    Rank rank=rank<Array>>
using PartitionsT =
    typename detail::ArrayTraits<Array>::template Partitions<Element, rank>;


/*!
    @brief      Alias template of type aliases for partitioned array
                types, instantiated for a given element type and rank
    @tparam     Array Partitioned array-like type
    @tparam     Element Element type
    @tparam     rank rank

    Given @a Array, @a Element and @a rank, this template generates a
    type alias for the partitioned array.
*/
template<
    typename Array,
    typename Element=ElementT<Array>,
    Rank rank=rank<Array>>
using PartitionedArrayT =
    typename detail::ArrayTraits<Array>::
        template PartitionedArray<Element, rank>;

}  // namespace lue
