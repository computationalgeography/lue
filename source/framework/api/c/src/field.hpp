#pragma once
#include "lue/framework/api/c/field.h"
#include "lue/framework/api/cxx/field.hpp"
#include <cassert>


inline auto as_cxx_field_ptr(Field const* field) -> lue::api::Field const*
{
    assert(field);
    assert(field->instance);

    return static_cast<lue::api::Field const*>(field->instance);
}


inline auto as_cxx_field_ptr(Field* field) -> lue::api::Field*
{
    assert(field);
    assert(field->instance);

    return static_cast<lue::api::Field*>(field->instance);
}


inline auto as_cxx_field(Field const* field) -> lue::api::Field const&
{
    return *as_cxx_field_ptr(field);
}


inline auto as_cxx_field(Field* field) -> lue::api::Field&
{
    return *as_cxx_field_ptr(field);
}
