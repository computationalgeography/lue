#pragma once
#include "lue/framework/algorithm/exp.hpp"


namespace lue {
    namespace policy::exp {

        template<typename Element>
        class RangePolicy
        {

            public:

                constexpr static auto within_range(
                    [[maybe_unused]] Element const& argument, Element const& result) -> bool
                {
                    return !std::isinf(result);
                }
        };


        template<typename Element>
        using DefaultValuePolicies = policy::Policies<
            AllValuesWithinDomain<Element>,
            OutputsPolicies<OutputPolicies<DefaultOutputNoDataPolicy<Element>, RangePolicy<Element>>>,
            InputsPolicies<InputPolicies<DefaultInputNoDataPolicy<Element>>>>;

    }  // namespace policy::exp


    namespace value_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            exp, policy::exp::DefaultValuePolicies)

    }  // namespace value_policies
}  // namespace lue
