#pragma once
#include "lue/framework/algorithm/subtract.hpp"


namespace lue {
    namespace policy::subtract {

        template<
            typename OutputElement,
            typename InputElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<InputElement, InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement, InputElement>>;

    }  // namespace policy::subtract


    namespace value_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS2(
            subtract, detail::Subtract, policy::subtract::DefaultValuePolicies)
        LUE_BINARY_ARITHMETIC_OPERATOR(-, subtract)

    }  // namespace value_policies
}  // namespace lue
