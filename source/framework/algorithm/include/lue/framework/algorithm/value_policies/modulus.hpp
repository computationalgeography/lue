#pragma once
#include "lue/framework/algorithm/modulus.hpp"


namespace lue {
    namespace policy::modulus {

        template<typename Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            DomainPolicy<Element>,
            OutputElements<Element>,
            InputElements<Element, Element>>;

    }  // namespace policy::modulus


    namespace value_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            modulus, policy::modulus::DefaultValuePolicies)
        LUE_BINARY_ARITHMETIC_OPERATOR(%, modulus)

    }  // namespace value_policies
}  // namespace lue
