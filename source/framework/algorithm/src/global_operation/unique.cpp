#include "lue/framework/algorithm/default_policies/unique.hpp"
#include "lue/framework/algorithm/definition/unique.hpp"
#include "lue/framework/algorithm/value_policies/unique.hpp"


#define LUE_INSTANTIATE(Policies, Element)       \
                                                 \
    LUE_INSTANTIATE_UNIQUE(                      \
        ESC(policy::unique::Policies<Element>),  \
        Element);


namespace lue {

    LUE_INSTANTIATE(DefaultPolicies, std::uint8_t);
    LUE_INSTANTIATE(DefaultPolicies, std::uint32_t);
    LUE_INSTANTIATE(DefaultPolicies, std::int32_t);
    LUE_INSTANTIATE(DefaultPolicies, std::uint64_t);
    LUE_INSTANTIATE(DefaultPolicies, std::int64_t);

    LUE_INSTANTIATE(DefaultValuePolicies, std::uint8_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint32_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::int32_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::uint64_t);
    LUE_INSTANTIATE(DefaultValuePolicies, std::int64_t);

}  // namespace lue
