#pragma once
#include <hpx/lcos/future.hpp>
#include <fmt/format.h>
#include <string>


namespace lue {

hpx::future<std::string> system_description();

void               write_debug_message (std::string const& message);

template<
    typename Class>
std::string        describe            (Class const& instance);

}  // namespace lue


#define LUE_ASSERT(                                     \
        test,                                           \
        ...)                                            \
    if(!(test)) {                                       \
        write_debug_message(fmt::format(__VA_ARGS__));  \
    }                                                   \
    assert(test);
