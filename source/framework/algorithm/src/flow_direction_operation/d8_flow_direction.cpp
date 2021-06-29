#include "lue/framework/algorithm/definition/d8_flow_direction.hpp"


#define LUE_INSTANTIATE(FlowDirectionElement, ElevationElement)                                     \
                                                                                                    \
    LUE_INSTANTIATE_D8_FLOW_DIRECTION(                                                              \
        (policy::d8_flow_direction::DefaultValuePolicies<FlowDirectionElement, ElevationElement>),  \
        FlowDirectionElement,                                                                       \
        ElevationElement);


namespace lue {

    LUE_INSTANTIATE(std::uint8_t, float);
    LUE_INSTANTIATE(std::uint8_t, double);

}  // namespace lue
