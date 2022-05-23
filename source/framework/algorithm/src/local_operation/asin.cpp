#include "lue/framework/algorithm/default_policies/asin.hpp"
#include "lue/framework/algorithm/definition/asin.hpp"
#include "lue/framework/algorithm/value_policies/asin.hpp"


#define LUE_INSTANTIATE(Element, rank)                     \
                                                           \
    LUE_INSTANTIATE_ASIN(                                  \
        ESC(policy::asin::DefaultPolicies<Element>),       \
        Element,                                           \
        rank);                                             \
                                                           \
    LUE_INSTANTIATE_ASIN(                                  \
        ESC(policy::asin::DefaultValuePolicies<Element>),  \
        Element,                                           \
        rank);


namespace lue {

    LUE_INSTANTIATE(float, 2);
    LUE_INSTANTIATE(double, 2);

}  // namespace lue
