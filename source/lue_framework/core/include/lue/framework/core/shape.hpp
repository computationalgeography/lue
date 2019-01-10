#pragma once
#include <array>


namespace lue {

template<
    typename Index,
    std::size_t rank>
using Shape = std::array<Index, rank>;

}  // namespace lue
