#pragma once
#include "lue/framework/algorithm/logical_not.hpp"
#include "lue/framework/algorithm/operator.hpp"
#include "lue/framework/configure.hpp"


namespace lue {
    namespace policy::logical_not {

        template<std::integral OutputElement, std::integral InputElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement>>;

    }  // namespace policy::logical_not


    namespace value_policies {

        LUE_UNARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_DIFFERENT_OUTPUT_ELEMENT(
            logical_not, policy::logical_not::DefaultValuePolicies, std::integral, std::integral)

        LUE_UNARY_LOGICAL_OPERATOR(!, logical_not, BooleanElement)

    }  // namespace value_policies
}  // namespace lue
