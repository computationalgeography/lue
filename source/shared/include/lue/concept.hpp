#pragma once
#include <type_traits>


namespace lue {

    template<typename T>
    concept Arithmetic = std::is_arithmetic_v<T>;

}  // namespace lue
