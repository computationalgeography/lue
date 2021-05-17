#pragma once
#include "lue/framework/algorithm/operator.hpp"
#include "lue/framework/algorithm/policy/all_values_within_domain.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"
#include "lue/framework/algorithm/policy/default_value_policies.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {
    namespace policy::logical_not {

        template<
            typename OutputElement,
            typename InputElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement>>;

        template<
            typename OutputElement,
            typename InputElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement>>;

    }  // namespace policy::logical_not


    template<
        typename InputElement,
        Rank rank>
    PartitionedArray<std::uint8_t, rank> logical_not(
        PartitionedArray<InputElement, rank> const& array);


    LUE_UNARY_LOGICAL_OPERATOR(!, logical_not)

}  // namespace lue
