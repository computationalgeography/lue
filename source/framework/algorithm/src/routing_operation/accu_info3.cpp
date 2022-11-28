#include "lue/framework/algorithm/definition/accu_info3.hpp"


#define LUE_INSTANTIATE(FlowDirectionElement)                                 \
                                                                              \
    LUE_INSTANTIATE_ACCU_INFO3(                                               \
        ESC(policy::accu_info3::DefaultValuePolicies<FlowDirectionElement>),  \
        FlowDirectionElement);


namespace lue {

    LUE_INSTANTIATE(std::uint8_t);

}  // namespace lue
