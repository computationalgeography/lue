#pragma once
#include "lue/framework/algorithm/floor.hpp"


namespace lue {
    namespace policy::floor {

        template<std::floating_point Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::floor


    namespace value_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_SAME_OUTPUT_ELEMENT(
            floor, policy::floor::DefaultValuePolicies, std::floating_point)

    }  // namespace value_policies
}  // namespace lue
