#include "lue/framework/algorithm/definition/zonal_diversity.hpp"
#include "lue/framework/algorithm/value_policies/zonal_diversity.hpp"


#define LUE_INSTANTIATE(Policies, Count, Element, Zone)                \
                                                                       \
    LUE_INSTANTIATE_ZONAL_DIVERSITY(                                   \
        ESC(policy::zonal_diversity::Policies<Count, Element, Zone>),  \
        Count,                                                         \
        Element,                                                       \
        Zone);


namespace lue {

    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t, std::uint8_t, std::uint8_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t, std::uint8_t, std::uint32_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t, std::uint8_t, std::uint64_t);

    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t, std::int32_t, std::uint8_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t, std::int32_t, std::uint32_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t, std::int32_t, std::uint64_t);

    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t, std::uint32_t, std::uint8_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t, std::uint32_t, std::uint32_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t, std::uint32_t, std::uint64_t);

    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t, std::int64_t, std::uint8_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t, std::int64_t, std::uint32_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t, std::int64_t, std::uint64_t);

    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t, std::uint64_t, std::uint8_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t, std::uint64_t, std::uint32_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t, std::uint64_t, std::uint64_t);

}  // namespace lue
