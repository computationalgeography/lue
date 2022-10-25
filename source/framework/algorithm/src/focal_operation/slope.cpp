#include "lue/framework/algorithm/definition/slope.hpp"
#include "lue/framework/algorithm/value_policies/slope.hpp"


#define LUE_INSTANTIATE(Policies, Element)       \
                                                 \
    LUE_INSTANTIATE_SLOPE(                       \
        ESC(policy::slope::Policies<Element>),   \
        Element);


namespace lue {

    LUE_INSTANTIATE(DefaultValuePolicies, float);
    LUE_INSTANTIATE(DefaultValuePolicies, double);

}  // namespace lue
