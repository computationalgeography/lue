#pragma once
#include "lue/framework/algorithm/cast.hpp"


namespace lue {
    namespace policy::cast {

        template<typename OutputElement, typename InputElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement>>;

    }  // namespace policy::cast


    namespace default_policies {

        template<typename OutputElement, typename InputElement, Rank rank>
        PartitionedArray<OutputElement, rank> cast(PartitionedArray<InputElement, rank> const& array)
        {
            using Policies = policy::cast::DefaultPolicies<OutputElement, InputElement>;

            return lue::cast<OutputElement>(Policies{}, array);
        }

    }  // namespace default_policies
}  // namespace lue
