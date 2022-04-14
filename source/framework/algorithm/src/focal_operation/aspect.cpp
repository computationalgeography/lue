#include "lue/framework/algorithm/default_policies/aspect.hpp"
#include "lue/framework/algorithm/definition/aspect.hpp"
#include "lue/framework/algorithm/value_policies/aspect.hpp"


#define LUE_INSTANTIATE(Element)                             \
                                                             \
    LUE_INSTANTIATE_ASPECT(                                  \
        ESC(policy::aspect::DefaultPolicies<Element>),       \
        Element);                                            \
                                                             \
    LUE_INSTANTIATE_ASPECT(                                  \
        ESC(policy::aspect::DefaultValuePolicies<Element>),  \
        Element);


namespace lue {

    LUE_INSTANTIATE(float);
    LUE_INSTANTIATE(double);

}  // namespace lue
