#pragma once
#include "lue/framework/algorithm/scalar.hpp"
#include "lue/framework/api/cxx/variant_wrapper.hpp"
#include "lue/framework/partitioned_array.hpp"
#include <cstdint>


namespace lue::api {

    /*!
        @brief      Class for representing continuous fields of information
    */
    class Field:
        public VariantWrapper<
            PartitionedArray<std::int8_t, 2>,
            PartitionedArray<std::uint8_t, 2>,
            PartitionedArray<std::int32_t, 2>,
            PartitionedArray<std::uint32_t, 2>,
            PartitionedArray<std::int64_t, 2>,
            PartitionedArray<std::uint64_t, 2>,
            PartitionedArray<float, 2>,
            PartitionedArray<double, 2>,
            lue::Scalar<std::int8_t>,
            lue::Scalar<std::uint8_t>,
            lue::Scalar<std::int32_t>,
            lue::Scalar<std::uint32_t>,
            lue::Scalar<std::int64_t>,
            lue::Scalar<std::uint64_t>,
            lue::Scalar<float>,
            lue::Scalar<double>,
            std::int8_t,
            std::uint8_t,
            std::int32_t,
            std::uint32_t,
            std::int64_t,
            std::uint64_t,
            float,
            double>

    {

        public:

            using VariantWrapper::VariantWrapper;
    };

}  // namespace lue::api
