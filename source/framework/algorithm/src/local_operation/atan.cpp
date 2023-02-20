#include "lue/framework/algorithm/default_policies/atan.hpp"
#include "lue/framework/algorithm/definition/atan.hpp"
#include "lue/framework/algorithm/value_policies/atan.hpp"


#define LUE_INSTANTIATE(Element, rank)                                                                       \
                                                                                                             \
    LUE_INSTANTIATE_ATAN(ESC(policy::atan::DefaultPolicies<Element>), Element, rank);                        \
                                                                                                             \
    LUE_INSTANTIATE_ATAN(ESC(policy::atan::DefaultValuePolicies<Element>), Element, rank);


namespace lue {

    LUE_INSTANTIATE(float, 2);
    LUE_INSTANTIATE(double, 2);

}  // namespace lue
