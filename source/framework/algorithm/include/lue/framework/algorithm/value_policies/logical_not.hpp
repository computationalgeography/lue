#pragma once
#include "lue/framework/algorithm/logical_not.hpp"
#include "lue/framework/algorithm/operator.hpp"


namespace lue {
    namespace policy::logical_not {

        template<typename OutputElement, typename InputElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement>>;

    }  // namespace policy::logical_not


    namespace value_policies {

        template<typename Element, Rank rank>
        PartitionedArray<std::uint8_t, rank> logical_not(PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::logical_not::DefaultValuePolicies<std::uint8_t, Element>;

            return logical_not(Policies{}, array);
        }


        LUE_UNARY_LOGICAL_OPERATOR(!, logical_not)

    }  // namespace value_policies
}  // namespace lue
