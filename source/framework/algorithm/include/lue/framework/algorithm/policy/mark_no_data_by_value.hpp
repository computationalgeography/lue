#pragma once
#include "lue/framework/algorithm/policy/detect_no_data_by_value.hpp"


namespace lue {
namespace policy {

template<
    typename Element>
class MarkNoDataByValue:
    public DetectNoDataByValue<Element>
{

    public:

        MarkNoDataByValue()=default;

        MarkNoDataByValue(
                Element const value):

            DetectNoDataByValue<Element>{value}

        {
        }

        void mark_no_data(
            Element& element) const
        {
            element = this->value();
        }

        template<
            typename Data,
            typename... Idxs>
        void mark_no_data(
            Data& data,
            Idxs const...idxs) const
        {
            static_assert(std::is_same_v<ElementT<Data>, Element>);

            data(idxs...) = this->value();
        }

};

}  // namespace policy
}  // namespace lue
