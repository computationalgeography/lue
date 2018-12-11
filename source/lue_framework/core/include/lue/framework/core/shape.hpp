#pragma once
#include <array>


namespace lue {

template<
    typename T,
    std::size_t rank>
using Shape = std::array<T, rank>;

}  // namespace lue
