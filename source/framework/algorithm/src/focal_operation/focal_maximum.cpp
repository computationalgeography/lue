#include "lue/framework/algorithm/definition/focal_maximum.hpp"
#include "lue/framework/algorithm/serialize/kernel.hpp"
#include "lue/framework/algorithm/value_policies/focal_maximum.hpp"


#define LUE_INSTANTIATE(Policies, Element)                                                                   \
                                                                                                             \
    LUE_INSTANTIATE_FOCAL_MAXIMUM(                                                                           \
        ESC(policy::focal_maximum::Policies<Element>), Element, ESC(Kernel<std::uint8_t, 2>));


namespace lue {

    LUE_INSTANTIATE(DefaultValuePolicies, std::uint8_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::int32_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint32_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::int64_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t);
    LUE_INSTANTIATE(DefaultValuePolicies, float);
    LUE_INSTANTIATE(DefaultValuePolicies, double);

}  // namespace lue
