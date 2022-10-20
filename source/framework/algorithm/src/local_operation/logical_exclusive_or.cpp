#include "lue/framework/algorithm/default_policies/logical_exclusive_or.hpp"
#include "lue/framework/algorithm/value_policies/logical_exclusive_or.hpp"
#include "lue/framework/algorithm/definition/logical_exclusive_or.hpp"


#define LUE_INSTANTIATE(OutputElement, InputElement, rank)                                     \
                                                                                               \
    LUE_INSTANTIATE_BINARY_LOCAL_OPERATION(                                                    \
        ESC(policy::logical_exclusive_or::DefaultPolicies<OutputElement, InputElement>),       \
        OutputElement,                                                                         \
        InputElement,                                                                          \
        InputElement,                                                                          \
        rank,                                                                                  \
        ESC(detail::LogicalExclusiveOr<InputElement, OutputElement>));                         \
                                                                                               \
    LUE_INSTANTIATE_BINARY_LOCAL_OPERATION(                                                    \
        ESC(policy::logical_exclusive_or::DefaultValuePolicies<OutputElement, InputElement>),  \
        OutputElement,                                                                         \
        InputElement,                                                                          \
        InputElement,                                                                          \
        rank,                                                                                  \
        ESC(detail::LogicalExclusiveOr<InputElement, OutputElement>));


namespace lue {

    LUE_INSTANTIATE(std::uint8_t, std::uint8_t, 2);
    LUE_INSTANTIATE(std::uint8_t, std::int32_t, 2);
    LUE_INSTANTIATE(std::uint8_t, std::uint32_t, 2);
    LUE_INSTANTIATE(std::uint8_t, std::int64_t, 2);
    LUE_INSTANTIATE(std::uint8_t, std::uint64_t, 2);

}  // namespace lue
