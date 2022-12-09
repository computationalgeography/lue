#pragma once
#include "lue/framework/algorithm/policy/detect_no_data_by_nan.hpp"


namespace lue::policy {

    template<
        typename Element>
    class MarkNoDataByNaN:
        public DetectNoDataByNaN<Element>
    {

        public:

            static_assert(std::is_floating_point_v<Element>);


            MarkNoDataByNaN():

                DetectNoDataByNaN<Element>{}

            {
            }


            void mark_no_data(
                Element& element) const
            {
                element = std::numeric_limits<Element>::quiet_NaN();
            }


            template<
                typename Data>
            void mark_no_data(
                Data& data,
                Index const idx) const
            {
                static_assert(std::is_same_v<lue::ElementT<Data>, Element>);

                data[idx] = std::numeric_limits<Element>::quiet_NaN();
            }


            template<
                typename Data,
                typename... Idxs>
            void mark_no_data(
                Data& data,
                Idxs const...idxs) const
            {
                static_assert(std::is_same_v<lue::ElementT<Data>, Element>);

                data(idxs...) = std::numeric_limits<Element>::quiet_NaN();
            }

    };

}  // namespace lue::policy
