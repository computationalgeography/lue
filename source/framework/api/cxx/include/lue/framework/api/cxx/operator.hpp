#pragma once
#include "lue/framework/api/cxx/local/add.hpp"
#include "lue/framework/api/cxx/local/equal_to.hpp"
#include "lue/framework/api/cxx/local/greater_than.hpp"
#include "lue/framework/api/cxx/local/greater_than_equal_to.hpp"
#include "lue/framework/api/cxx/local/less_than.hpp"
#include "lue/framework/api/cxx/local/less_than_equal_to.hpp"
#include "lue/framework/api/cxx/local/logical_and.hpp"
#include "lue/framework/api/cxx/local/logical_inclusive_or.hpp"
#include "lue/framework/api/cxx/local/logical_not.hpp"
#include "lue/framework/api/cxx/local/not_equal_to.hpp"


inline auto operator+(lue::api::Field const& lhs, lue::api::Field const& rhs)
{
    return lue::api::add(lhs, rhs);
}


inline auto operator==(lue::api::Field const& lhs, lue::api::Field const& rhs)
{
    return lue::api::equal_to(lhs, rhs);
}


inline auto operator>(lue::api::Field const& lhs, lue::api::Field const& rhs)
{
    return lue::api::greater_than(lhs, rhs);
}


inline auto operator>=(lue::api::Field const& lhs, lue::api::Field const& rhs)
{
    return lue::api::greater_than_equal_to(lhs, rhs);
}


inline auto operator<(lue::api::Field const& lhs, lue::api::Field const& rhs)
{
    return lue::api::less_than(lhs, rhs);
}


inline auto operator<=(lue::api::Field const& lhs, lue::api::Field const& rhs)
{
    return lue::api::less_than_equal_to(lhs, rhs);
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


inline auto operator!=(lue::api::Field const& lhs, lue::api::Field const& rhs)
{
    return lue::api::not_equal_to(lhs, rhs);
}
