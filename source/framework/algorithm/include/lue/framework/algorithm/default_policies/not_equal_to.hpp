#pragma once
#include "lue/framework/algorithm/not_equal_to.hpp"
#include "lue/framework/configure.hpp"


namespace lue {
    namespace policy::not_equal_to {

        template<typename OutputElement, typename InputElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<InputElement, InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement, InputElement>>;

    }  // namespace policy::not_equal_to


    namespace default_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_DIFFERENT_OUTPUT_ELEMENT(
            not_equal_to, policy::not_equal_to::DefaultPolicies)
        LUE_BINARY_COMPARISON_OPERATOR(!=, not_equal_to, BooleanElement)

    }  // namespace default_policies
}  // namespace lue
