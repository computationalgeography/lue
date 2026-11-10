#pragma once
#include "lue/framework/api/c/lookup_table.h"
#include "lue/framework/api/cxx/lookup_table.hpp"
#include <cassert>


inline auto lue_as_cxx_lookup_table_ptr(LUE_LookupTable const* lookup_table) -> lue::api::LookupTable const*
{
    assert(lookup_table);
    assert(lookup_table->instance);

    return static_cast<lue::api::LookupTable const*>(lookup_table->instance);
}


inline auto lue_as_cxx_lookup_table_ptr(LUE_LookupTable* lookup_table) -> lue::api::LookupTable*
{
    assert(lookup_table);
    assert(lookup_table->instance);

    return static_cast<lue::api::LookupTable*>(lookup_table->instance);
}


inline auto lue_as_cxx_lookup_table(LUE_LookupTable const* lookup_table) -> lue::api::LookupTable const&
{
    return *lue_as_cxx_lookup_table_ptr(lookup_table);
}


inline auto lue_as_cxx_lookup_table(LUE_LookupTable* lookup_table) -> lue::api::LookupTable&
{
    return *lue_as_cxx_lookup_table_ptr(lookup_table);
}
