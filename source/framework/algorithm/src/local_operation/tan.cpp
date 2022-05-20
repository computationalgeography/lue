#include "lue/framework/algorithm/default_policies/tan.hpp"
#include "lue/framework/algorithm/definition/tan.hpp"
#include "lue/framework/algorithm/value_policies/tan.hpp"


#define LUE_INSTANTIATE(Element, rank)                    \
                                                          \
    LUE_INSTANTIATE_TAN(                                  \
        ESC(policy::tan::DefaultPolicies<Element>),       \
        Element,                                          \
        rank);                                            \
                                                          \
    LUE_INSTANTIATE_TAN(                                  \
        ESC(policy::tan::DefaultValuePolicies<Element>),  \
        Element,                                          \
        rank);


namespace lue {

    LUE_INSTANTIATE(float, 2);
    LUE_INSTANTIATE(double, 2);

}  // namespace lue
