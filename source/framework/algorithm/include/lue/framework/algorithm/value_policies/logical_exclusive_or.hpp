#pragma once
#include "lue/framework/algorithm/logical_exclusive_or.hpp"
#include "lue/framework/algorithm/operator.hpp"


namespace lue {
    namespace policy::logical_exclusive_or {

        template<std::integral OutputElement, std::integral InputElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<InputElement, InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement, InputElement>>;

    }  // namespace policy::logical_exclusive_or


    namespace value_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_DIFFERENT_OUTPUT_ELEMENT(
            logical_exclusive_or,
            policy::logical_exclusive_or::DefaultValuePolicies,
            std::integral,
            std::integral)

    }  // namespace value_policies
}  // namespace lue
