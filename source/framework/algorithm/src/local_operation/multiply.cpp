#include "lue/framework/algorithm/default_policies/multiply.hpp"
#include "lue/framework/algorithm/value_policies/multiply.hpp"
#include "lue/framework/algorithm/definition/multiply.hpp"


#define LUE_INSTANTIATE(Element, rank)                               \
                                                                     \
    LUE_INSTANTIATE_BINARY_LOCAL_OPERATION(                          \
        (policy::multiply::DefaultPolicies<Element, Element>),       \
        Element,                                                     \
        Element,                                                     \
        Element,                                                     \
        rank,                                                        \
        (detail::Multiply<Element, Element>));                       \
                                                                     \
    LUE_INSTANTIATE_BINARY_LOCAL_OPERATION(                          \
        (policy::multiply::DefaultValuePolicies<Element, Element>),  \
        Element,                                                     \
        Element,                                                     \
        Element,                                                     \
        rank,                                                        \
        (detail::Multiply<Element, Element>));


namespace lue {

    LUE_INSTANTIATE(std::uint8_t, 2);
    LUE_INSTANTIATE(std::uint32_t, 2);
    LUE_INSTANTIATE(std::uint64_t, 2);
    LUE_INSTANTIATE(std::int32_t, 2);
    LUE_INSTANTIATE(std::int64_t, 2);
    LUE_INSTANTIATE(float, 2);
    LUE_INSTANTIATE(double, 2);

}  // namespace lue
