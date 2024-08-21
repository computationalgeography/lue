#pragma once
#include "lue/framework/algorithm/abs.hpp"


namespace lue {
    namespace policy::abs {

        template<typename Element>
        class RangePolicy
        {

            public:

                constexpr static bool within_range(
                    Element const& argument, [[maybe_unused]] Element const& result)
                {
                    if constexpr (std::is_signed_v<Element>)
                    {
                        // In 2's complement systems, the absolute value of the smallest integer
                        // value cannot be represented: abs(min_int) == max_int + 1
                        return argument != std::numeric_limits<Element>::lowest();
                    }
                    else
                    {
                        return true;
                    }
                }
        };


        template<typename Element>
        using DefaultValuePolicies = policy::Policies<
            AllValuesWithinDomain<Element>,
            OutputsPolicies<OutputPolicies<DefaultOutputNoDataPolicy<Element>, RangePolicy<Element>>>,
            InputsPolicies<InputPolicies<DefaultInputNoDataPolicy<Element>>>>;

    }  // namespace policy::abs


    namespace value_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            abs, policy::abs::DefaultValuePolicies)

    }  // namespace value_policies
}  // namespace lue
