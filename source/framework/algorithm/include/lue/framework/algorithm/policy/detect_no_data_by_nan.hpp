#pragma once
#include "lue/framework/core/type_traits.hpp"
#include <cmath>
#include <limits>
#include <type_traits>


namespace lue::policy {

    template<
        typename Element>
    class DetectNoDataByNaN
    {

        public:

            static_assert(std::is_floating_point_v<Element>);

            static constexpr Element no_data_value{std::numeric_limits<Element>::quiet_NaN()};


            DetectNoDataByNaN()
            {
            }


            bool is_no_data(
                Element const& element) const
            {
                return std::isnan(element);
            }


            template<
                typename Data>
            bool is_no_data(
                Data const& data,
                Index const idx) const
            {
                static_assert(std::is_same_v<lue::ElementT<Data>, Element>);

                return std::isnan(data[idx]);
            }


            template<
                typename Data,
                typename... Idxs>
            bool is_no_data(
                Data const& data,
                Idxs const... idxs) const
            {
                static_assert(std::is_same_v<lue::ElementT<Data>, Element>);

                return std::isnan(data(idxs...));
            }

    };

}  // namespace lue::policy
