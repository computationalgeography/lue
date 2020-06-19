#pragma once


#define LUE_BINARY_ARITHMETIC_OPERATOR(symbol, name)     \
template<                                                \
    typename Element,                                    \
    Rank rank>                                           \
PartitionedArray<Element, rank> operator symbol(         \
    PartitionedArray<Element, rank> const& lhs,          \
    PartitionedArray<Element, rank> const& rhs)          \
{                                                        \
    return name(lhs, rhs);                               \
}                                                        \
                                                         \
                                                         \
template<                                                \
    typename Element,                                    \
    Rank rank>                                           \
PartitionedArray<Element, rank> operator symbol(         \
    PartitionedArray<Element, rank> const& lhs,          \
    hpx::shared_future<Element> const& rhs)              \
{                                                        \
    return name(lhs, rhs);                               \
}                                                        \
                                                         \
                                                         \
template<                                                \
    typename Element,                                    \
    Rank rank>                                           \
PartitionedArray<Element, rank> operator symbol(         \
    hpx::shared_future<Element> const& lhs,              \
    PartitionedArray<Element, rank> const& rhs)          \
{                                                        \
    return name(lhs, rhs);                               \
}                                                        \
                                                         \
                                                         \
template<                                                \
    typename Element,                                    \
    Rank rank>                                           \
PartitionedArray<Element, rank> operator symbol(         \
    PartitionedArray<Element, rank> const& lhs,          \
    Element const rhs)                                   \
{                                                        \
    return name(lhs, rhs);                               \
}                                                        \
                                                         \
                                                         \
template<                                                \
    typename Element,                                    \
    Rank rank>                                           \
PartitionedArray<Element, rank> operator symbol(         \
    Element const lhs,                                   \
    PartitionedArray<Element, rank> const& rhs)          \
{                                                        \
    return name(lhs, rhs);                               \
}


#define LUE_BINARY_COMPARISON_OPERATOR(symbol, name)     \
template<                                                \
    typename Element,                                    \
    Rank rank>                                           \
PartitionedArray<bool, rank> operator symbol(            \
    PartitionedArray<Element, rank> const& lhs,          \
    PartitionedArray<Element, rank> const& rhs)          \
{                                                        \
    return name(lhs, rhs);                               \
}                                                        \
                                                         \
                                                         \
template<                                                \
    typename Element,                                    \
    Rank rank>                                           \
PartitionedArray<bool, rank> operator symbol(            \
    PartitionedArray<Element, rank> const& lhs,          \
    hpx::shared_future<Element> const& rhs)              \
{                                                        \
    return name(lhs, rhs);                               \
}                                                        \
                                                         \
                                                         \
template<                                                \
    typename Element,                                    \
    Rank rank>                                           \
PartitionedArray<bool, rank> operator symbol(            \
    hpx::shared_future<Element> const& lhs,              \
    PartitionedArray<Element, rank> const& rhs)          \
{                                                        \
    return name(lhs, rhs);                               \
}                                                        \
                                                         \
                                                         \
template<                                                \
    typename Element,                                    \
    Rank rank>                                           \
PartitionedArray<bool, rank> operator symbol(            \
    PartitionedArray<Element, rank> const& lhs,          \
    Element const rhs)                                   \
{                                                        \
    return name(lhs, rhs);                               \
}                                                        \
                                                         \
                                                         \
template<                                                \
    typename Element,                                    \
    Rank rank>                                           \
PartitionedArray<bool, rank> operator symbol(            \
    Element const lhs,                                   \
    PartitionedArray<Element, rank> const& rhs)          \
{                                                        \
    return name(lhs, rhs);                               \
}


#define LUE_UNARY_LOGICAL_OPERATOR(symbol, name)         \
template<                                                \
    typename Element,                                    \
    Rank rank>                                           \
PartitionedArray<bool, rank> operator symbol(            \
    PartitionedArray<Element, rank> const& array)        \
{                                                        \
    return name(array);                                  \
}


#define LUE_BINARY_LOGICAL_OPERATOR(symbol, name)        \
template<                                                \
    typename Element,                                    \
    Rank rank>                                           \
PartitionedArray<bool, rank> operator symbol(            \
    PartitionedArray<Element, rank> const& lhs,          \
    PartitionedArray<Element, rank> const& rhs)          \
{                                                        \
    return name(lhs, rhs);                               \
}                                                        \
                                                         \
                                                         \
template<                                                \
    typename Element,                                    \
    Rank rank>                                           \
PartitionedArray<bool, rank> operator symbol(            \
    PartitionedArray<Element, rank> const& lhs,          \
    hpx::shared_future<Element> const& rhs)              \
{                                                        \
    return name(lhs, rhs);                               \
}                                                        \
                                                         \
                                                         \
template<                                                \
    typename Element,                                    \
    Rank rank>                                           \
PartitionedArray<bool, rank> operator symbol(            \
    hpx::shared_future<Element> const& lhs,              \
    PartitionedArray<Element, rank> const& rhs)          \
{                                                        \
    return name(lhs, rhs);                               \
}                                                        \
                                                         \
                                                         \
template<                                                \
    typename Element,                                    \
    Rank rank>                                           \
PartitionedArray<bool, rank> operator symbol(            \
    PartitionedArray<Element, rank> const& lhs,          \
    Element const rhs)                                   \
{                                                        \
    return name(lhs, rhs);                               \
}                                                        \
                                                         \
                                                         \
template<                                                \
    typename Element,                                    \
    Rank rank>                                           \
PartitionedArray<bool, rank> operator symbol(            \
    Element const lhs,                                   \
    PartitionedArray<Element, rank> const& rhs)          \
{                                                        \
    return name(lhs, rhs);                               \
}
