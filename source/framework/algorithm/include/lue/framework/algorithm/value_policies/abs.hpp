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

        template<typename Element, Rank rank>
        auto abs(PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::abs::DefaultValuePolicies<Element>;

            return abs(Policies{}, array);
        }

    }  // namespace value_policies
}  // namespace lue
