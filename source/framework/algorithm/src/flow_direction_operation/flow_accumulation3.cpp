#include "lue/framework/algorithm/definition/flow_accumulation3.hpp"


using ChannelMaterial_float = lue::detail::ChannelMaterial<float, 2>;
using ChannelMaterial_double = lue::detail::ChannelMaterial<double, 2>;

HPX_REGISTER_CHANNEL(ChannelMaterial_float);
HPX_REGISTER_CHANNEL(ChannelMaterial_double);
