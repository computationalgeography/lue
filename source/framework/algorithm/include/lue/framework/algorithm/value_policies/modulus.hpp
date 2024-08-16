#pragma once
#include "lue/framework/algorithm/modulus.hpp"

namespace lue {
    namespace policy::modulus {

        template<typename OutputElement, typename InputElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            DomainPolicy<InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement, InputElement>>;

    }  // namespace policy::modulus


    namespace value_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS2(modulus, detail::Modulus, policy::modulus::DefaultValuePolicies)
        LUE_BINARY_ARITHMETIC_OPERATOR(%, modulus)

    }  // namespace value_policies
}  // namespace lue
