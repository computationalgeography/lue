#pragma once


#define LUE_BINARY_ARITHMETIC_OPERATOR(symbol, name)                                                         \
    template<typename Element, Rank rank>                                                                    \
    PartitionedArray<Element, rank> operator symbol(                                                         \
        PartitionedArray<Element, rank> const& lhs, PartitionedArray<Element, rank> const& rhs)              \
    {                                                                                                        \
        return name(lhs, rhs);                                                                               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    template<typename Element, Rank rank>                                                                    \
    PartitionedArray<Element, rank> operator symbol(                                                         \
        PartitionedArray<Element, rank> const& lhs, hpx::shared_future<Element> const& rhs)                  \
    {                                                                                                        \
        return name(lhs, rhs);                                                                               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    template<typename Element, Rank rank>                                                                    \
    PartitionedArray<Element, rank> operator symbol(                                                         \
        hpx::shared_future<Element> const& lhs, PartitionedArray<Element, rank> const& rhs)                  \
    {                                                                                                        \
        return name(lhs, rhs);                                                                               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    template<typename Element, Rank rank>                                                                    \
    PartitionedArray<Element, rank> operator symbol(                                                         \
        PartitionedArray<Element, rank> const& lhs, Element const rhs)                                       \
    {                                                                                                        \
        return name(lhs, rhs);                                                                               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    template<typename Element, Rank rank>                                                                    \
    PartitionedArray<Element, rank> operator symbol(                                                         \
        Element const lhs, PartitionedArray<Element, rank> const& rhs)                                       \
    {                                                                                                        \
        return name(lhs, rhs);                                                                               \
    }


#define LUE_BINARY_ARITHMETIC_OPERATOR2(symbol, name)                                                        \
                                                                                                             \
    /* array + array */                                                                                      \
    template<typename Element, Rank rank>                                                                    \
    PartitionedArray<Element, rank> operator symbol(                                                         \
        PartitionedArray<Element, rank> const& lhs, PartitionedArray<Element, rank> const& rhs)              \
    {                                                                                                        \
        return name(lhs, rhs);                                                                               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* array + scalar */                                                                                     \
    template<typename Element, Rank rank>                                                                    \
    PartitionedArray<Element, rank> operator symbol(                                                         \
        PartitionedArray<Element, rank> const& lhs, Scalar<Element> const& rhs)                              \
    {                                                                                                        \
        return name(lhs, rhs);                                                                               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* array + value */                                                                                      \
    template<typename Element, Rank rank>                                                                    \
    PartitionedArray<Element, rank> operator symbol(                                                         \
        PartitionedArray<Element, rank> const& lhs, Element const& rhs)                                      \
    {                                                                                                        \
        return name(lhs, rhs);                                                                               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* scalar + array */                                                                                     \
    template<typename Element, Rank rank>                                                                    \
    PartitionedArray<Element, rank> operator symbol(                                                         \
        Scalar<Element> const& lhs, PartitionedArray<Element, rank> const& rhs)                              \
    {                                                                                                        \
        return name(lhs, rhs);                                                                               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* value + array */                                                                                      \
    template<typename Element, Rank rank>                                                                    \
    PartitionedArray<Element, rank> operator symbol(                                                         \
        Element const& lhs, PartitionedArray<Element, rank> const& rhs)                                      \
    {                                                                                                        \
        return name(lhs, rhs);                                                                               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* scalar + scalar */                                                                                    \
    template<typename Element>                                                                               \
    auto operator symbol(Scalar<Element> const& lhs, Scalar<Element> const& rhs)->Scalar<Element>            \
    {                                                                                                        \
        return name(lhs, rhs);                                                                               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* scalar + value */                                                                                     \
    template<typename Element>                                                                               \
    auto operator symbol(Scalar<Element> const& lhs, Element const& rhs)->Scalar<Element>                    \
    {                                                                                                        \
        return name(lhs, rhs);                                                                               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* value + scalar */                                                                                     \
    template<typename Element>                                                                               \
    auto operator symbol(Element const& lhs, Scalar<Element> const& rhs)->Scalar<Element>                    \
    {                                                                                                        \
        return name(lhs, rhs);                                                                               \
    }


#define LUE_BINARY_COMPARISON_OPERATOR(symbol, name)                                                         \
    template<typename Element, Rank rank>                                                                    \
    PartitionedArray<std::uint8_t, rank> operator symbol(                                                    \
        PartitionedArray<Element, rank> const& lhs, PartitionedArray<Element, rank> const& rhs)              \
    {                                                                                                        \
        return name(lhs, rhs);                                                                               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    template<typename Element, Rank rank>                                                                    \
    PartitionedArray<std::uint8_t, rank> operator symbol(                                                    \
        PartitionedArray<Element, rank> const& lhs, hpx::shared_future<Element> const& rhs)                  \
    {                                                                                                        \
        return name(lhs, rhs);                                                                               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    template<typename Element, Rank rank>                                                                    \
    PartitionedArray<std::uint8_t, rank> operator symbol(                                                    \
        hpx::shared_future<Element> const& lhs, PartitionedArray<Element, rank> const& rhs)                  \
    {                                                                                                        \
        return name(lhs, rhs);                                                                               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    template<typename Element, Rank rank>                                                                    \
    PartitionedArray<std::uint8_t, rank> operator symbol(                                                    \
        PartitionedArray<Element, rank> const& lhs, Element const rhs)                                       \
    {                                                                                                        \
        return name(lhs, rhs);                                                                               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    template<typename Element, Rank rank>                                                                    \
    PartitionedArray<std::uint8_t, rank> operator symbol(                                                    \
        Element const lhs, PartitionedArray<Element, rank> const& rhs)                                       \
    {                                                                                                        \
        return name(lhs, rhs);                                                                               \
    }


#define LUE_UNARY_LOGICAL_OPERATOR(symbol, name)                                                             \
    template<typename Element, Rank rank>                                                                    \
    PartitionedArray<std::uint8_t, rank> operator symbol(PartitionedArray<Element, rank> const& array)       \
    {                                                                                                        \
        return name(array);                                                                                  \
    }


#define LUE_BINARY_LOGICAL_OPERATOR(symbol, name)                                                            \
    template<typename Element, Rank rank>                                                                    \
    PartitionedArray<std::uint8_t, rank> operator symbol(                                                    \
        PartitionedArray<Element, rank> const& lhs, PartitionedArray<Element, rank> const& rhs)              \
    {                                                                                                        \
        return name(lhs, rhs);                                                                               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    template<typename Element, Rank rank>                                                                    \
    PartitionedArray<std::uint8_t, rank> operator symbol(                                                    \
        PartitionedArray<Element, rank> const& lhs, hpx::shared_future<Element> const& rhs)                  \
    {                                                                                                        \
        return name(lhs, rhs);                                                                               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    template<typename Element, Rank rank>                                                                    \
    PartitionedArray<std::uint8_t, rank> operator symbol(                                                    \
        hpx::shared_future<Element> const& lhs, PartitionedArray<Element, rank> const& rhs)                  \
    {                                                                                                        \
        return name(lhs, rhs);                                                                               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    template<typename Element, Rank rank>                                                                    \
    PartitionedArray<std::uint8_t, rank> operator symbol(                                                    \
        PartitionedArray<Element, rank> const& lhs, Element const rhs)                                       \
    {                                                                                                        \
        return name(lhs, rhs);                                                                               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    template<typename Element, Rank rank>                                                                    \
    PartitionedArray<std::uint8_t, rank> operator symbol(                                                    \
        Element const lhs, PartitionedArray<Element, rank> const& rhs)                                       \
    {                                                                                                        \
        return name(lhs, rhs);                                                                               \
    }
