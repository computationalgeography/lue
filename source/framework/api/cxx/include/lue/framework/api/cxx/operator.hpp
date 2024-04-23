#pragma once
#include "lue/framework/api/cxx/local/add.hpp"


namespace lue {

    inline auto operator+(Field const& lhs, Field const& rhs)
    {
        return add(lhs, rhs);
    }

}  // namespace lue
