#pragma once
#include "lue/framework/algorithm/less_than.hpp"
#include "lue/framework/configure.hpp"


namespace lue {
    namespace policy::less_than {

        template<std::integral OutputElement, Arithmetic InputElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<InputElement, InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement, InputElement>>;

    }  // namespace policy::less_than


    namespace value_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_DIFFERENT_OUTPUT_ELEMENT(
            less_than, policy::less_than::DefaultValuePolicies, Arithmetic, std::integral)
        LUE_BINARY_COMPARISON_OPERATOR(<, less_than, BooleanElement)

    }  // namespace value_policies

}  // namespace lue
