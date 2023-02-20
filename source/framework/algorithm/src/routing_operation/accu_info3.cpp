#include "lue/framework/algorithm/definition/accu_info3.hpp"


using lue_CellsIdxs = std::vector<lue::Index>;
using ChannelMaterial_uint8 = lue::detail::ChannelMaterial<std::uint8_t, 2>;

HPX_REGISTER_CHANNEL_DECLARATION(lue_CellsIdxs);
HPX_REGISTER_CHANNEL_DECLARATION(ChannelMaterial_uint8);


#define LUE_INSTANTIATE(FlowDirectionElement)                                                                \
                                                                                                             \
    LUE_INSTANTIATE_ACCU_INFO3(                                                                              \
        ESC(policy::accu_info3::DefaultValuePolicies<FlowDirectionElement>), FlowDirectionElement);


namespace lue {

    LUE_INSTANTIATE(std::uint8_t);

}  // namespace lue
