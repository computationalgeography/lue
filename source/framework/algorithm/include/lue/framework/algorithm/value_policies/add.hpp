#pragma once
#include "lue/framework/algorithm/add.hpp"
#include "lue/framework/algorithm/out_of_range.hpp"


namespace lue {
    namespace policy::add {

        template<typename OutputElement, typename InputElement>
        class RangePolicy
        {

            public:

                static_assert(std::is_same_v<InputElement, OutputElement>);

                constexpr static bool within_range(
                    InputElement const& argument1, InputElement const& argument2, OutputElement const& result)
                {
                    return add_within_range(argument1, argument2, result);
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

    }  // namespace policy::add


    namespace value_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            add, policy::add::DefaultValuePolicies)
        LUE_BINARY_ARITHMETIC_OPERATOR(+, add)

    }  // namespace value_policies
}  // namespace lue
