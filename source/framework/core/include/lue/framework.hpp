#pragma once
#include "lue/framework/configure.hpp"
#include <algorithm>


namespace lue {

    inline constexpr auto rank_supported(Rank const rank) -> bool
    {
        return std::find(ranks.begin(), ranks.end(), rank) != ranks.end();
    }


    void verify_rank_supported(Rank rank);

}  // namespace lue
