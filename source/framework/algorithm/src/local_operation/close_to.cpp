#include "lue/framework/algorithm/default_policies/close_to.hpp"
#include "lue/framework/algorithm/definition/close_to.hpp"
#include "lue/framework/algorithm/value_policies/close_to.hpp"


#define LUE_INSTANTIATE(OutputElement, InputElement, rank)                      \
                                                                                \
    LUE_INSTANTIATE_BINARY_LOCAL_OPERATION(                                     \
        (policy::close_to::DefaultPolicies<OutputElement, InputElement>),       \
        OutputElement,                                                          \
        InputElement,                                                           \
        InputElement,                                                           \
        rank,                                                                   \
        (detail::CloseTo<InputElement, OutputElement>));                        \
                                                                                \
    LUE_INSTANTIATE_BINARY_LOCAL_OPERATION(                                     \
        (policy::close_to::DefaultValuePolicies<OutputElement, InputElement>),  \
        OutputElement,                                                          \
        InputElement,                                                           \
        InputElement,                                                           \
        rank,                                                                   \
        (detail::CloseTo<InputElement, OutputElement>));


namespace lue {

    LUE_INSTANTIATE(std::uint8_t, float, 2);
    LUE_INSTANTIATE(std::uint8_t, double, 2);

}  // namespace lue
