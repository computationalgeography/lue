#pragma once
#include <cmath>
#include <limits>
#include <type_traits>


namespace lue {

/*!
    @brief      Return whether @a value passed in represents the special
                no-data value
    @tparam     T Type of value to test. Must be an arithmetic type and
                not bool. Bool only has two states.
    @param      value Value to test
    @return     True or false
*/
template<
    typename T>
bool is_no_data(
    T const& value)
{
    static_assert(std::is_arithmetic_v<T>);

    if constexpr(std::is_integral_v<T> && !std::is_same_v<T, bool>)
    {
        return value == std::numeric_limits<T>::max();
    }
    else
    {
        static_assert(std::numeric_limits<T>::has_quiet_NaN);

        return std::isnan(value);
    }
}


/*!
    @brief      Set @a value passed in to the special no-data value
    @tparam     T Type of value to test. Must be an arithmetic type and
                not bool. Bool only has two states.
    @param      value Value to set
*/
template<
    typename T>
void set_no_data(
    T& value)
{
    static_assert(std::is_arithmetic_v<T>);

    if constexpr(std::is_integral_v<T> && !std::is_same_v<T, bool>)
    {
        value = std::numeric_limits<T>::max();
    }
    else
    {
        static_assert(std::numeric_limits<T>::has_quiet_NaN);

        value = std::numeric_limits<T>::quiet_NaN();
    }
}

}  // namespace lue
