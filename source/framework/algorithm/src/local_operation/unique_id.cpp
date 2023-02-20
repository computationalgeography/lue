#include "lue/framework/algorithm/default_policies/unique_id.hpp"
#include "lue/framework/algorithm/definition/unique_id.hpp"
#include "lue/framework/algorithm/value_policies/unique_id.hpp"


#define LUE_INSTANTIATE(Policies, IDElement, ConditionElement)                                               \
                                                                                                             \
    LUE_INSTANTIATE_UNIQUE_ID(                                                                               \
        ESC(policy::unique_id::Policies<IDElement, ConditionElement>), IDElement, ConditionElement);


namespace lue {

    LUE_INSTANTIATE(DefaultPolicies, std::uint8_t, std::uint8_t);
    LUE_INSTANTIATE(DefaultPolicies, std::uint32_t, std::uint8_t);
    LUE_INSTANTIATE(DefaultPolicies, std::int32_t, std::uint8_t);
    LUE_INSTANTIATE(DefaultPolicies, std::uint64_t, std::uint8_t);
    LUE_INSTANTIATE(DefaultPolicies, std::int64_t, std::uint8_t);

    LUE_INSTANTIATE(DefaultValuePolicies, std::uint8_t, std::uint8_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint32_t, std::uint8_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::int32_t, std::uint8_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t, std::uint8_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::int64_t, std::uint8_t);

}  // namespace lue
