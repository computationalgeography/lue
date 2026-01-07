#pragma once
#include "lue/framework/algorithm/divide.hpp"
#include "lue/framework/algorithm/out_of_range.hpp"


namespace lue {
    namespace policy::divide {

        template<Arithmetic OutputElement, Arithmetic InputElement>
        class RangePolicy
        {

            public:

                static_assert(std::is_same_v<InputElement, OutputElement>);

                constexpr static auto within_range(
                    InputElement const& argument1, InputElement const& argument2, OutputElement const& result)
                    -> bool
                {
                    return divide_within_range(argument1, argument2, result);
                }
        };


        template<Arithmetic Element>
        using DefaultValuePolicies = policy::Policies<
            DomainPolicy<Element>,
            OutputsPolicies<
                OutputPolicies<DefaultOutputNoDataPolicy<Element>, RangePolicy<Element, Element>>>,
            InputsPolicies<
                InputPolicies<DefaultInputNoDataPolicy<Element>>,
                InputPolicies<DefaultInputNoDataPolicy<Element>>>>;

    }  // namespace policy::divide


    namespace value_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            divide, policy::divide::DefaultValuePolicies)
        LUE_BINARY_ARITHMETIC_OPERATOR(/, divide)

    }  // namespace value_policies
}  // namespace lue
