#pragma once
#include "lue/framework/core/type_traits.hpp"
#include <cmath>
#include <limits>
#include <type_traits>


namespace lue::policy {

    /*!
        @brief      Input no-data policy
        @sa         MarkNoDataByNaN, DetectNoDataByValue

        Policy to use when @a Element is a floating point type and NaN is used to represent no-data values.
    */
    template<typename Element>
    class DetectNoDataByNaN
    {

        public:

            static_assert(std::is_floating_point_v<Element>);

            /*!
                @brief      The no-data value to use in tests: std::numeric_limits<Element>::quiet_NaN()
            */
            static constexpr Element no_data_value{std::numeric_limits<Element>::quiet_NaN()};


            /*!
                @brief      Return whether @a value is no-data
            */
            auto is_no_data(Element const& value) const -> bool
            {
                return std::isnan(value);
            }


            /*!
                @brief      Return whether the element at index @a idx in @a data contains no-data
                @tparam     Data Collection of elements
            */
            template<typename Data>
            auto is_no_data(Data const& data, Index const idx) const -> bool
            {
                static_assert(std::is_same_v<lue::ElementT<Data>, Element>);

                return std::isnan(data[idx]);
            }


            /*!
                @brief      Return whether the element at index @a idx in @a data contains no-data
                @tparam     Data Collection of elements
            */
            template<typename Data, typename... Idxs>
            auto is_no_data(Data const& data, Idxs const... idxs) const -> bool
            {
                static_assert(std::is_same_v<lue::ElementT<Data>, Element>);

                return std::isnan(data(idxs...));
            }
    };

}  // namespace lue::policy
