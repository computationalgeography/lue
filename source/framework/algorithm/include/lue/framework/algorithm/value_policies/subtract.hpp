#pragma once
#include "lue/framework/algorithm/out_of_range.hpp"
#include "lue/framework/algorithm/subtract.hpp"


namespace lue {
    namespace policy::subtract {

        template<typename OutputElement, typename InputElement>
        class RangePolicy
        {

            public:

                static_assert(std::is_same_v<InputElement, OutputElement>);

                constexpr static auto within_range(
                    InputElement const& argument1, InputElement const& argument2, OutputElement const& result)
                    -> bool
                {
                    return subtract_within_range(argument1, argument2, result);
                }
        };


        template<typename Element>
        using DefaultValuePolicies = policy::Policies<
            AllValuesWithinDomain<Element, Element>,
            OutputsPolicies<
                OutputPolicies<DefaultOutputNoDataPolicy<Element>, RangePolicy<Element, Element>>>,
            InputsPolicies<
                InputPolicies<DefaultInputNoDataPolicy<Element>>,
                InputPolicies<DefaultInputNoDataPolicy<Element>>>>;

    }  // namespace policy::subtract


    namespace value_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            subtract, policy::subtract::DefaultValuePolicies)
        LUE_BINARY_ARITHMETIC_OPERATOR(-, subtract)

    }  // namespace value_policies
}  // namespace lue
