#include "lue/framework/algorithm/default_policies/divide.hpp"
#include "lue/framework/algorithm/value_policies/divide.hpp"
#include "lue/framework/algorithm/definition/divide.hpp"


#define LUE_INSTANTIATE(Element, rank)                                \
                                                                      \
    LUE_INSTANTIATE_BINARY_LOCAL_OPERATION(                           \
        ESC(policy::divide::DefaultPolicies<Element, Element>),       \
        Element,                                                      \
        Element,                                                      \
        Element,                                                      \
        rank,                                                         \
        ESC(detail::Divide<Element, Element>));                       \
                                                                      \
    LUE_INSTANTIATE_BINARY_LOCAL_OPERATION(                           \
        ESC(policy::divide::DefaultValuePolicies<Element, Element>),  \
        Element,                                                      \
        Element,                                                      \
        Element,                                                      \
        rank,                                                         \
        ESC(detail::Divide<Element, Element>));


namespace lue {

    LUE_INSTANTIATE(float, 2);
    LUE_INSTANTIATE(double, 2);

}  // namespace lue
