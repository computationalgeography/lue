#pragma once
#include "lue/framework/algorithm/add.hpp"
#include "lue/framework/algorithm/operator.hpp"


namespace lue {
    namespace policy::add {

        template<typename Element>
        using DefaultPolicies = policy::
            DefaultPolicies<DomainPolicy<Element>, OutputElements<Element>, InputElements<Element, Element>>;

    }  // namespace policy::add


    namespace default_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            add, policy::add::DefaultPolicies)
        LUE_BINARY_ARITHMETIC_OPERATOR(+, add)

    }  // namespace default_policies
}  // namespace lue
