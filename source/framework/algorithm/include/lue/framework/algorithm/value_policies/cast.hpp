#pragma once
#include "lue/framework/algorithm/cast.hpp"


namespace lue {
    namespace policy::cast {

        template<
            typename OutputElement,
            typename InputElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement>>;

    }  // namespace policy::cast


    namespace value_policies {

        template<
            typename OutputElement,
            typename InputElement,
            Rank rank>
        PartitionedArray<OutputElement, rank> cast(
            PartitionedArray<InputElement, rank> const& array)
        {
            using Policies = policy::cast::DefaultValuePolicies<OutputElement, InputElement>;

            return lue::cast<OutputElement>(Policies{}, array);
        }

    }  // namespace value_policies
}  // namespace lue
