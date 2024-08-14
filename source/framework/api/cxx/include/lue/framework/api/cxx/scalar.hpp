#pragma once
#include "lue/framework/algorithm/scalar.hpp"
#include "lue/framework/api/cxx/variant_wrapper.hpp"
#include <cstdint>


namespace lue::api {

    /*!
        @brief      Class for representing scalars
    */
    class Scalar:
        public VariantWrapper<
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
            double>

    {

        public:

            using VariantWrapper::VariantWrapper;
    };

}  // namespace lue::api
