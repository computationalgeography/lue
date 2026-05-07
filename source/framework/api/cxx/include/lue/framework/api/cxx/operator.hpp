#pragma once
#include "lue/framework/api/cxx/local/add.hpp"
#include "lue/framework/api/cxx/local/logical_and.hpp"
#include "lue/framework/api/cxx/local/logical_inclusive_or.hpp"
#include "lue/framework/api/cxx/local/logical_not.hpp"


inline auto operator+(lue::api::Field const& lhs, lue::api::Field const& rhs)
{
    return lue::api::add(lhs, rhs);
}


inline auto operator&&(lue::api::Field const& lhs, lue::api::Field const& rhs)
{
    return lue::api::logical_and(lhs, rhs);
}


inline auto operator||(lue::api::Field const& lhs, lue::api::Field const& rhs)
{
    return lue::api::logical_inclusive_or(lhs, rhs);
}


inline auto operator!(lue::api::Field const& argument)
{
    return lue::api::logical_not(argument);
}
