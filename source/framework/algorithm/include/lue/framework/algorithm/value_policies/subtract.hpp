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

                constexpr static bool within_range(
                    InputElement const& argument1, InputElement const& argument2, OutputElement const& result)
                {
                    return subtract_within_range(argument1, argument2, result);
                }
        };


        template<typename OutputElement, typename InputElement>
        using DefaultValuePolicies = policy::Policies<
            AllValuesWithinDomain<InputElement, InputElement>,
            OutputsPolicies<OutputPolicies<
                DefaultOutputNoDataPolicy<OutputElement>,
                RangePolicy<OutputElement, InputElement>>>,
            InputsPolicies<
                InputPolicies<DefaultInputNoDataPolicy<InputElement>>,
                InputPolicies<DefaultInputNoDataPolicy<InputElement>>>>;

    }  // namespace policy::subtract


    namespace value_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES(
            subtract, detail::Subtract, policy::subtract::DefaultValuePolicies)
        LUE_BINARY_ARITHMETIC_OPERATOR(-, subtract)

    }  // namespace value_policies
}  // namespace lue
