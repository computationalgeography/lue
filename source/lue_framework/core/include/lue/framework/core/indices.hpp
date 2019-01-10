#pragma once
#include <array>


namespace lue {

template<
    typename Index,
    std::size_t rank>
using Indices = std::array<Index, rank>;

}  // namespace lue
