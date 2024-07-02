#pragma once
#include <cstdint>
#include <variant>


namespace lue::api {

    /*!
        @brief      A variant of all types that can be used to represent literals
    */
    using Literal = std::variant<
        std::uint8_t,
        std::int32_t,
        std::uint32_t,
        std::int64_t,
        std::uint64_t,
        float,
        double>;

}  // namespace lue::api
