#pragma once
#include "lue/framework/algorithm/logical_not.hpp"
#include "lue/framework/algorithm/operator.hpp"


namespace lue {
    namespace policy::logical_not {

        template<typename OutputElement, typename InputElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement>>;

    }  // namespace policy::logical_not


    namespace default_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_DIFFERENT_OUTPUT_ELEMENT(
            logical_not, policy::logical_not::DefaultPolicies)

        LUE_UNARY_LOGICAL_OPERATOR(!, logical_not)

    }  // namespace default_policies
}  // namespace lue
