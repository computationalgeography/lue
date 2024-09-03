#pragma once
#include "lue/framework/algorithm/atan.hpp"


namespace lue {
    namespace policy::atan {

        template<typename Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::atan


    namespace value_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            atan, policy::atan::DefaultValuePolicies)

    }  // namespace value_policies
}  // namespace lue
