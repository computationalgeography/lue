#pragma once
#include "lue/configure.hpp"
#include <fmt/format.h>
#include <stdexcept>


// In the case of crashes during non-Debug builds, turn on assertion
// checking and see whether one of the assertions ends up being false


namespace lue {
namespace detail {

inline bool empty(
    char const* string)
{
    return std::strlen(string) == 0;
}


template<
    typename String>
bool empty(
    String const& string)
{
    return string.empty();
}

}  // namespace detail


template<
    typename Condition,
    typename String,
    typename... Arguments>
void _lue_assert(
    [[maybe_unused]] char const* filename,
    [[maybe_unused]] int const line_nr,
    [[maybe_unused]] Condition const& condition,
    [[maybe_unused]] char const* condition_as_string,
    [[maybe_unused]] String const& format_string,
    [[maybe_unused]] Arguments&&... arguments)
{
    if constexpr(BuildOptions::assert_conditions)
    {
        if(!condition) {
            if(detail::empty(format_string))
            {
                throw std::logic_error(fmt::format(
                    "{}:{}: assertion failed: {}",
                    filename, line_nr,
                    condition_as_string));
            }
            else
            {
                throw std::logic_error(fmt::format(
                    "{}:{}: assertion failed: {}: {}",
                    filename, line_nr,
                    condition_as_string,
                    fmt::format(format_string, arguments...)));
            }
        }
    }
}


template<
    typename Condition>
void _lue_assert(
    char const* filename,
    int const line_nr,
    Condition const& condition,
    char const* condition_as_string)
{
    _lue_assert(filename, line_nr, condition, condition_as_string, "");
}

}  // namespace lue


// https://en.wikipedia.org/wiki/Variadic_macro
// FIXME This is not portable, update when using C++20
#define lue_assert(condition, ...)  \
_lue_assert(                   \
    __FILE__, __LINE__, condition, #condition, ##__VA_ARGS__);
