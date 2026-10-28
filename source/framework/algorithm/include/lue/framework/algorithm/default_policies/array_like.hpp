#pragma once
#include "lue/framework/algorithm/array_like.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {
    namespace policy::array_like {

        template<Arithmetic OutputElement, Arithmetic InputElement>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement>>;

    }  // namespace policy::array_like


    namespace default_policies {

        template<Arithmetic OutputElement, Arithmetic InputElement, Rank rank>
        auto array_like(
            PartitionedArray<InputElement, rank> const& input_array, Scalar<OutputElement> const& fill_value)
            -> PartitionedArray<OutputElement, rank>
        {
            using Policies = policy::array_like::DefaultPolicies<OutputElement, InputElement>;

            return array_like(Policies{}, input_array, fill_value);
        }


        template<Arithmetic OutputElement, Arithmetic InputElement, Rank rank>
        auto array_like(
            PartitionedArray<InputElement, rank> const& input_array, OutputElement const fill_value)
            -> PartitionedArray<OutputElement, rank>
        {
            using Policies = policy::array_like::DefaultPolicies<OutputElement, InputElement>;

            return array_like(Policies{}, input_array, fill_value);
        }

    }  // namespace default_policies
}  // namespace lue
