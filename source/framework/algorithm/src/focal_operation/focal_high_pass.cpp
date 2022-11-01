#include "lue/framework/algorithm/definition/focal_high_pass.hpp"
#include "lue/framework/algorithm/value_policies/focal_high_pass.hpp"
#include "lue/framework/algorithm/serialize/kernel.hpp"


#define LUE_INSTANTIATE(Policies, Element)                \
                                                          \
    LUE_INSTANTIATE_FOCAL_HIGH_PASS(                      \
        ESC(policy::focal_high_pass::Policies<Element>),  \
        Element,                                          \
        ESC(Kernel<std::uint8_t, 2>));


namespace lue {

    LUE_INSTANTIATE(DefaultValuePolicies, float);
    LUE_INSTANTIATE(DefaultValuePolicies, double);

}  // namespace lue
