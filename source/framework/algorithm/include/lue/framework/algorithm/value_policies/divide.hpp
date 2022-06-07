#pragma once
#include "lue/framework/algorithm/divide.hpp"
#include "lue/framework/algorithm/out_of_range.hpp"


namespace lue {
    namespace policy::divide {

        template<
            typename OutputElement,
            typename InputElement>
        class RangePolicy
        {

            public:

                static_assert(std::is_same_v<InputElement, OutputElement>);

                constexpr static bool within_range(
                    InputElement const& argument1,
                    InputElement const& argument2,
                    OutputElement const& result)
                {
                    return divide_within_range(argument1, argument2, result);
                }

        };


        template<
            typename OutputElement,
            typename InputElement>
        using DefaultValuePolicies =
            policy::Policies<
                    DomainPolicy<InputElement>,
                    OutputsPolicies<
                            OutputPolicies<
                                    DefaultOutputNoDataPolicy<OutputElement>,
                                    RangePolicy<OutputElement, InputElement>
                                >
                        >,
                    InputsPolicies<
                            InputPolicies<
                                    DefaultInputNoDataPolicy<InputElement>
                                >,
                            InputPolicies<
                                    DefaultInputNoDataPolicy<InputElement>
                                >
                        >
                >;

    }  // namespace divide::policy


    namespace value_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS2(
            divide, detail::Divide, policy::divide::DefaultValuePolicies)
        LUE_BINARY_ARITHMETIC_OPERATOR(/, divide)

    }  // namespace value_policies
}  // namespace lue
