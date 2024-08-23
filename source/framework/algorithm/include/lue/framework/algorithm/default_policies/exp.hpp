#pragma once
#include "lue/framework/algorithm/exp.hpp"


namespace lue {
    namespace policy::exp {

        template<typename Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::exp


    namespace default_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            exp, policy::exp::DefaultPolicies)

    }  // namespace default_policies
}  // namespace lue
