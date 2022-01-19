#include "lue/framework/algorithm/definition/slope.hpp"


#define LUE_INSTANTIATE(Element)                             \
                                                             \
    LUE_INSTANTIATE_SLOPE(                                   \
        ESC(policy::slope::DefaultValuePolicies<Element>),   \
        Element);


namespace lue {

    LUE_INSTANTIATE(float);
    LUE_INSTANTIATE(double);

}  // namespace lue
