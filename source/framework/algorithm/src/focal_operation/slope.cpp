#include "lue/framework/algorithm/default_policies/slope.hpp"
#include "lue/framework/algorithm/definition/slope.hpp"
#include "lue/framework/algorithm/value_policies/slope.hpp"


#define LUE_INSTANTIATE(Element)                             \
                                                             \
    LUE_INSTANTIATE_SLOPE(                                   \
        ESC(policy::slope::DefaultPolicies<Element>),        \
        Element);                                            \
                                                             \
    LUE_INSTANTIATE_SLOPE(                                   \
        ESC(policy::slope::DefaultValuePolicies<Element>),   \
        Element);


namespace lue {

    LUE_INSTANTIATE(float);
    LUE_INSTANTIATE(double);

}  // namespace lue
