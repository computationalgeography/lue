#pragma once
#include "lue/framework/core/define.hpp"
#include <array>


namespace lue {

    template<typename Index, Rank rank>
    using Indices = std::array<Index, rank>;

}  // namespace lue
