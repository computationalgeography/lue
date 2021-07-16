#include "lue/framework/algorithm/default_policies/where.hpp"
#include "lue/framework/algorithm/definition/where.hpp"


#define LUE_INSTANTIATE(ConditionElement, ExpressionElement, rank)                        \
                                                                                          \
    LUE_INSTANTIATE_WHERE2(                                                               \
        (policy::where::DefaultPolicies<                                                  \
            ExpressionElement, ConditionElement, ExpressionElement>),                     \
        ConditionElement,                                                                 \
        ExpressionElement,                                                                \
        rank);                                                                            \
                                                                                          \
    LUE_INSTANTIATE_WHERE3(                                                               \
        (policy::where::DefaultPolicies<                                                  \
            ExpressionElement, ConditionElement, ExpressionElement, ExpressionElement>),  \
        ConditionElement,                                                                 \
        ExpressionElement,                                                                \
        rank);


namespace lue {

    LUE_INSTANTIATE(std::uint8_t, std::uint8_t, 2);
    LUE_INSTANTIATE(std::uint8_t, std::uint32_t, 2);
    LUE_INSTANTIATE(std::uint8_t, std::uint64_t, 2);
    LUE_INSTANTIATE(std::uint8_t, std::int32_t, 2);
    LUE_INSTANTIATE(std::uint8_t, std::int64_t, 2);
    LUE_INSTANTIATE(std::uint8_t, float, 2);
    LUE_INSTANTIATE(std::uint8_t, double, 2);

}  // namespace lue
