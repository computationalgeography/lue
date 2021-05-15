#include "lue/framework/algorithm/definition/log.hpp"


#define LUE_INSTANTIATE(Element, rank)                  \
                                                        \
    LUE_INSTANTIATE_LOG(                                \
        (policy::log::DefaultValuePolicies<Element>),  \
        Element,                                        \
        rank);                                          \


namespace lue {

    LUE_INSTANTIATE(float, 2);
    LUE_INSTANTIATE(double, 2);

}  // namespace lue
