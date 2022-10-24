#include "lue/framework/algorithm/default_policies/focal_maximum.hpp"
#include "lue/framework/algorithm/definition/focal_maximum.hpp"
#include "lue/framework/algorithm/value_policies/focal_maximum.hpp"
#include "lue/framework/algorithm/serialize/kernel.hpp"


#define LUE_INSTANTIATE(Element)                                    \
                                                                    \
    LUE_INSTANTIATE_FOCAL_MAXIMUM(                                  \
        ESC(policy::focal_maximum::DefaultPolicies<Element>),       \
        Element,                                                    \
        ESC(Kernel<std::uint8_t, 2>));                              \
                                                                    \
    LUE_INSTANTIATE_FOCAL_MAXIMUM(                                  \
        ESC(policy::focal_maximum::DefaultValuePolicies<Element>),  \
        Element,                                                    \
        ESC(Kernel<std::uint8_t, 2>));


namespace lue {

    LUE_INSTANTIATE(std::uint8_t);
    LUE_INSTANTIATE(std::int32_t);
    LUE_INSTANTIATE(std::uint32_t);
    LUE_INSTANTIATE(std::int64_t);
    LUE_INSTANTIATE(std::uint64_t);
    LUE_INSTANTIATE(float);
    LUE_INSTANTIATE(double);

}  // namespace lue
