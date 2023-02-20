#include "lue/framework/algorithm/definition/zonal_uniform.hpp"
#include "lue/framework/algorithm/value_policies/zonal_uniform.hpp"


#define LUE_INSTANTIATE(Policies, Element, Zone)                                                             \
                                                                                                             \
    LUE_INSTANTIATE_ZONAL_UNIFORM(ESC(policy::zonal_uniform::Policies<Element, Zone>), Element, Zone);


namespace lue {

    LUE_INSTANTIATE(DefaultValuePolicies, float, std::uint8_t);
    LUE_INSTANTIATE(DefaultValuePolicies, float, std::uint32_t);
    LUE_INSTANTIATE(DefaultValuePolicies, float, std::uint64_t);
    LUE_INSTANTIATE(DefaultValuePolicies, float, std::int32_t);
    LUE_INSTANTIATE(DefaultValuePolicies, float, std::int64_t);

    LUE_INSTANTIATE(DefaultValuePolicies, double, std::uint8_t);
    LUE_INSTANTIATE(DefaultValuePolicies, double, std::uint32_t);
    LUE_INSTANTIATE(DefaultValuePolicies, double, std::uint64_t);
    LUE_INSTANTIATE(DefaultValuePolicies, double, std::int32_t);
    LUE_INSTANTIATE(DefaultValuePolicies, double, std::int64_t);

}  // namespace lue
