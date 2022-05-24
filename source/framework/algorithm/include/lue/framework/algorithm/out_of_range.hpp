#pragma once
#include <algorithm>
#include <cmath>
#ifndef NDEBUG
#include <limits>
#endif
#include <type_traits>


namespace lue {

    /*!
        @brief      Return whether the result of adding @a argument1 and @a argument2 resulted
                    in an out-of-range @a result value
    */
    template<
        typename T>
    bool add_out_of_range(
        T const& argument1,
        T const& argument2,
        T const& result) noexcept
    {
        static_assert(std::is_arithmetic_v<T>);

        if constexpr (std::is_integral_v<T>)
        {
            if constexpr (std::is_unsigned_v<T>)
            {
                // Unsigned integer

#ifndef NDEBUG
                static constexpr T max{std::numeric_limits<T>::max()};
                static_assert(max + 1 == 0);
#endif

                // Overflow if result is smaller than one of the arguments
                return result < argument1;  // std::max(argument1, argument2);
            }
            else
            {
                // Signed integer

#ifndef NDEBUG
                // static constexpr T min{std::numeric_limits<T>::min()};
                // static constexpr T max{std::numeric_limits<T>::max()};
                // TODO Why are these expressions not constant conditions while the one for
                //      unsigned integers is???
                // static_assert(min - 1 == max);
                // static_assert(max + 1 == min);
#endif

                // Overflow/underflow if sign of result is different
                return argument2 > 0
                    ? (result < argument1)
                    : (result > argument1);
            }
        }
        else
        {
            // Floating point

            return std::isinf(result);
        }
    }


    /*!
        @brief      Return whether the result of adding @a argument1 and @a argument2 resulted
                    in a within-range @a result value
    */
    template<
        typename T>
    bool add_within_range(
        T const& argument1,
        T const& argument2,
        T const& result) noexcept
    {
        return !add_out_of_range(argument1, argument2, result);
    }


    /*!
        @brief      Return whether the result of multiplying @a argument1 and @a argument2 resulted
                    in an out-of-range @a result value
    */
    template<
        typename T>
    bool multiply_out_of_range(
        T const& argument1,
        T const& argument2,
        T const& result) noexcept
    {
        static_assert(std::is_arithmetic_v<T>);

        if constexpr (std::is_integral_v<T>)
        {
            return argument2 == T{0} ? false : result / argument2 != argument1;
        }
        else
        {
            return std::isinf(result);
        }
    }


    /*!
        @brief      Return whether the result of multiplying @a argument1 and @a argument2 resulted
                    in a within-range @a result value
    */
    template<
        typename T>
    bool multiply_within_range(
        T const& argument1,
        T const& argument2,
        T const& result) noexcept
    {
        return !multiply_out_of_range(argument1, argument2, result);
    }

}  // namespace lue
