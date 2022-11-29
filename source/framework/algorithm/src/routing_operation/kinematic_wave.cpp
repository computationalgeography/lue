#include "lue/framework/algorithm/definition/kinematic_wave.hpp"
#include "lue/framework/algorithm/value_policies/kinematic_wave.hpp"


using lue_CellsIdxs = std::vector<lue::Index>;
using ChannelMaterial_float = lue::detail::ChannelMaterial<float, 2>;
using ChannelMaterial_double = lue::detail::ChannelMaterial<double, 2>;

HPX_REGISTER_CHANNEL_DECLARATION(lue_CellsIdxs);
HPX_REGISTER_CHANNEL_DECLARATION(ChannelMaterial_float);
HPX_REGISTER_CHANNEL_DECLARATION(ChannelMaterial_double);


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
