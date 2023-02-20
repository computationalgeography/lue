#include "lue/framework/algorithm/definition/gradients.hpp"
#include "lue/framework/algorithm/value_policies/gradients.hpp"


#define LUE_INSTANTIATE(Policies, Element)                                                                   \
                                                                                                             \
    LUE_INSTANTIATE_GRADIENTS(ESC(policy::gradients::Policies<Element>), Element);


namespace lue {

    LUE_INSTANTIATE(DefaultValuePolicies, float);
    LUE_INSTANTIATE(DefaultValuePolicies, double);

}  // namespace lue
