#include "lue/framework/algorithm/definition/logical_inclusive_or.hpp"


#define LUE_INSTANTIATE(OutputElement, InputElement, rank)                                     \
                                                                                               \
    LUE_INSTANTIATE_BINARY_LOCAL_OPERATION(                                                    \
        ESC(policy::logical_inclusive_or::DefaultValuePolicies<OutputElement, InputElement>),  \
        OutputElement,                                                                         \
        InputElement,                                                                          \
        InputElement,                                                                          \
        rank,                                                                                  \
        ESC(detail::LogicalInclusiveOr<InputElement, OutputElement>));


namespace lue {

    LUE_INSTANTIATE(std::uint8_t, std::uint8_t, 2);

}  // namespace lue
