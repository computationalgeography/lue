#include "lue/framework/algorithm/default_policies/greater_than.hpp"
#include "lue/framework/algorithm/definition/greater_than.hpp"
#include "lue/framework/algorithm/value_policies/greater_than.hpp"


#define LUE_INSTANTIATE(OutputElement, InputElement, rank)                                                   \
                                                                                                             \
    LUE_INSTANTIATE_BINARY_LOCAL_OPERATION(                                                                  \
        ESC(policy::greater_than::DefaultPolicies<OutputElement, InputElement>),                             \
        OutputElement,                                                                                       \
        InputElement,                                                                                        \
        InputElement,                                                                                        \
        rank,                                                                                                \
        ESC(detail::GreaterThan<InputElement, OutputElement>));                                              \
                                                                                                             \
    LUE_INSTANTIATE_BINARY_LOCAL_OPERATION(                                                                  \
        ESC(policy::greater_than::DefaultValuePolicies<OutputElement, InputElement>),                        \
        OutputElement,                                                                                       \
        InputElement,                                                                                        \
        InputElement,                                                                                        \
        rank,                                                                                                \
        ESC(detail::GreaterThan<InputElement, OutputElement>));


namespace lue {

    LUE_INSTANTIATE(std::uint8_t, std::uint8_t, 2);
    LUE_INSTANTIATE(std::uint8_t, std::uint32_t, 2);
    LUE_INSTANTIATE(std::uint8_t, std::uint64_t, 2);
    LUE_INSTANTIATE(std::uint8_t, std::int32_t, 2);
    LUE_INSTANTIATE(std::uint8_t, std::int64_t, 2);
    LUE_INSTANTIATE(std::uint8_t, float, 2);
    LUE_INSTANTIATE(std::uint8_t, double, 2);

}  // namespace lue
