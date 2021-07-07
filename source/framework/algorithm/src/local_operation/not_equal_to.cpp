#include "lue/framework/algorithm/default_policies/not_equal_to.hpp"
#include "lue/framework/algorithm/definition/not_equal_to.hpp"
#include "lue/framework/algorithm/value_policies/not_equal_to.hpp"


#define LUE_INSTANTIATE(OutputElement, InputElement, rank)                          \
                                                                                    \
    LUE_INSTANTIATE_BINARY_LOCAL_OPERATION(                                         \
        (policy::not_equal_to::DefaultPolicies<OutputElement, InputElement>),       \
        OutputElement,                                                              \
        InputElement,                                                               \
        InputElement,                                                               \
        rank,                                                                       \
        (detail::NotEqualTo<InputElement, OutputElement>));                         \
                                                                                    \
    LUE_INSTANTIATE_BINARY_LOCAL_OPERATION(                                         \
        (policy::not_equal_to::DefaultValuePolicies<OutputElement, InputElement>),  \
        OutputElement,                                                              \
        InputElement,                                                               \
        InputElement,                                                               \
        rank,                                                                       \
        (detail::NotEqualTo<InputElement, OutputElement>));


namespace lue {

    LUE_INSTANTIATE(std::uint8_t, std::uint8_t, 2);
    LUE_INSTANTIATE(std::uint8_t, std::uint32_t, 2);
    LUE_INSTANTIATE(std::uint8_t, std::uint64_t, 2);
    LUE_INSTANTIATE(std::uint8_t, std::int32_t, 2);
    LUE_INSTANTIATE(std::uint8_t, std::int64_t, 2);
    LUE_INSTANTIATE(std::uint8_t, float, 2);
    LUE_INSTANTIATE(std::uint8_t, double, 2);

}  // namespace lue
