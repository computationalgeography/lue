#pragma once
#include "lue/framework/algorithm/scalar.hpp"
#include "lue/framework/partitioned_array.hpp"

#include <cstdint>
#include <variant>


namespace lue::api {

    /*!
        @brief      A variant of all types that can be used to represent continuous fields
    */
    using Field = std::variant<
        PartitionedArray<std::uint8_t, 2>,
        PartitionedArray<std::int32_t, 2>,
        PartitionedArray<std::uint32_t, 2>,
        PartitionedArray<std::int64_t, 2>,
        PartitionedArray<std::uint64_t, 2>,
        PartitionedArray<float, 2>,
        PartitionedArray<double, 2>,
        lue::Scalar<std::uint8_t>,
        lue::Scalar<std::int32_t>,
        lue::Scalar<std::uint32_t>,
        lue::Scalar<std::int64_t>,
        lue::Scalar<std::uint64_t>,
        lue::Scalar<float>,
        lue::Scalar<double>,
        std::uint8_t,
        std::int32_t,
        std::uint32_t,
        std::int64_t,
        std::uint64_t,
        float,
        double>;

}  // namespace lue::api
