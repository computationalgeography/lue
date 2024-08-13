#pragma once
#include "lue/framework/core/define.hpp"  // Rank
#include <array>


namespace lue {

    template<typename Index, Rank rank>
    using Offset = std::array<Index, rank>;

}  // namespace lue
