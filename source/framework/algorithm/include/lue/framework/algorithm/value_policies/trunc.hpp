#pragma once
#include "lue/framework/algorithm/trunc.hpp"


namespace lue {
    namespace policy::trunc {

        template<typename Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::trunc


    namespace value_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            trunc, policy::trunc::DefaultValuePolicies)

    }  // namespace value_policies
}  // namespace lue
