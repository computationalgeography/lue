#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/modulus.hpp"


namespace lue {
    namespace policy::modulus {

        template<typename Element>
        using DefaultPolicies = policy::DefaultPolicies<
            DomainPolicy<Element>,
            OutputElements<Element>,
            InputElements<Element, Element>>;

    }  // namespace policy::modulus


    namespace default_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(modulus, policy::modulus::DefaultPolicies)
        LUE_BINARY_ARITHMETIC_OPERATOR(%, modulus)

    }  // namespace default_policies

}  // namespace lue
