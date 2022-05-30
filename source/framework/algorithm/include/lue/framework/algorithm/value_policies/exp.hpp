#pragma once
#include "lue/framework/algorithm/exp.hpp"


namespace lue {
    namespace policy::exp {

        template<
            typename Element>
        class RangePolicy
        {

            public:

                constexpr static bool within_range(
                    [[maybe_unused]] Element const& argument,
                    Element const& result)
                {
                    return !std::isinf(result);
                }

        };


        template<
            typename Element>
        using DefaultValuePolicies =
            policy::Policies<
                    AllValuesWithinDomain<Element>,
                    OutputsPolicies<
                            OutputPolicies<
                                    DefaultOutputNoDataPolicy<Element>,
                                    RangePolicy<Element>
                                >
                        >,
                    InputsPolicies<
                            InputPolicies<
                                    DefaultInputNoDataPolicy<Element>
                                >
                        >
                >;

    }  // namespace policy::exp


    namespace value_policies {

        template<
            typename Element,
            Rank rank>
        auto exp(
            PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::exp::DefaultValuePolicies<Element>;

            return exp(Policies{}, array);
        }

    }  // namespace value_policies
}  // namespace lue
