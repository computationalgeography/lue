#pragma once
#include "lue/framework/algorithm/cos.hpp"


namespace lue {
    namespace policy::cos {

        template<std::floating_point Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::cos


    namespace default_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            cos, policy::cos::DefaultPolicies, std::floating_point)

    }  // namespace default_policies
}  // namespace lue
