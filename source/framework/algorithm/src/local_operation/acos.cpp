#include "lue/framework/algorithm/default_policies/acos.hpp"
#include "lue/framework/algorithm/definition/acos.hpp"
#include "lue/framework/algorithm/value_policies/acos.hpp"


#define LUE_INSTANTIATE(Element, rank)                     \
                                                           \
    LUE_INSTANTIATE_ACOS(                                  \
        ESC(policy::acos::DefaultPolicies<Element>),       \
        Element,                                           \
        rank);                                             \
                                                           \
    LUE_INSTANTIATE_ACOS(                                  \
        ESC(policy::acos::DefaultValuePolicies<Element>),  \
        Element,                                           \
        rank);


namespace lue {

    LUE_INSTANTIATE(float, 2);
    LUE_INSTANTIATE(double, 2);

}  // namespace lue
