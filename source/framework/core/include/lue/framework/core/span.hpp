#pragma once
#include "lue/framework/core/define.hpp"
#include <experimental/mdspan>


namespace lue {

    template<Rank rank>
    using DynamicExtents = std::experimental::dextents<Index, rank>;

    template<typename Element, Rank rank>
    using DynamicSpan =
        std::experimental::mdspan<Element, DynamicExtents<rank>, std::experimental::layout_right>;

    template<typename Element, Rank rank>
    using DynamicSubspan =
        std::experimental::mdspan<Element, DynamicExtents<rank>, std::experimental::layout_stride>;

    using std::experimental::submdspan;

}  // namespace lue
