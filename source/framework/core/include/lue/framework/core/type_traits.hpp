#pragma once
#include "lue/framework/core/define.hpp"
#include <type_traits>


namespace lue {
namespace detail {

// Since C++20
template<
    typename T>
struct remove_cvref
{
    using type = std::remove_cv_t<std::remove_reference_t<T>>;
};


// Since C++20
template<
    typename T>
using remove_cvref_t = typename remove_cvref<T>::type;


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
using ElementT = detail::remove_cvref_t<typename detail::ArrayTraits<Array>::Element>;


/*!
    @brief      Rank of an array-like type
    @tparam     Array-like type
*/
template<
    typename Array>
static constexpr Rank rank = detail::ArrayTraits<detail::remove_cvref_t<Array>>::rank;


template<
    typename Array>
using OffsetT = detail::remove_cvref_t<typename detail::ArrayTraits<Array>::Offset>;


/*!
    @brief      Alias template of type aliases for the shape types
    @tparam     Array Array-like type

    Given @a Array, this template generates a type alias for the
    shape type.
*/
template<
    typename Array>
using ShapeT = detail::remove_cvref_t<typename detail::ArrayTraits<Array>::Shape>;


template<
    typename Array>
using SlicesT = detail::remove_cvref_t<typename detail::ArrayTraits<Array>::Slices>;


template<
    typename Array>
using SliceT = detail::remove_cvref_t<typename detail::ArrayTraits<Array>::Slice>;


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
using DataT = detail::remove_cvref_t<typename detail::ArrayTraits<Array>::template Data<Element, rank>>;


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
    detail::remove_cvref_t<
            typename detail::ArrayTraits<Array>::template Partition<Element, rank>
        >;


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
    detail::remove_cvref_t<
            typename detail::ArrayTraits<Array>::template Partitions<Element, rank>
        >;


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
    detail::remove_cvref_t<
            typename detail::ArrayTraits<Array>::template PartitionedArray<Element, rank>
        >;


template<
    typename Array>
inline constexpr bool is_array_partition_v =
    detail::IsArrayPartition<Array>::value;


template<
    typename Array>
Count              nr_partitions       (Array const& array);

}  // namespace lue
