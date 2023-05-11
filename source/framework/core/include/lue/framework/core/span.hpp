#pragma once
#include "lue/framework/core/define.hpp"
#include <boost/predef.h>
#include <experimental/mdspan>


namespace lue {
    namespace detail {

        template<Rank rank>
        using DynamicExtents = std::experimental::dextents<Index, rank>;

    }  // namespace detail


    template<typename Element, Rank rank>
    using DynamicSpan =
        std::experimental::mdspan<Element, detail::DynamicExtents<rank>, std::experimental::layout_right>;

    template<typename Element, Rank rank>
    using DynamicSubspan =
        std::experimental::mdspan<Element, detail::DynamicExtents<rank>, std::experimental::layout_stride>;

    using std::experimental::submdspan;

}  // namespace lue
