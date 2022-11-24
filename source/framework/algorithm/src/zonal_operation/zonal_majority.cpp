#include "lue/framework/algorithm/definition/zonal_majority.hpp"
#include "lue/framework/algorithm/value_policies/zonal_majority.hpp"


#define LUE_INSTANTIATE(Policies, Element, Zone)               \
                                                               \
    LUE_INSTANTIATE_ZONAL_MAJORITY(                            \
        ESC(policy::zonal_majority::Policies<Element, Zone>),  \
        Element,                                               \
        Zone);


namespace lue {

    LUE_INSTANTIATE(DefaultValuePolicies, std::uint8_t, std::uint8_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint8_t, std::uint32_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint8_t, std::uint64_t);

    LUE_INSTANTIATE(DefaultValuePolicies, std::int32_t, std::uint8_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::int32_t, std::uint32_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::int32_t, std::uint64_t);

    LUE_INSTANTIATE(DefaultValuePolicies, std::uint32_t, std::uint8_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint32_t, std::uint32_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint32_t, std::uint64_t);

    LUE_INSTANTIATE(DefaultValuePolicies, std::int64_t, std::uint8_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::int64_t, std::uint32_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::int64_t, std::uint64_t);

    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t, std::uint8_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t, std::uint32_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t, std::uint64_t);

}  // namespace lue
