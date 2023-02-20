#include "lue/framework/algorithm/default_policies/cos.hpp"
#include "lue/framework/algorithm/definition/cos.hpp"
#include "lue/framework/algorithm/value_policies/cos.hpp"


#define LUE_INSTANTIATE(Element, rank)                                                                       \
                                                                                                             \
    LUE_INSTANTIATE_COS(ESC(policy::cos::DefaultPolicies<Element>), Element, rank);                          \
                                                                                                             \
    LUE_INSTANTIATE_COS(ESC(policy::cos::DefaultValuePolicies<Element>), Element, rank);


namespace lue {

    LUE_INSTANTIATE(float, 2);
    LUE_INSTANTIATE(double, 2);

}  // namespace lue
