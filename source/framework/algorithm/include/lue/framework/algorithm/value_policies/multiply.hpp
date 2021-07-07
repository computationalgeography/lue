#pragma once
#include "lue/framework/algorithm/multiply.hpp"


namespace lue {
    namespace policy::multiply {

        template<
            typename OutputElement,
            typename InputElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<InputElement, InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement, InputElement>>;

    }  // namespace multiply::policy


    namespace value_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS2(
            multiply, detail::Multiply, policy::multiply::DefaultValuePolicies)
        LUE_BINARY_ARITHMETIC_OPERATOR(*, multiply)

    }  // namespace value_policies
}  // namespace lue
