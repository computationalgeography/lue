#include "lue/framework/algorithm/definition/downstream_distance.hpp"


#define LUE_INSTANTIATE(FlowDirectionElement, DistanceElement)                                               \
                                                                                                             \
    LUE_INSTANTIATE_DOWNSTREAM_DISTANCE(                                                                     \
        ESC(policy::downstream_distance::DefaultValuePolicies<FlowDirectionElement, DistanceElement>),       \
        FlowDirectionElement,                                                                                \
        DistanceElement);


namespace lue {

    LUE_INSTANTIATE(std::uint8_t, float);
    LUE_INSTANTIATE(std::uint8_t, double);

}  // namespace lue
