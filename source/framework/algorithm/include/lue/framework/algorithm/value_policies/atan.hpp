#pragma once
#include "lue/framework/algorithm/atan.hpp"


namespace lue {
    namespace policy::atan {

        template<std::floating_point Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::atan


    namespace value_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            atan, policy::atan::DefaultValuePolicies, std::floating_point)

    }  // namespace value_policies
}  // namespace lue
