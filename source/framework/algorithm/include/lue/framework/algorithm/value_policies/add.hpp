#pragma once
#include "lue/framework/algorithm/add.hpp"


namespace lue {
    namespace policy::add {

        template<
            typename OutputElement,
            typename InputElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            DomainPolicy<InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement, InputElement>>;

    }  // namespace policy::add


    namespace value_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS2(
            add, detail::Add, policy::add::DefaultValuePolicies)
        LUE_BINARY_ARITHMETIC_OPERATOR(+, add)

    }  // namespace value_policies
}  // namespace lue
