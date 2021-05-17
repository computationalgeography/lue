#include "lue/framework/algorithm/definition/valid.hpp"


#define LUE_INSTANTIATE(BooleanElement, ExpressionElement)                         \
                                                                                   \
    LUE_INSTANTIATE_VALID(                                                         \
        (policy::valid::DefaultValuePolicies<BooleanElement, ExpressionElement>),  \
        BooleanElement,                                                            \
        ExpressionElement,                                                         \
        2);


namespace lue {

    LUE_INSTANTIATE(std::uint8_t, std::uint8_t);
    LUE_INSTANTIATE(std::uint8_t, std::uint32_t);
    LUE_INSTANTIATE(std::uint8_t, std::uint64_t);
    LUE_INSTANTIATE(std::uint8_t, std::int32_t);
    LUE_INSTANTIATE(std::uint8_t, std::int64_t);
    LUE_INSTANTIATE(std::uint8_t, float);
    LUE_INSTANTIATE(std::uint8_t, double);

}  // namespace lue
