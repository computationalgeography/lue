#pragma once
#include "lue/framework/algorithm/cell_index.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {
    namespace policy::cell_index {

        template<typename IndexElement, typename ConditionElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<ConditionElement>,
            OutputElements<IndexElement>,
            InputElements<ConditionElement>>;

    }  // namespace policy::cell_index


    namespace default_policies {

        template<typename IndexElement, typename ConditionElement, Rank rank>
            requires std::same_as<ConditionElement, BooleanElement>
        auto cell_index(PartitionedArray<ConditionElement, rank> const& condition, Index const dimension_idx)
            -> PartitionedArray<IndexElement, rank>
        {
            using Policies = policy::cell_index::DefaultPolicies<IndexElement, ConditionElement>;

            return cell_index(Policies{}, condition, dimension_idx);
        }

    }  // namespace default_policies
}  // namespace lue
