#include "lue/framework/algorithm/default_policies/logical_not.hpp"
#include "lue/framework/algorithm/definition/logical_not.hpp"
#include "lue/framework/algorithm/value_policies/logical_not.hpp"


#define LUE_INSTANTIATE(Element, rank)                                          \
                                                                                \
    LUE_INSTANTIATE_LOGICAL_NOT(                                                \
        ESC(policy::logical_not::DefaultPolicies<std::uint8_t, Element>),       \
        Element,                                                                \
        rank);                                                                  \
                                                                                \
    LUE_INSTANTIATE_LOGICAL_NOT(                                                \
        ESC(policy::logical_not::DefaultValuePolicies<std::uint8_t, Element>),  \
        Element,                                                                \
        rank);


namespace lue {

    LUE_INSTANTIATE(std::uint8_t, 2);
    LUE_INSTANTIATE(std::int32_t, 2);
    LUE_INSTANTIATE(std::uint32_t, 2);
    LUE_INSTANTIATE(std::int64_t, 2);
    LUE_INSTANTIATE(std::uint64_t, 2);

}  // namespace lue
