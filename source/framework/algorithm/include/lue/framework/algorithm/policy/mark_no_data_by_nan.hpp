#pragma once
#include "lue/framework/algorithm/policy/detect_no_data_by_nan.hpp"


namespace lue::policy {

    /*!
        @brief      Output no-data policy
        @sa         DetectNoDataByNaN, MarkNoDataByValue

        Policy to use when @a Element is a floating point type and NaN is used to represent no-data values.
    */
    template<typename Element>
    class MarkNoDataByNaN: public DetectNoDataByNaN<Element>
    {

        public:

            static_assert(std::is_floating_point_v<Element>);


            /*!
                @brief      Mark @a value as no-data
            */
            void mark_no_data(Element& value) const
            {
                value = std::numeric_limits<Element>::quiet_NaN();
            }


            /*!
                @brief      Mark the value at @a idx in @a data as no-data
            */
            template<typename Data>
            void mark_no_data(Data& data, Index const idx) const
            {
                static_assert(std::is_same_v<lue::ElementT<Data>, Element>);

                data[idx] = std::numeric_limits<Element>::quiet_NaN();
            }


            /*!
                @brief      Mark the value at @a idx in @a data as no-data
            */
            template<typename Data, typename... Idxs>
            void mark_no_data(Data& data, Idxs const... idxs) const
            {
                static_assert(std::is_same_v<lue::ElementT<Data>, Element>);

                data(idxs...) = std::numeric_limits<Element>::quiet_NaN();
            }
    };

}  // namespace lue::policy
