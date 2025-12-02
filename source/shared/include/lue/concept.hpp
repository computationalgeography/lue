#pragma once
#include <tuple>
#include <type_traits>


namespace lue {

    template<typename T>
    concept Arithmetic = std::is_arithmetic_v<T>;


    // Can't use std::tuple_size_v as it is not SFINAE-friendly
    template<typename T>
    // NOLINTNEXTLINE(modernize-type-traits)
    concept TupleLike = requires { std::tuple_size<T>::value; };

}  // namespace lue
