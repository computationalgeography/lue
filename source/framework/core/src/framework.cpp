#include "lue/framework.hpp"
#include <fmt/ranges.h>
#include <stdexcept>


namespace lue {

    /*!
        @brief      Verify whether LUE framework was built with support for arrays with the rank passed in
        @exception  std::runtime_error In case support for arrays with the rank is not built in
    */
    void verify_rank_supported(Rank const rank)
    {
        if (!rank_supported(rank))
        {
            throw std::runtime_error(fmt::format(
                "Unsupported rank: {}. This version of LUE is built with support for arrays with rank(s): {}",
                rank,
                fmt::join(ranks, ", ")));
        }
    }

}  // namespace lue
