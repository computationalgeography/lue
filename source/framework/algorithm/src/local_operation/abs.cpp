#include "lue/framework/algorithm/default_policies/abs.hpp"
#include "lue/framework/algorithm/definition/abs.hpp"
#include "lue/framework/algorithm/value_policies/abs.hpp"


#define LUE_INSTANTIATE(Element, rank)                    \
                                                          \
    LUE_INSTANTIATE_ABS(                                  \
        ESC(policy::abs::DefaultPolicies<Element>),       \
        Element,                                          \
        rank);                                            \
                                                          \
    LUE_INSTANTIATE_ABS(                                  \
        ESC(policy::abs::DefaultValuePolicies<Element>),  \
        Element,                                          \
        rank);


namespace lue {

    LUE_INSTANTIATE(std::int32_t, 2);
    LUE_INSTANTIATE(std::int64_t, 2);
    LUE_INSTANTIATE(float, 2);
    LUE_INSTANTIATE(double, 2);

}  // namespace lue
