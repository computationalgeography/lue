#pragma once
#include "lue/framework/algorithm/logical_exclusive_or.hpp"
#include "lue/framework/algorithm/operator.hpp"


namespace lue {
    namespace policy::logical_exclusive_or {

        template<typename OutputElement, typename InputElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<InputElement, InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement, InputElement>>;

    }  // namespace policy::logical_exclusive_or


    namespace default_policies {

        LUE_BINARY_LOCAL_OPERATION_OVERLOADS2(
            logical_exclusive_or, detail::LogicalExclusiveOr, policy::logical_exclusive_or::DefaultPolicies)

    }  // namespace default_policies
}  // namespace lue
