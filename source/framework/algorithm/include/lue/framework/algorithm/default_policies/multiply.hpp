#pragma once
#include "lue/framework/algorithm/multiply.hpp"


namespace lue {
    namespace policy::multiply {

        template<
            typename OutputElement,
            typename InputElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<InputElement, InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement, InputElement>>;

    }  // namespace multiply::policy


    namespace default_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS2(
            multiply, detail::Multiply, policy::multiply::DefaultPolicies)
        LUE_BINARY_ARITHMETIC_OPERATOR(*, multiply)

    }  // namespace default_policies

}  // namespace lue
