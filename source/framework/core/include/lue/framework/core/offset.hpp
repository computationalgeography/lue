#pragma once
#include <array>
#include "lue/framework/core/define.hpp"  // Rank


namespace lue {

    template<typename Index, Rank rank>
    using Offset = std::array<Index, rank>;

}  // namespace lue
