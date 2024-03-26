#pragma once
#include "lue/framework/core/define.hpp"
#include <mdspan/mdspan.hpp>


namespace lue {

    template<Rank rank>
    using DynamicExtents = Kokkos::dextents<Index, rank>;

    template<typename Element, Rank rank>
    using DynamicSpan = Kokkos::mdspan<Element, DynamicExtents<rank>, Kokkos::layout_right>;

    template<typename Element, Rank rank>
    using DynamicSubspan = Kokkos::mdspan<Element, DynamicExtents<rank>, Kokkos::layout_stride>;

    using Kokkos::submdspan;

}  // namespace lue
