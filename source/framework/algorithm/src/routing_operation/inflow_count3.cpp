#include "lue/framework/algorithm/definition/inflow_count3.hpp"


using lue_CellsIdxs = std::vector<lue::Index>;

HPX_REGISTER_CHANNEL_DECLARATION(lue_CellsIdxs);


#define LUE_INSTANTIATE(CountElement, FlowDirectionElement)                                                  \
                                                                                                             \
    LUE_INSTANTIATE_INFLOW_COUNT3(                                                                           \
        ESC(policy::inflow_count3::DefaultValuePolicies<CountElement, FlowDirectionElement>),                \
        CountElement,                                                                                        \
        FlowDirectionElement);


namespace lue {

    LUE_INSTANTIATE(std::uint8_t, std::uint8_t);

}  // namespace lue
