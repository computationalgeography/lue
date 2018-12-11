#pragma once
#include <array>


namespace lue {

template<
    typename T,
    std::size_t rank>
using Indices = std::array<T, rank>;

}  // namespace lue
