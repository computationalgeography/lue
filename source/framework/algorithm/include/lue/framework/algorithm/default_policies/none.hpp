#pragma once
#include "lue/framework/algorithm/none.hpp"


namespace lue {
    namespace policy::none {

        template<typename Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::none


    namespace default_policies {

        template<typename Element, Rank rank>
        auto none(PartitionedArray<Element, rank> const& array) -> Scalar<Element>
        {
            using Functor = detail::None<Element>;
            using Policies = policy::none::DefaultPolicies<Element>;

            return unary_aggregate_operation(Policies{}, array, Functor{});
        }

    }  // namespace default_policies
}  // namespace lue
