#include "lue/framework/algorithm/definition/accu3.hpp"


#define LUE_INSTANTIATE(FlowDirectionElement, MaterialElement)                         \
                                                                                       \
    LUE_INSTANTIATE_ACCU3(                                                             \
        (policy::accu3::DefaultValuePolicies<FlowDirectionElement, MaterialElement>),  \
        FlowDirectionElement,                                                          \
        MaterialElement);                                                              \


namespace lue {

    LUE_INSTANTIATE(std::uint8_t, float);
    LUE_INSTANTIATE(std::uint8_t, double);

}  // namespace lue
