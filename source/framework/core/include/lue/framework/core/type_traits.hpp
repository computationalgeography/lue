#pragma once
#include "lue/framework/core/define.hpp"
#include "lue/concept.hpp"
#include <type_traits>


namespace lue {
    namespace detail {

        /*!
            @brief      Primary template for traits of an array-like type
            @tparam     Array-like type
            @todo       Rename to TypeTraits (FieldTraits?)

            Specializations add traits that are relevant for the actual array-like
            type. Some of these traits are relevant for all array-like types
            (value-type of the elements, rank, ...).
        */
        template<typename Array>
        class ArrayTraits
        {

            public:

                static constexpr bool is_partitioned_array{false};
        };


        template<Arithmetic T>
        class ArrayTraits<T>
        {

            public:

                using Element = T;
        };


        template<Arithmetic T>
        class ArrayTraits<T*>
        {

            public:

                using Element = T;
        };


        template<class T>
        class IsArrayPartition: public std::false_type
        {
        };

    }  // namespace detail


    // Alias templates cannot be specialized, so the implementations below use
    // a class template (detail::ArrayTraits), which can be specialized.


    /*!
        @brief      Alias template of type aliases for the element types
        @tparam     Array Array-like type

        Given @a Array, this template generates a type alias for the
        element type.
    */
    template<typename Array>
    using ElementT = typename detail::ArrayTraits<std::remove_cvref_t<Array>>::Element;


    template<typename Array>
    using ElementsT = typename detail::ArrayTraits<std::remove_cvref_t<Array>>::Elements;


    namespace detail {

        template<typename Shaped>
        struct ShapedTraits
        {
                static constexpr Rank rank = detail::ArrayTraits<std::remove_cvref_t<Shaped>>::rank;
        };


        template<TupleLike T>
        struct ShapedTraits<T>
        {
                static constexpr Rank rank = std::tuple_size_v<T>;
        };

    }  // namespace detail


    /*!
        @brief      Rank of a type
        @tparam     Array-like type or a tuple containing extents
    */
    template<typename Shaped>
    static constexpr Rank rank = detail::ShapedTraits<Shaped>::rank;


    template<typename Array>
    using OffsetT = typename detail::ArrayTraits<std::remove_cvref_t<Array>>::Offset;


    /*!
        @brief      Alias template of type aliases for the shape types
        @tparam     Array Array-like type

        Given @a Array, this template generates a type alias for the
        shape type.
    */
    template<typename Array>
    using ShapeT = typename detail::ArrayTraits<std::remove_cvref_t<Array>>::Shape;


    template<typename Array>
    using SlicesT = typename detail::ArrayTraits<std::remove_cvref_t<Array>>::Slices;


    template<typename Array>
    using SliceT = typename detail::ArrayTraits<std::remove_cvref_t<Array>>::Slice;


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
    template<typename Array, typename Element = ElementT<Array>, Rank rank = rank<Array>>
    using DataT = typename detail::ArrayTraits<
        std::remove_cvref_t<Array>>::template Data<std::remove_cvref_t<Element>, rank>;


    /*!
        @brief      Alias template of type aliases for partition types,
                    instantiated for a given element type
        @tparam     Array Partitioned array-like type
        @tparam     Element Element type
        @tparam     rank rank

        Given @a Array, @a Element and @a rank, this template generates a type
        alias for the partition type.
    */
    template<typename Array, typename Element = ElementT<Array>, Rank rank = rank<Array>>
    using PartitionT = typename detail::ArrayTraits<
        std::remove_cvref_t<Array>>::template Partition<std::remove_cvref_t<Element>, rank>;


    template<typename Array, Rank rank = rank<Array>>
    using PartitionT2 = typename detail::ArrayTraits<std::remove_cvref_t<Array>>::template Partition<rank>;


    /*!
        @brief      Alias template of type aliases for partition collection
                    types, instantiated for a given element type and rank
        @tparam     Array Partitioned array-like type
        @tparam     Element Element type
        @tparam     rank rank

        Given @a Array, @a Element and @a rank, this template generates a
        type alias for the partition collection.
    */
    template<typename Array, typename Element = ElementT<Array>, Rank rank = rank<Array>>
    using PartitionsT = typename detail::ArrayTraits<
        std::remove_cvref_t<Array>>::template Partitions<std::remove_cvref_t<Element>, rank>;


    template<typename Array, Rank rank = rank<Array>>
    using PartitionsT2 = typename detail::ArrayTraits<std::remove_cvref_t<Array>>::template Partitions<rank>;


    template<typename Array, typename Element = ElementT<Array>, Rank rank = rank<Array>>
    using ComponentT = typename detail::ArrayTraits<
        std::remove_cvref_t<Array>>::template Component<std::remove_cvref_t<Element>, rank>;


    template<typename Array, typename Element = ElementT<Array>, Rank rank = rank<Array>>
    using ComponentsT = typename detail::ArrayTraits<
        std::remove_cvref_t<Array>>::template Components<std::remove_cvref_t<Element>, rank>;


    /*!
        @brief      Alias template of type aliases for partitioned array
                    types, instantiated for a given element type and rank
        @tparam     Array Partitioned array-like type
        @tparam     Element Element type
        @tparam     rank rank

        Given @a Array, @a Element and @a rank, this template generates a
        type alias for the partitioned array.
    */
    template<typename Array, typename Element = ElementT<Array>, Rank rank = rank<Array>>
    using PartitionedArrayT = typename detail::ArrayTraits<
        std::remove_cvref_t<Array>>::template PartitionedArray<std::remove_cvref_t<Element>, rank>;


    template<typename Array>
    inline constexpr bool is_partitioned_array_v = detail::ArrayTraits<Array>::is_partitioned_array;


    template<typename Array>
    inline constexpr bool is_array_partition_v = detail::IsArrayPartition<Array>::value;


    template<typename Array>
    Count nr_partitions(Array const& array);


    template<typename T>
    struct TypeTraits
    {
    };


    template<>
    struct TypeTraits<std::uint8_t>
    {
            static constexpr auto name{"uint8"};
    };


    template<>
    struct TypeTraits<std::int8_t>
    {
            static constexpr auto name{"int8"};
    };


    template<>
    struct TypeTraits<std::uint32_t>
    {
            static constexpr auto name{"uint32"};
    };


    template<>
    struct TypeTraits<std::int32_t>
    {
            static constexpr auto name{"int32"};
    };


    template<>
    struct TypeTraits<std::uint64_t>
    {
            static constexpr auto name{"uint64"};
    };


    template<>
    struct TypeTraits<std::int64_t>
    {
            static constexpr auto name{"int64"};
    };


    template<>
    struct TypeTraits<float>
    {
            static constexpr auto name{"float32"};
    };


    template<>
    struct TypeTraits<double>
    {
            static constexpr auto name{"float64"};
    };


    /*!
        @brief      String representation of @Element
        @tparam     Element Element type
    */
    template<typename Element>
    constexpr auto as_string = TypeTraits<Element>::name;

}  // namespace lue
