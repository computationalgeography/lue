#include "lue/framework/algorithm/default_policies/focal_mean.hpp"
#include "lue/framework/algorithm/definition/focal_mean.hpp"
#include "lue/framework/algorithm/value_policies/focal_mean.hpp"
#include "lue/framework/algorithm/serialize/kernel.hpp"


#define LUE_INSTANTIATE(Element)                                 \
                                                                 \
    LUE_INSTANTIATE_FOCAL_MEAN(                                  \
        ESC(policy::focal_mean::DefaultPolicies<Element>),       \
        Element,                                                 \
        ESC(Kernel<std::uint8_t, 2>));                           \
                                                                 \
    LUE_INSTANTIATE_FOCAL_MEAN(                                  \
        ESC(policy::focal_mean::DefaultValuePolicies<Element>),  \
        Element,                                                 \
        ESC(Kernel<std::uint8_t, 2>));


namespace lue {

    LUE_INSTANTIATE(float);
    LUE_INSTANTIATE(double);

}  // namespace lue
