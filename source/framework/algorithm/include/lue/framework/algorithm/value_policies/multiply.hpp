#pragma once
#include "lue/framework/algorithm/multiply.hpp"
#include "lue/framework/algorithm/out_of_range.hpp"


namespace lue {
    namespace policy::multiply {

        template<typename OutputElement, typename InputElement>
        class RangePolicy
        {

            public:

                static_assert(std::is_same_v<InputElement, OutputElement>);

                constexpr static auto within_range(
                    InputElement const& argument1, InputElement const& argument2, OutputElement const& result)
                    -> bool
                {
                    return multiply_within_range(argument1, argument2, result);
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

    }  // namespace policy::multiply


    namespace value_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            multiply, policy::multiply::DefaultValuePolicies)
        LUE_BINARY_ARITHMETIC_OPERATOR(*, multiply)

    }  // namespace value_policies
}  // namespace lue
