#pragma once
#include "lue/framework/algorithm/round.hpp"


namespace lue {
    namespace policy::round {

        template<std::floating_point Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::round


    namespace value_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            round, policy::round::DefaultValuePolicies, std::floating_point)

    }  // namespace value_policies
}  // namespace lue
