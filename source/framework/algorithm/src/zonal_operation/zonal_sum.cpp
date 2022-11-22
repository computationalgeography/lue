#include "lue/framework/algorithm/definition/zonal_sum.hpp"
#include "lue/framework/algorithm/value_policies/zonal_sum.hpp"


#define LUE_INSTANTIATE(Policies, Element, Zone)          \
                                                          \
    LUE_INSTANTIATE_ZONAL_SUM(                            \
        ESC(policy::zonal_sum::Policies<Element, Zone>),  \
        Element,                                          \
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

    LUE_INSTANTIATE(DefaultValuePolicies, float, std::uint8_t);
    LUE_INSTANTIATE(DefaultValuePolicies, float, std::uint32_t);
    LUE_INSTANTIATE(DefaultValuePolicies, float, std::uint64_t);

    LUE_INSTANTIATE(DefaultValuePolicies, double, std::uint8_t);
    LUE_INSTANTIATE(DefaultValuePolicies, double, std::uint32_t);
    LUE_INSTANTIATE(DefaultValuePolicies, double, std::uint64_t);

}  // namespace lue
