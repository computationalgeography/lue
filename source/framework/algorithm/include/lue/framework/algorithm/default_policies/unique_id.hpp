#pragma once
#include "lue/framework/algorithm/unique_id.hpp"


namespace lue {
    namespace policy::unique_id {

        template<typename IDElement, typename ConditionElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<ConditionElement>,
            OutputElements<IDElement>,
            InputElements<ConditionElement>>;

    }  // namespace policy::unique_id


    namespace default_policies {

        template<typename IDElement, typename ConditionElement, Rank rank>
        auto unique_id(PartitionedArray<ConditionElement, rank> const& array)
            -> PartitionedArray<IDElement, rank>
        {
            using Policies = policy::unique_id::DefaultPolicies<IDElement, ConditionElement>;

            return lue::unique_id<IDElement>(Policies{}, array);
        }

    }  // namespace default_policies
}  // namespace lue
