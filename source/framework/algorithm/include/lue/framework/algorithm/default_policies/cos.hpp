#pragma once
#include "lue/framework/algorithm/cos.hpp"


namespace lue {
    namespace policy::cos {

        template<typename Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::cos


    namespace default_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            cos, policy::cos::DefaultPolicies)

    }  // namespace default_policies
}  // namespace lue
