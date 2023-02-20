#include "lue/framework/algorithm/default_policies/atan2.hpp"
#include "lue/framework/algorithm/definition/atan2.hpp"
#include "lue/framework/algorithm/value_policies/atan2.hpp"


#define LUE_INSTANTIATE(Element, rank)                                                                       \
                                                                                                             \
    LUE_INSTANTIATE_ATAN2(ESC(policy::atan2::DefaultPolicies<Element>), Element, rank);                      \
                                                                                                             \
    LUE_INSTANTIATE_ATAN2(ESC(policy::atan2::DefaultValuePolicies<Element>), Element, rank);


namespace lue {

    LUE_INSTANTIATE(float, 2);
    LUE_INSTANTIATE(double, 2);

}  // namespace lue
