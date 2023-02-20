#pragma once
#include "lue/framework/algorithm/none.hpp"


namespace lue {
    namespace policy::none {

        template<typename Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::none


    namespace value_policies {

        template<typename Element, Rank rank>
        hpx::future<Element> none(PartitionedArray<Element, rank> const& array)
        {
            using Functor = detail::None<Element>;
            using Policies = policy::none::DefaultValuePolicies<Element>;

            return unary_aggregate_operation(Policies{}, array, Functor{});
        }

    }  // namespace value_policies
}  // namespace lue
