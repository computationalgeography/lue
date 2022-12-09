#include "lue/framework/algorithm/default_policies/reclassify.hpp"
#include "lue/framework/algorithm/definition/reclassify.hpp"
#include "lue/framework/algorithm/value_policies/reclassify.hpp"


#define LUE_INSTANTIATE(Policies, FromElement, ToElement)           \
                                                                    \
    LUE_INSTANTIATE_RECLASSIFY(                                     \
        ESC(policy::reclassify::Policies<FromElement, ToElement>),  \
        FromElement,                                                \
        ToElement);


namespace lue {

    LUE_INSTANTIATE(DefaultPolicies, std::uint8_t, float);
    LUE_INSTANTIATE(DefaultPolicies, std::uint8_t, double);

    LUE_INSTANTIATE(DefaultPolicies, std::uint32_t, float);
    LUE_INSTANTIATE(DefaultPolicies, std::uint32_t, double);

    LUE_INSTANTIATE(DefaultPolicies, std::int32_t, float);
    LUE_INSTANTIATE(DefaultPolicies, std::int32_t, double);

    LUE_INSTANTIATE(DefaultPolicies, std::uint64_t, float);
    LUE_INSTANTIATE(DefaultPolicies, std::uint64_t, double);

    LUE_INSTANTIATE(DefaultPolicies, std::int64_t, float);
    LUE_INSTANTIATE(DefaultPolicies, std::int64_t, double);

    LUE_INSTANTIATE(DefaultValuePolicies, std::uint8_t, float);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint8_t, double);

    LUE_INSTANTIATE(DefaultValuePolicies, std::uint32_t, float);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint32_t, double);

    LUE_INSTANTIATE(DefaultValuePolicies, std::int32_t, float);
    LUE_INSTANTIATE(DefaultValuePolicies, std::int32_t, double);

    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t, float);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t, double);

    LUE_INSTANTIATE(DefaultValuePolicies, std::int64_t, float);
    LUE_INSTANTIATE(DefaultValuePolicies, std::int64_t, double);

}  // namespace lue
