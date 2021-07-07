#pragma once
#include "lue/framework/algorithm/add.hpp"


namespace lue {
    namespace policy::add {

        template<
            typename OutputElement,
            typename InputElement>
        using DefaultPolicies = policy::DefaultPolicies<
            DomainPolicy<InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement, InputElement>>;

    }  // namespace policy::add


    namespace default_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS2(
            add, detail::Add, policy::add::DefaultPolicies)
        LUE_BINARY_ARITHMETIC_OPERATOR(+, add)

    }  // namespace default_policies

}  // namespace lue
