#include "lue/framework/algorithm/definition/kinematic_wave.hpp"
#include "lue/framework/algorithm/value_policies/kinematic_wave.hpp"


#define LUE_INSTANTIATE(FlowDirectionElement, Element)                                     \
                                                                                           \
    LUE_INSTANTIATE_KINEMATIC_WAVE(                                                        \
        ESC(policy::kinematic_wave::DefaultValuePolicies<FlowDirectionElement, Element>),  \
        FlowDirectionElement,                                                              \
        Element);


namespace lue {

    LUE_INSTANTIATE(std::uint8_t, float);
    LUE_INSTANTIATE(std::uint8_t, double);

}  // namespace lue
