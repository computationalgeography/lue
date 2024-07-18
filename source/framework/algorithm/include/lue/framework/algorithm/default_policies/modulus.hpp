#pragma once
#include "lue/framework/algorithm/modulus.hpp"


namespace lue {
    namespace policy::modulus {

        template<typename OutputElement, typename InputElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<InputElement, InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement, InputElement>>;

    }  // namespace policy::modulus


    namespace default_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS2(modulus, detail::Modulus, policy::modulus::DefaultPolicies)
        LUE_BINARY_ARITHMETIC_OPERATOR(%, modulus)

    }  // namespace default_policies

}  // namespace lue
