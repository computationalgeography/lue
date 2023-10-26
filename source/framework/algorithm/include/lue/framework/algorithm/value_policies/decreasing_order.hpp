#pragma once
#include "lue/framework/algorithm/decreasing_order.hpp"


namespace lue {
    namespace policy::decreasing_order {

        template<typename... InputElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<InputElement...>,
            OutputElements<>,
            InputElements<InputElement...>>;

    }  // namespace policy::decreasing_order


    namespace value_policies {

        template<typename ZoneElement, typename InputElement, Rank rank>
        SerialRoute<rank> decreasing_order(
            PartitionedArray<ZoneElement, rank> const& region,
            PartitionedArray<InputElement, rank> const& value,
            Count const max_length = std::numeric_limits<Count>::max())
        {
            using Policies = policy::decreasing_order::DefaultValuePolicies<ZoneElement, InputElement>;

            return decreasing_order(Policies{}, region, value, max_length);
        }


        template<typename InputElement, Rank rank>
        SerialRoute<rank> decreasing_order(
            PartitionedArray<InputElement, rank> const& value,
            Count const max_length = std::numeric_limits<Count>::max())
        {
            using Policies = policy::decreasing_order::DefaultValuePolicies<InputElement>;

            return decreasing_order(Policies{}, value, max_length);
        }

    }  // namespace value_policies
}  // namespace lue
