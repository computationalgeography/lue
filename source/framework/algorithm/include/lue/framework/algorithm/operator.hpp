#pragma once


#define LUE_BINARY_ARITHMETIC_OPERATOR(symbol, name)                                                         \
                                                                                                             \
    /* array + array */                                                                                      \
    template<typename Element, Rank rank>                                                                    \
    auto operator symbol(                                                                                    \
        PartitionedArray<Element, rank> const& lhs, PartitionedArray<Element, rank> const& rhs)              \
        ->PartitionedArray<Element, rank>                                                                    \
    {                                                                                                        \
        return name(lhs, rhs);                                                                               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* array + scalar */                                                                                     \
    template<typename Element, Rank rank>                                                                    \
    auto operator symbol(PartitionedArray<Element, rank> const& lhs, Scalar<Element> const& rhs)             \
        ->PartitionedArray<Element, rank>                                                                    \
    {                                                                                                        \
        return name(lhs, rhs);                                                                               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* array + value */                                                                                      \
    template<typename Element, Rank rank>                                                                    \
    auto operator symbol(PartitionedArray<Element, rank> const& lhs, Element const& rhs)                     \
        ->PartitionedArray<Element, rank>                                                                    \
    {                                                                                                        \
        return name(lhs, rhs);                                                                               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* scalar + array */                                                                                     \
    template<typename Element, Rank rank>                                                                    \
    auto operator symbol(Scalar<Element> const& lhs, PartitionedArray<Element, rank> const& rhs)             \
        ->PartitionedArray<Element, rank>                                                                    \
    {                                                                                                        \
        return name(lhs, rhs);                                                                               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    /* value + array */                                                                                      \
    template<typename Element, Rank rank>                                                                    \
    auto operator symbol(Element const& lhs, PartitionedArray<Element, rank> const& rhs)                     \
        ->PartitionedArray<Element, rank>                                                                    \
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


#define LUE_BINARY_COMPARISON_OPERATOR(symbol, name, BooleanElement)                                         \
                                                                                                             \
    /* array == array */                                                                                     \
    template<typename Element, Rank rank>                                                                    \
    auto operator symbol(                                                                                    \
        PartitionedArray<Element, rank> const& lhs, PartitionedArray<Element, rank> const& rhs)              \
        ->PartitionedArray<BooleanElement, rank>                                                             \
    {                                                                                                        \
        return name<BooleanElement>(lhs, rhs);                                                               \
    }                                                                                                        \
                                                                                                             \
    /* array == scalar */                                                                                    \
    template<typename Element, Rank rank>                                                                    \
    auto operator symbol(PartitionedArray<Element, rank> const& lhs, Scalar<Element> const& rhs)             \
        ->PartitionedArray<BooleanElement, rank>                                                             \
    {                                                                                                        \
        return name<BooleanElement>(lhs, rhs);                                                               \
    }                                                                                                        \
                                                                                                             \
    /* scalar == array */                                                                                    \
    template<typename Element, Rank rank>                                                                    \
    auto operator symbol(Scalar<Element> const& lhs, PartitionedArray<Element, rank> const& rhs)             \
        ->PartitionedArray<BooleanElement, rank>                                                             \
    {                                                                                                        \
        return name<BooleanElement>(lhs, rhs);                                                               \
    }                                                                                                        \
                                                                                                             \
    /* array == value */                                                                                     \
    template<typename Element, Rank rank>                                                                    \
    auto operator symbol(PartitionedArray<Element, rank> const& lhs, Element const rhs)                      \
        ->PartitionedArray<BooleanElement, rank>                                                             \
    {                                                                                                        \
        return name<BooleanElement>(lhs, rhs);                                                               \
    }                                                                                                        \
                                                                                                             \
    /* value == array */                                                                                     \
    template<typename Element, Rank rank>                                                                    \
    auto operator symbol(Element const lhs, PartitionedArray<Element, rank> const& rhs)                      \
        ->PartitionedArray<BooleanElement, rank>                                                             \
    {                                                                                                        \
        return name<BooleanElement>(lhs, rhs);                                                               \
    }                                                                                                        \
                                                                                                             \
    /* scalar == scalar */                                                                                   \
    template<typename Element>                                                                               \
    auto operator symbol(Scalar<Element> const& lhs, Scalar<Element> const& rhs)->Scalar<BooleanElement>     \
    {                                                                                                        \
        return name<BooleanElement>(lhs, rhs);                                                               \
    }                                                                                                        \
                                                                                                             \
    /* scalar == value */                                                                                    \
    template<typename Element>                                                                               \
    auto operator symbol(Scalar<Element> const& lhs, Element const rhs)->Scalar<BooleanElement>              \
    {                                                                                                        \
        return name<BooleanElement>(lhs, rhs);                                                               \
    }                                                                                                        \
                                                                                                             \
    /* value == scalar */                                                                                    \
    template<typename Element>                                                                               \
    auto operator symbol(Element const lhs, Scalar<Element> const& rhs)->Scalar<BooleanElement>              \
    {                                                                                                        \
        return name<BooleanElement>(lhs, rhs);                                                               \
    }


#define LUE_UNARY_LOGICAL_OPERATOR(symbol, name, BooleanElement)                                             \
    template<typename Element, Rank rank>                                                                    \
    auto operator symbol(PartitionedArray<Element, rank> const& array)                                       \
        ->PartitionedArray<BooleanElement, rank>                                                             \
    {                                                                                                        \
        return name<BooleanElement>(array);                                                                  \
    }                                                                                                        \
                                                                                                             \
    template<typename Element>                                                                               \
    auto operator symbol(Scalar<Element> const& scalar)->Scalar<BooleanElement>                              \
    {                                                                                                        \
        return name<BooleanElement>(scalar);                                                                 \
    }


#define LUE_BINARY_LOGICAL_OPERATOR(symbol, name, BooleanElement)                                            \
    template<typename Element, Rank rank>                                                                    \
    auto operator symbol(                                                                                    \
        PartitionedArray<Element, rank> const& lhs, PartitionedArray<Element, rank> const& rhs)              \
        ->PartitionedArray<BooleanElement, rank>                                                             \
    {                                                                                                        \
        return name<BooleanElement>(lhs, rhs);                                                               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    template<typename Element, Rank rank>                                                                    \
    auto operator symbol(PartitionedArray<Element, rank> const& lhs, hpx::shared_future<Element> const& rhs) \
        ->PartitionedArray<BooleanElement, rank>                                                             \
    {                                                                                                        \
        return name<BooleanElement>(lhs, rhs);                                                               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    template<typename Element, Rank rank>                                                                    \
    auto operator symbol(hpx::shared_future<Element> const& lhs, PartitionedArray<Element, rank> const& rhs) \
        ->PartitionedArray<BooleanElement, rank>                                                             \
    {                                                                                                        \
        return name<BooleanElement>(lhs, rhs);                                                               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    template<typename Element, Rank rank>                                                                    \
    auto operator symbol(PartitionedArray<Element, rank> const& lhs, Element const rhs)                      \
        ->PartitionedArray<BooleanElement, rank>                                                             \
    {                                                                                                        \
        return name<BooleanElement>(lhs, rhs);                                                               \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    template<typename Element, Rank rank>                                                                    \
    auto operator symbol(Element const lhs, PartitionedArray<Element, rank> const& rhs)                      \
        ->PartitionedArray<BooleanElement, rank>                                                             \
    {                                                                                                        \
        return name<BooleanElement>(lhs, rhs);                                                               \
    }
