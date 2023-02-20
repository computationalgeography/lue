#include "lue/framework/algorithm/definition/aspect.hpp"
#include "lue/framework/algorithm/value_policies/aspect.hpp"


#define LUE_INSTANTIATE(Policies, Element)                                                                   \
                                                                                                             \
    LUE_INSTANTIATE_ASPECT(ESC(policy::aspect::Policies<Element>), Element);


namespace lue {

    LUE_INSTANTIATE(DefaultValuePolicies, float);
    LUE_INSTANTIATE(DefaultValuePolicies, double);

}  // namespace lue
