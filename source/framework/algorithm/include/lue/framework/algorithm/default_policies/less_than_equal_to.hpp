#pragma once
#include "lue/framework/algorithm/less_than_equal_to.hpp"


namespace lue {
    namespace policy::less_than_equal_to {

        template<typename OutputElement, typename InputElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<InputElement, InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement, InputElement>>;

    }  // namespace policy::less_than_equal_to


    namespace default_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITHOUT_POLICIES(
            less_than_equal_to, detail::LessThanEqualTo, policy::less_than_equal_to::DefaultPolicies)
        LUE_BINARY_COMPARISON_OPERATOR(<=, less_than_equal_to)

    }  // namespace default_policies

}  // namespace lue
