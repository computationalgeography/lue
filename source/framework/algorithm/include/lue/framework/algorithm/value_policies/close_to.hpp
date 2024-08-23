#pragma once
#include "lue/framework/algorithm/close_to.hpp"


namespace lue {
    namespace policy::close_to {

        template<typename OutputElement, typename InputElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<InputElement, InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement, InputElement>>;

    }  // namespace policy::close_to


    namespace value_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_DIFFERENT_OUTPUT_ELEMENT(
            close_to, policy::close_to::DefaultValuePolicies)

    }  // namespace value_policies


}  // namespace lue
