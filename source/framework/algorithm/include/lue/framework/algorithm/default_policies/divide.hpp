#pragma once
#include "lue/framework/algorithm/divide.hpp"


namespace lue {
    namespace policy::divide {

        template<typename OutputElement, typename InputElement>
        using DefaultPolicies = policy::DefaultPolicies<
            DomainPolicy<InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement, InputElement>>;

    }  // namespace policy::divide


    namespace default_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES(
            divide, detail::Divide, policy::divide::DefaultPolicies)
        LUE_BINARY_ARITHMETIC_OPERATOR(/, divide)

    }  // namespace default_policies
}  // namespace lue
