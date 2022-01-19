#include "lue/framework/algorithm/definition/atan.hpp"


#define LUE_INSTANTIATE(Element)                           \
                                                           \
    LUE_INSTANTIATE_ATAN(                                  \
        ESC(policy::atan::DefaultValuePolicies<Element>),  \
        Element,                                           \
        2);


namespace lue {

    LUE_INSTANTIATE(float);
    LUE_INSTANTIATE(double);

}  // namespace lue
