#include "lue/framework/algorithm/default_policies/sqrt.hpp"
#include "lue/framework/algorithm/definition/sqrt.hpp"
#include "lue/framework/algorithm/value_policies/sqrt.hpp"


#define LUE_INSTANTIATE(Element, rank)                     \
                                                           \
    LUE_INSTANTIATE_SQRT(                                  \
        ESC(policy::sqrt::DefaultPolicies<Element>),       \
        Element,                                           \
        rank);                                             \
                                                           \
    LUE_INSTANTIATE_SQRT(                                  \
        ESC(policy::sqrt::DefaultValuePolicies<Element>),  \
        Element,                                           \
        rank);


namespace lue {

    LUE_INSTANTIATE(float, 2);
    LUE_INSTANTIATE(double, 2);

}  // namespace lue
