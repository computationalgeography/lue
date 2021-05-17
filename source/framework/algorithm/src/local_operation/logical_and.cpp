#include "lue/framework/algorithm/definition/logical_and.hpp"


#define LUE_INSTANTIATE(OutputElement, InputElement, rank)                         \
                                                                                   \
    LUE_INSTANTIATE_BINARY_LOCAL_OPERATION(                                        \
        (policy::logical_and::DefaultValuePolicies<OutputElement, InputElement>),  \
        OutputElement,                                                             \
        InputElement,                                                              \
        InputElement,                                                              \
        rank,                                                                      \
        (detail::LogicalAnd<InputElement, OutputElement>));


namespace lue {

    LUE_INSTANTIATE(std::uint8_t, std::uint8_t, 2);

}  // namespace lue
