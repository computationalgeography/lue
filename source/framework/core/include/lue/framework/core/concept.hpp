#pragma once
#include "lue/framework/core/shape.hpp"
#include <concepts>


namespace lue {

    template<typename T>
    concept ShapeLike = requires(T instance) {
        // { rank<T> } -> std::integral;
        { is_hypercube(instance) } -> std::same_as<bool>;
    };

}  // namespace lue
