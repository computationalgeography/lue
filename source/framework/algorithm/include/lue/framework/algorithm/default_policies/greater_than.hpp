#pragma once
#include "lue/framework/algorithm/greater_than.hpp"


namespace lue {
    namespace policy::greater_than {

        template<typename OutputElement, typename InputElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<InputElement, InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement, InputElement>>;

    }  // namespace policy::greater_than


    namespace default_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS2(
            greater_than, detail::GreaterThan, policy::greater_than::DefaultPolicies)
        LUE_BINARY_COMPARISON_OPERATOR(>, greater_than)

    }  // namespace default_policies

}  // namespace lue
