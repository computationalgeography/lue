#include "lue/framework/algorithm/definition/zonal_area.hpp"
#include "lue/framework/algorithm/value_policies/zonal_area.hpp"


#define LUE_INSTANTIATE(Policies, Count, Zone)                                                               \
                                                                                                             \
    LUE_INSTANTIATE_ZONAL_AREA(ESC(policy::zonal_area::Policies<Count, Zone>), Count, Zone);


namespace lue {

    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t, std::uint8_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t, std::uint32_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t, std::uint64_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t, std::int32_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t, std::int64_t);

}  // namespace lue
