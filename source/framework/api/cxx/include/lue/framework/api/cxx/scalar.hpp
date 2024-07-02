#pragma once
#include "lue/framework/algorithm/scalar.hpp"

#include <cstdint>
#include <variant>


namespace lue::api {

    /*!
        @brief      A variant of all types that can be used to represent scalars
    */
    using Scalar = std::variant<
        Scalar<std::uint8_t>,
        Scalar<std::int32_t>,
        Scalar<std::uint32_t>,
        Scalar<std::int64_t>,
        Scalar<std::uint64_t>,
        Scalar<float>,
        Scalar<double>,
        std::uint8_t,
        std::int32_t,
        std::uint32_t,
        std::int64_t,
        std::uint64_t,
        float,
        double>;

}  // namespace lue::api
