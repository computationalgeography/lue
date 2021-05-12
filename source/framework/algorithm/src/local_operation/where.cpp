#include "lue/framework/algorithm/definition/where.hpp"


#define LUE_INSTANTIATE_BINARY(ConditionElement, ExpressionElement, rank)                               \
                                                                                                        \
    LUE_INSTANTIATE_BINARY_LOCAL_OPERATION(                                                             \
        (policy::where::DefaultValuePolicies<ExpressionElement, ConditionElement, ExpressionElement>),  \
        ExpressionElement,                                                                              \
        ConditionElement,                                                                               \
        ExpressionElement,                                                                              \
        rank,                                                                                           \
        (detail::Where<ConditionElement, ExpressionElement>));


namespace lue {

    LUE_INSTANTIATE_BINARY(std::uint8_t, std::uint8_t, 2);
    LUE_INSTANTIATE_BINARY(std::uint8_t, std::uint32_t, 2);
    LUE_INSTANTIATE_BINARY(std::uint8_t, std::uint64_t, 2);
    LUE_INSTANTIATE_BINARY(std::uint8_t, std::int32_t, 2);
    LUE_INSTANTIATE_BINARY(std::uint8_t, std::int64_t, 2);
    LUE_INSTANTIATE_BINARY(std::uint8_t, float, 2);
    LUE_INSTANTIATE_BINARY(std::uint8_t, double, 2);

}  // namespace lue
