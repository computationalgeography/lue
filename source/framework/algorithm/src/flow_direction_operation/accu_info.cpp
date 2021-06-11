#include "lue/framework/algorithm/definition/accu_info.hpp"


#define LUE_INSTANTIATE(FlowDirectionElement)                             \
                                                                          \
    LUE_INSTANTIATE_ACCU_INFO(                                            \
        (policy::accu_info::DefaultValuePolicies<FlowDirectionElement>),  \
        FlowDirectionElement);


namespace lue {

    LUE_INSTANTIATE(std::uint8_t);

}  // namespace lue
