#include "lue/framework/algorithm/definition/atan2.hpp"


#define LUE_INSTANTIATE(Element, rank)                   \
                                                         \
    LUE_INSTANTIATE_BINARY_LOCAL_OPERATION(              \
        (policy::atan2::DefaultValuePolicies<Element>),  \
        Element,                                         \
        Element,                                         \
        Element,                                         \
        rank,                                            \
        (detail::ATan2<Element>));


namespace lue {

    LUE_INSTANTIATE(float, 2);
    LUE_INSTANTIATE(double, 2);

}  // namespace lue
