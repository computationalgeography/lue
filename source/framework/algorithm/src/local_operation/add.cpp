#include "lue/framework/algorithm/default_policies/add.hpp"
#include "lue/framework/algorithm/definition/add.hpp"
#include "lue/framework/algorithm/value_policies/add.hpp"


#define LUE_INSTANTIATE(Element, rank)                                                                       \
                                                                                                             \
    LUE_INSTANTIATE_BINARY_LOCAL_OPERATION(                                                                  \
        ESC(policy::add::DefaultPolicies<Element, Element>),                                                 \
        Element,                                                                                             \
        Element,                                                                                             \
        Element,                                                                                             \
        rank,                                                                                                \
        ESC(detail::Add<Element, Element>));                                                                 \
                                                                                                             \
    LUE_INSTANTIATE_BINARY_LOCAL_OPERATION(                                                                  \
        ESC(policy::add::DefaultValuePolicies<Element, Element>),                                            \
        Element,                                                                                             \
        Element,                                                                                             \
        Element,                                                                                             \
        rank,                                                                                                \
        ESC(detail::Add<Element, Element>));


namespace lue {

    LUE_INSTANTIATE(std::uint8_t, 2);
    LUE_INSTANTIATE(std::uint32_t, 2);
    LUE_INSTANTIATE(std::uint64_t, 2);
    LUE_INSTANTIATE(std::int32_t, 2);
    LUE_INSTANTIATE(std::int64_t, 2);
    LUE_INSTANTIATE(float, 2);
    LUE_INSTANTIATE(double, 2);

}  // namespace lue
