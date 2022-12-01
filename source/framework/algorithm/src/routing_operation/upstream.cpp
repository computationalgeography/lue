#include "lue/framework/algorithm/definition/upstream.hpp"


#define LUE_INSTANTIATE(FlowDirectionElement, MaterialElement)                                \
                                                                                              \
    LUE_INSTANTIATE_UPSTREAM(                                                                 \
        ESC(policy::upstream::DefaultValuePolicies<FlowDirectionElement, MaterialElement>),   \
        FlowDirectionElement,                                                                 \
        MaterialElement);                                                                     \


namespace lue {

    LUE_INSTANTIATE(std::uint8_t, std::uint64_t);
    LUE_INSTANTIATE(std::uint8_t, float);
    LUE_INSTANTIATE(std::uint8_t, double);

}  // namespace lue
