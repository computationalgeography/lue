#pragma once
#include "lue/framework/algorithm/close_to.hpp"


namespace lue {
    namespace policy::close_to {

        template<std::integral OutputElement, std::floating_point InputElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<InputElement, InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement, InputElement>>;

    }  // namespace policy::close_to


    namespace default_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES_DIFFERENT_OUTPUT_ELEMENT(
            close_to, policy::close_to::DefaultPolicies, std::floating_point, std::integral)

    }  // namespace default_policies
}  // namespace lue
