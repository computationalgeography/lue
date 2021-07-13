#include "lue/framework/algorithm/definition/accu_threshold3.hpp"


#define LUE_INSTANTIATE(FlowDirectionElement, MaterialElement)                                   \
                                                                                                 \
    LUE_INSTANTIATE_ACCU_THRESHOLD3(                                                             \
        (policy::accu_threshold3::DefaultValuePolicies<FlowDirectionElement, MaterialElement>),  \
        FlowDirectionElement,                                                                    \
        MaterialElement);                                                                        \


namespace lue {

    LUE_INSTANTIATE(std::uint8_t, float);
    LUE_INSTANTIATE(std::uint8_t, double);

}  // namespace lue
