#pragma once
#include "lue/framework/api/c/future.h"
#include "lue/framework/api/cxx/future.hpp"
#include <cassert>


inline auto as_cxx_future_ptr(Future const* future) -> lue::api::Future const*
{
    assert(future);
    assert(future->instance);

    return static_cast<lue::api::Future const*>(future->instance);
}


inline auto as_cxx_future_ptr(Future* future) -> lue::api::Future*
{
    assert(future);
    assert(future->instance);

    return static_cast<lue::api::Future*>(future->instance);
}


inline auto as_cxx_future(Future const* future) -> lue::api::Future const&
{
    return *as_cxx_future_ptr(future);
}


inline auto as_cxx_future(Future* future) -> lue::api::Future&
{
    return *as_cxx_future_ptr(future);
}
