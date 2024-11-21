#pragma once
#include "lue/framework/algorithm/unique_id.hpp"


namespace lue {
    namespace policy::unique_id {

        // TODO IDElement can be too small for storing all unique IDs. A range policy must test
        //      for that. We can reuse the out of range policy from addition. The unique_id
        //      algorithm adds 1 to a running sum. The policy can check whether the original value
        //      isn't the largest value that can be represented by the IDElement type.
        template<typename IDElement, typename ConditionElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<ConditionElement>,
            OutputElements<IDElement>,
            InputElements<ConditionElement>>;

    }  // namespace policy::unique_id


    namespace value_policies {

        template<typename IDElement, typename ConditionElement, Rank rank>
        auto unique_id(PartitionedArray<ConditionElement, rank> const& array)
            -> PartitionedArray<IDElement, rank>
        {
            using Policies = policy::unique_id::DefaultValuePolicies<IDElement, ConditionElement>;

            return lue::unique_id<IDElement>(Policies{}, array);
        }

    }  // namespace value_policies
}  // namespace lue
