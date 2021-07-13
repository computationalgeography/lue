#include "lue/framework/algorithm/default_policies/accu3.hpp"
#include "lue/framework/algorithm/definition/accu3.hpp"
#include "lue/framework/algorithm/value_policies/accu3.hpp"


#define LUE_INSTANTIATE(FlowDirectionElement, MaterialElement)                         \
                                                                                       \
    LUE_INSTANTIATE_ACCU3(                                                             \
        (policy::accu3::DefaultPolicies<FlowDirectionElement, MaterialElement>),       \
        FlowDirectionElement,                                                          \
        MaterialElement);                                                              \
                                                                                       \
    LUE_INSTANTIATE_ACCU3(                                                             \
        (policy::accu3::DefaultValuePolicies<FlowDirectionElement, MaterialElement>),  \
        FlowDirectionElement,                                                          \
        MaterialElement);


namespace lue {

    LUE_INSTANTIATE(std::uint8_t, float);
    LUE_INSTANTIATE(std::uint8_t, double);

}  // namespace lue
