#include "lue/framework/algorithm/default_policies/sin.hpp"
#include "lue/framework/algorithm/definition/sin.hpp"
#include "lue/framework/algorithm/value_policies/sin.hpp"


#define LUE_INSTANTIATE(Element, rank)                    \
                                                          \
    LUE_INSTANTIATE_SIN(                                  \
        ESC(policy::sin::DefaultPolicies<Element>),       \
        Element,                                          \
        rank);                                            \
                                                          \
    LUE_INSTANTIATE_SIN(                                  \
        ESC(policy::sin::DefaultValuePolicies<Element>),  \
        Element,                                          \
        rank);


namespace lue {

    LUE_INSTANTIATE(float, 2);
    LUE_INSTANTIATE(double, 2);

}  // namespace lue
