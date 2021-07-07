#pragma once
#include "lue/framework/algorithm/less_than_equal_to.hpp"


namespace lue {
    namespace policy::less_than_equal_to {

        template<
            typename OutputElement,
            typename InputElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<InputElement, InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement, InputElement>>;

    }  // namespace policy::less_than_equal_to


    namespace value_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS2(
            less_than_equal_to, detail::LessThanEqualTo,
            policy::less_than_equal_to::DefaultValuePolicies)
        LUE_BINARY_COMPARISON_OPERATOR(<=, less_than_equal_to)

    }  // namespace value_policies

}  // namespace lue
