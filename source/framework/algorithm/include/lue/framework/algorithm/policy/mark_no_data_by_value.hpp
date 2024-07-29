#pragma once
#include "lue/framework/algorithm/policy/detect_no_data_by_value.hpp"


namespace lue::policy {

    /*!
        @brief      Output no-data policy
        @sa         DetectNoDataByValue, MarkNoDataByNaN

        Policy suitable for those cases where no-data is represented by some "special" value, like a minimum
        value, maximum value, or a very small value.
    */
    template<typename Element>
    class MarkNoDataByValue: public DetectNoDataByValue<Element>
    {

        public:

            /*!
                @brief      Construct an instance, using the default DetectNoDataByValue::no_data_value as
                            the no-data value
            */
            MarkNoDataByValue():

                MarkNoDataByValue{DetectNoDataByValue<Element>::no_data_value}

            {
            }


            /*!
                @brief      Construct an instance, using @a value as the no-data value
            */
            MarkNoDataByValue(Element const value):

                DetectNoDataByValue<Element>{value}

            {
            }


            /*!
                @brief      Mark @a value as no-data
            */
            void mark_no_data(Element& value) const
            {
                value = this->value();
            }


            /*!
                @brief      Mark the value at @a idx in @a data as no-data
            */
            template<typename Data>
            void mark_no_data(Data& data, Index const idx) const
            {
                static_assert(std::is_same_v<lue::ElementT<Data>, Element>);

                data[idx] = this->value();
            }


            /*!
                @brief      Mark the value at @a idx in @a data as no-data
            */
            template<typename Data, typename... Idxs>
            void mark_no_data(Data& data, Idxs const... idxs) const
            {
                static_assert(std::is_same_v<lue::ElementT<Data>, Element>);

                data(idxs...) = this->value();
            }
    };

}  // namespace lue::policy
