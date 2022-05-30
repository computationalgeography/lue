#pragma once
#include <boost/safe_numerics/safe_integer.hpp>
#include <cmath>
#include <type_traits>


namespace lue {

    /*!
        @brief      Add two numbers and return whether the result is valid
    */
    template<
        typename T>
    bool safe_add(
        T const& argument1,
        T const& argument2,
        T& result) noexcept
    {
        static_assert(std::is_arithmetic_v<T>);

        if constexpr (std::is_integral_v<T>)
        {
            namespace bsf = boost::safe_numerics;

            bool status{false};

            try
            {
                bsf::safe<T> a1{argument1};
                bsf::safe<T> a2{argument2};
                bsf::safe<T> r{a1 + a2};

                result = r;
                status = true;
            }
            catch([[maybe_unused]] std::exception const& exception)
            {
            }

            return status;
        }
        else
        {
            // Floating point
            result = argument1 + argument2;

            return std::isinf(result);
        }
    }


    template<
        typename T>
    bool add_within_range(
        T const& argument1,
        T const& argument2,
        [[maybe_unused]] T const& result) noexcept
    {
        T r;
        return safe_add(argument1, argument2, r);
    }


    template<
        typename T>
    bool add_out_of_range(
        T const& argument1,
        T const& argument2,
        T const& result) noexcept
    {
        return !add_within_range(argument1, argument2, result);
    }


    /*!
        @brief      Multiply two numbers and return whether the result is valid
    */
    template<
        typename T>
    bool safe_multiply(
        T const& argument1,
        T const& argument2,
        T& result) noexcept
    {
        static_assert(std::is_arithmetic_v<T>);

        if constexpr (std::is_integral_v<T>)
        {
            namespace bsf = boost::safe_numerics;

            bool status{false};

            try
            {
                bsf::safe<T> a1{argument1};
                bsf::safe<T> a2{argument2};
                bsf::safe<T> r{a1 * a2};

                result = r;
                status = true;
            }
            catch([[maybe_unused]] std::exception const& exception)
            {
            }

            return status;
        }
        else
        {
            // Floating point
            result = argument1 * argument2;

            return std::isinf(result);
        }
    }


    template<
        typename T>
    bool multiply_within_range(
        T const& argument1,
        T const& argument2,
        [[maybe_unused]] T const& result) noexcept
    {
        T r;
        return safe_multiply(argument1, argument2, r);
    }


    template<
        typename T>
    bool multiply_out_of_range(
        T const& argument1,
        T const& argument2,
        T const& result) noexcept
    {
        return !multiply_within_range(argument1, argument2, result);
    }

}  // namespace lue
