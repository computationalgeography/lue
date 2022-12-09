#include "lue/framework/algorithm/definition/zonal_normal.hpp"
#include "lue/framework/algorithm/value_policies/zonal_normal.hpp"


#define LUE_INSTANTIATE(Policies, Element, Zone)             \
                                                             \
    LUE_INSTANTIATE_ZONAL_NORMAL(                            \
        ESC(policy::zonal_normal::Policies<Element, Zone>),  \
        Element,                                             \
        Zone);


namespace lue {

    LUE_INSTANTIATE(DefaultValuePolicies, float, std::uint8_t);
    LUE_INSTANTIATE(DefaultValuePolicies, float, std::uint32_t);
    LUE_INSTANTIATE(DefaultValuePolicies, float, std::uint64_t);

    LUE_INSTANTIATE(DefaultValuePolicies, double, std::uint8_t);
    LUE_INSTANTIATE(DefaultValuePolicies, double, std::uint32_t);
    LUE_INSTANTIATE(DefaultValuePolicies, double, std::uint64_t);

}  // namespace lue
