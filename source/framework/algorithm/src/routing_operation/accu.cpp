#include "lue/framework/algorithm/definition/accu.hpp"


#define LUE_INSTANTIATE(FlowDirectionElement, MaterialElement)                                               \
                                                                                                             \
    LUE_INSTANTIATE_ACCU(                                                                                    \
        ESC(policy::accu::DefaultValuePolicies<FlowDirectionElement, MaterialElement>),                      \
        FlowDirectionElement,                                                                                \
        MaterialElement);


namespace lue {

    LUE_INSTANTIATE(std::uint8_t, std::uint64_t);
    LUE_INSTANTIATE(std::uint8_t, float);
    LUE_INSTANTIATE(std::uint8_t, double);

}  // namespace lue
