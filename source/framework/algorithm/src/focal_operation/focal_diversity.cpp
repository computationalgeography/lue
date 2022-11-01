#include "lue/framework/algorithm/definition/focal_diversity.hpp"
#include "lue/framework/algorithm/value_policies/focal_diversity.hpp"
#include "lue/framework/algorithm/serialize/kernel.hpp"


#define LUE_INSTANTIATE(Policies, Count, Element)                \
                                                                 \
    LUE_INSTANTIATE_FOCAL_DIVERSITY(                             \
        ESC(policy::focal_diversity::Policies<Count, Element>),  \
        Count,                                                   \
        Element,                                                 \
        ESC(Kernel<std::uint8_t, 2>));


namespace lue {

    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t, std::uint8_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t, std::int32_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t, std::uint32_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t, std::int64_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t, std::uint64_t);

}  // namespace lue
