#include "lue/framework/algorithm/definition/pow.hpp"


#define LUE_INSTANTIATE(Element, rank)                          \
                                                                \
    LUE_INSTANTIATE_BINARY_LOCAL_OPERATION(                     \
        (policy::pow::DefaultValuePolicies<Element, Element>),  \
        Element,                                                \
        Element,                                                \
        Element,                                                \
        rank,                                                   \
        (detail::Pow<Element, Element>));


namespace lue {

    LUE_INSTANTIATE(float, 2);
    LUE_INSTANTIATE(double, 2);

}  // namespace lue
