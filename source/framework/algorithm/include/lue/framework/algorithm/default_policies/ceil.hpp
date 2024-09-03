#pragma once
#include "lue/framework/algorithm/ceil.hpp"


namespace lue {
    namespace policy::ceil {

        template<typename Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::ceil


    namespace default_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            ceil, policy::ceil::DefaultPolicies)

    }  // namespace default_policies
}  // namespace lue
