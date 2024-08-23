#pragma once
#include "lue/framework/algorithm/less_than.hpp"


namespace lue {
    namespace policy::less_than {

        template<typename OutputElement, typename InputElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<InputElement, InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement, InputElement>>;

    }  // namespace policy::less_than


    namespace value_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_DIFFERENT_OUTPUT_ELEMENT(
            less_than, policy::less_than::DefaultValuePolicies)
        LUE_BINARY_COMPARISON_OPERATOR(<, less_than)

    }  // namespace value_policies

}  // namespace lue
