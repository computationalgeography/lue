#include "lue/framework/algorithm/default_policies/accu3.hpp"
#include "lue/framework/algorithm/definition/accu3.hpp"
#include "lue/framework/algorithm/value_policies/accu3.hpp"


using lue_CellsIdxs = std::vector<lue::Index>;
using ChannelMaterial_float = lue::detail::ChannelMaterial<float, 2>;
using ChannelMaterial_double = lue::detail::ChannelMaterial<double, 2>;

HPX_REGISTER_CHANNEL_DECLARATION(lue_CellsIdxs);
HPX_REGISTER_CHANNEL_DECLARATION(ChannelMaterial_float);
HPX_REGISTER_CHANNEL_DECLARATION(ChannelMaterial_double);


#define LUE_INSTANTIATE(FlowDirectionElement, MaterialElement)                            \
                                                                                          \
    LUE_INSTANTIATE_ACCU3(                                                                \
        ESC(policy::accu3::DefaultPolicies<FlowDirectionElement, MaterialElement>),       \
        FlowDirectionElement,                                                             \
        MaterialElement);                                                                 \
                                                                                          \
    LUE_INSTANTIATE_ACCU3(                                                                \
        ESC(policy::accu3::DefaultValuePolicies<FlowDirectionElement, MaterialElement>),  \
        FlowDirectionElement,                                                             \
        MaterialElement);


namespace lue {

    LUE_INSTANTIATE(std::uint8_t, float);
    LUE_INSTANTIATE(std::uint8_t, double);

}  // namespace lue
