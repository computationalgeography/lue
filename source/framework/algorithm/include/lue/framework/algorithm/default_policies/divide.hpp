#pragma once
#include "lue/framework/algorithm/divide.hpp"


namespace lue {
    namespace policy::divide {

        template<Arithmetic Element>
        using DefaultPolicies = policy::
            DefaultPolicies<DomainPolicy<Element>, OutputElements<Element>, InputElements<Element, Element>>;

    }  // namespace policy::divide


    namespace default_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            divide, policy::divide::DefaultPolicies)
        LUE_BINARY_ARITHMETIC_OPERATOR(/, divide)

    }  // namespace default_policies
}  // namespace lue
