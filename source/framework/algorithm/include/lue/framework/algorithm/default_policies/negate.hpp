#pragma once
#include "lue/framework/algorithm/negate.hpp"
#include "lue/framework/algorithm/operator.hpp"


namespace lue {
    namespace policy::negate {

        template<typename Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;

    }  // namespace policy::negate


    namespace default_policies {

        template<typename Element, Rank rank>
        auto negate(PartitionedArray<Element, rank> const& array)
        {
            using Policies = policy::negate::DefaultPolicies<Element>;

            return negate(Policies{}, array);
        }

        LUE_UNARY_LOGICAL_OPERATOR(-, negate)

    }  // namespace default_policies
}  // namespace lue
