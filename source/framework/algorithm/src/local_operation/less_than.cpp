#include "lue/framework/algorithm/default_policies/less_than.hpp"
#include "lue/framework/algorithm/definition/less_than.hpp"
#include "lue/framework/algorithm/value_policies/less_than.hpp"


#define LUE_INSTANTIATE(OutputElement, InputElement, rank)                                                   \
                                                                                                             \
    LUE_INSTANTIATE_BINARY_LOCAL_OPERATION(                                                                  \
        ESC(policy::less_than::DefaultPolicies<OutputElement, InputElement>),                                \
        OutputElement,                                                                                       \
        InputElement,                                                                                        \
        InputElement,                                                                                        \
        rank,                                                                                                \
        ESC(detail::LessThan<InputElement, OutputElement>));                                                 \
                                                                                                             \
    LUE_INSTANTIATE_BINARY_LOCAL_OPERATION(                                                                  \
        ESC(policy::less_than::DefaultValuePolicies<OutputElement, InputElement>),                           \
        OutputElement,                                                                                       \
        InputElement,                                                                                        \
        InputElement,                                                                                        \
        rank,                                                                                                \
        ESC(detail::LessThan<InputElement, OutputElement>));


namespace lue {

    LUE_INSTANTIATE(std::uint8_t, std::uint8_t, 2);
    LUE_INSTANTIATE(std::uint8_t, std::uint32_t, 2);
    LUE_INSTANTIATE(std::uint8_t, std::uint64_t, 2);
    LUE_INSTANTIATE(std::uint8_t, std::int32_t, 2);
    LUE_INSTANTIATE(std::uint8_t, std::int64_t, 2);
    LUE_INSTANTIATE(std::uint8_t, float, 2);
    LUE_INSTANTIATE(std::uint8_t, double, 2);

}  // namespace lue
