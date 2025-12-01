#pragma once
#include <tuple>
#include <type_traits>


namespace lue {

    template<typename T>
    concept Arithmetic = std::is_arithmetic_v<T>;


    template<typename T>
    concept TupleLike = requires { std::tuple_size_v<T>; };

}  // namespace lue
