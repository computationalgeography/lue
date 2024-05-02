#pragma once
#include "lue/framework/algorithm/scalar.hpp"
#include "lue/framework/partitioned_array.hpp"

#include <cstdlib>
#include <variant>


namespace lue {

    namespace detail {

        template<typename T>
        using Ptr = std::shared_ptr<T>;

    }  // namespace detail


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

}  // namespace lue
