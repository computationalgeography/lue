#pragma once
#include "lue/framework/algorithm/exp.hpp"


namespace lue {
    namespace policy::exp {

        template<std::floating_point Element>
        class RangePolicy
        {

            public:

                constexpr static auto within_range(
                    [[maybe_unused]] Element const& argument, Element const& result) -> bool
                {
                    return !std::isinf(result);
                }
        };


        template<std::floating_point Element>
        using DefaultValuePolicies = policy::Policies<
            AllValuesWithinDomain<Element>,
            OutputsPolicies<OutputPolicies<DefaultOutputNoDataPolicy<Element>, RangePolicy<Element>>>,
            InputsPolicies<InputPolicies<DefaultInputNoDataPolicy<Element>>>>;

    }  // namespace policy::exp


    namespace value_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            exp, policy::exp::DefaultValuePolicies, std::floating_point)

    }  // namespace value_policies
}  // namespace lue
