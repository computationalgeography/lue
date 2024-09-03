#pragma once
#include "lue/framework/algorithm/ceil.hpp"


namespace lue {
    namespace policy::ceil {

        template<typename Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::ceil


    namespace value_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            ceil, policy::ceil::DefaultValuePolicies)

    }  // namespace value_policies
}  // namespace lue
