#pragma once
#include "lue/framework/core/define.hpp"
#include <experimental/mdspan>


namespace lue {
namespace detail {

// -----------------------------------------------------------------------------
// TODO(KDJ) How to replace this by generic code for translating a rank to
//      a extents<dynamic_extent...>
template<
    Rank rank>
class RankToDynamicExtents;


template<>
class RankToDynamicExtents<1>
{

public:

    using Type = std::experimental::extents<
            std::experimental::dynamic_extent
        >;

};


template<>
class RankToDynamicExtents<2>
{

public:

    using Type = std::experimental::extents<
            std::experimental::dynamic_extent,
            std::experimental::dynamic_extent
        >;

};


template<>
class RankToDynamicExtents<3>
{

public:

    using Type = std::experimental::extents<
            std::experimental::dynamic_extent,
            std::experimental::dynamic_extent,
            std::experimental::dynamic_extent
        >;

};

template<
    Rank rank>
using DynamicExtents = typename RankToDynamicExtents<rank>::Type;


// -----------------------------------------------------------------------------

} // namespace detail


template<
    typename Element,
    Rank rank>
using DynamicSpan =
    std::experimental::basic_mdspan<
            Element,
            detail::DynamicExtents<rank>,
            std::experimental::layout_right
        >;

template<
    typename Element,
    Rank rank>
using DynamicSubspan =
    std::experimental::basic_mdspan<
            Element,
            detail::DynamicExtents<rank>,
            std::experimental::layout_stride<-1, 1>
        >;

using std::experimental::subspan;

} // namespace lue
