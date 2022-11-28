#include "lue/framework/algorithm/definition/accu_fraction.hpp"


#define LUE_INSTANTIATE(FlowDirectionElement, MaterialElement, FractionElement)  \
                                                                                 \
    LUE_INSTANTIATE_ACCU_FRACTION(                                               \
        ESC(policy::accu_fraction::DefaultValuePolicies<                         \
            FlowDirectionElement, MaterialElement, FractionElement>),            \
        FlowDirectionElement,                                                    \
        MaterialElement,                                                         \
        FractionElement);                                                        \


namespace lue {

    LUE_INSTANTIATE(std::uint8_t, float, float);
    LUE_INSTANTIATE(std::uint8_t, double, double);

}  // namespace lue
