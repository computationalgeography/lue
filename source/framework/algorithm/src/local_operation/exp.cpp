#include "lue/framework/algorithm/default_policies/exp.hpp"
#include "lue/framework/algorithm/definition/exp.hpp"
#include "lue/framework/algorithm/value_policies/exp.hpp"


#define LUE_INSTANTIATE(Element, rank)                    \
                                                          \
    LUE_INSTANTIATE_EXP(                                  \
        ESC(policy::exp::DefaultPolicies<Element>),       \
        Element,                                          \
        rank);                                            \
                                                          \
    LUE_INSTANTIATE_EXP(                                  \
        ESC(policy::exp::DefaultValuePolicies<Element>),  \
        Element,                                          \
        rank);


namespace lue {

    LUE_INSTANTIATE(float, 2);
    LUE_INSTANTIATE(double, 2);

}  // namespace lue
