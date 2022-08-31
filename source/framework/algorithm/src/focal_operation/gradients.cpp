#include "lue/framework/algorithm/default_policies/gradients.hpp"
#include "lue/framework/algorithm/definition/gradients.hpp"
#include "lue/framework/algorithm/value_policies/gradients.hpp"


#define LUE_INSTANTIATE(Element)                                \
                                                                \
    LUE_INSTANTIATE_GRADIENTS(                                  \
        ESC(policy::gradients::DefaultPolicies<Element>),       \
        Element);                                               \
                                                                \
    LUE_INSTANTIATE_GRADIENTS(                                  \
        ESC(policy::gradients::DefaultValuePolicies<Element>),  \
        Element);


namespace lue {

    LUE_INSTANTIATE(float);
    LUE_INSTANTIATE(double);

}  // namespace lue
