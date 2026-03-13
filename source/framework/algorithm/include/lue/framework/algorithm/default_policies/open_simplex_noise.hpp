#pragma once
#include "lue/framework/algorithm/open_simplex_noise.hpp"


namespace lue {
    namespace policy::open_simplex_noise {

        template<std::floating_point Element>
        using DefaultPolicies = policy::DefaultPolicies<
            AllValuesWithinDomain<Element, Element>,
            OutputElements<Element>,
            InputElements<Element, Element>>;

    }  // namespace policy::open_simplex_noise


    namespace default_policies {

        template<std::floating_point Element, Rank rank>
        auto open_simplex_noise(
            PartitionedArray<Element, rank> const& x_coordinates,
            PartitionedArray<Element, rank> const& y_coordinates,
            int const seed) -> PartitionedArray<Element, rank>
        {
            using Policies = policy::open_simplex_noise::DefaultPolicies<Element>;

            return lue::open_simplex_noise(Policies{}, x_coordinates, y_coordinates, seed);
        }

    }  // namespace default_policies
}  // namespace lue
