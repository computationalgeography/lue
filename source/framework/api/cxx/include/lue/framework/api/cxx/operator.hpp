#pragma once
#include "lue/framework/api/cxx/local/add.hpp"


inline auto operator+(lue::api::Field const& lhs, lue::api::Field const& rhs)
{
    return lue::api::add(lhs, rhs);
}
