#pragma once
#include "lue/framework/algorithm/acos.hpp"


namespace lue {
    namespace policy::acos {

        template<std::floating_point Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::acos


    namespace default_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            acos, policy::acos::DefaultPolicies, std::floating_point)

    }  // namespace default_policies
}  // namespace lue
