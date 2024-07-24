#pragma once
#include "lue/framework/api/cxx/variant_wrapper.hpp"
#include <cstdint>


namespace lue::api {

    /*!
        @brief      Class for representing literals
    */
    class Literal:
        public VariantWrapper<
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
