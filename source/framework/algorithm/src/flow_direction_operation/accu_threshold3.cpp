#include "lue/framework/algorithm/definition/accu_threshold3.hpp"


using ChannelMaterial_float = lue::detail::ChannelMaterial<float, 2>;
using ChannelMaterial_double = lue::detail::ChannelMaterial<double, 2>;

HPX_REGISTER_CHANNEL(ChannelMaterial_float);
HPX_REGISTER_CHANNEL(ChannelMaterial_double);


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
