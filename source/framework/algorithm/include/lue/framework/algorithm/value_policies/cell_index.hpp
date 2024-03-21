#pragma once
#include "lue/framework/algorithm/cell_index.hpp"


namespace lue {
    namespace policy::cell_index {

        template<typename IndexElement, typename ConditionElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<ConditionElement>,
            OutputElements<IndexElement>,
            InputElements<ConditionElement>>;

    }  // namespace policy::cell_index


    namespace value_policies {

        template<typename IndexElement, typename ConditionElement, Rank rank>
        auto cell_index(PartitionedArray<ConditionElement, rank> const& condition, Index const dimension_idx)
            -> PartitionedArray<IndexElement, rank>
        {
            using Policies = policy::cell_index::DefaultValuePolicies<IndexElement, ConditionElement>;

            return cell_index(Policies{}, condition, dimension_idx);
        }

    }  // namespace value_policies
}  // namespace lue
