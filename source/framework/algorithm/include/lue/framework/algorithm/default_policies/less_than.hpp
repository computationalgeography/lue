#pragma once
#include "lue/framework/algorithm/less_than.hpp"
#include "lue/framework/configure.hpp"


namespace lue {
    namespace policy::less_than {

        template<typename OutputElement, typename InputElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<InputElement, InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement, InputElement>>;

    }  // namespace policy::less_than


    namespace default_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_DIFFERENT_OUTPUT_ELEMENT(
            less_than, policy::less_than::DefaultPolicies)
        LUE_BINARY_COMPARISON_OPERATOR(<, less_than, BooleanElement)

    }  // namespace default_policies

}  // namespace lue
