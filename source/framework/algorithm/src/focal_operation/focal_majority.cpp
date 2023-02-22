#include "lue/framework/algorithm/definition/focal_majority.hpp"
#include "lue/framework/algorithm/serialize/kernel.hpp"
#include "lue/framework/algorithm/value_policies/focal_majority.hpp"


#define LUE_INSTANTIATE(Policies, Element)                                                                   \
                                                                                                             \
    LUE_INSTANTIATE_FOCAL_MAJORITY(                                                                          \
        ESC(policy::focal_majority::Policies<Element>), Element, ESC(Kernel<std::uint8_t, 2>));


namespace lue {

    LUE_INSTANTIATE(DefaultValuePolicies, std::uint8_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::int32_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint32_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::int64_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t);

}  // namespace lue
