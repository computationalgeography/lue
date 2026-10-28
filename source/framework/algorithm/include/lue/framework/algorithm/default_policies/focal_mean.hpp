#pragma once
#include "lue/framework/algorithm/focal_mean.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {
    namespace policy::focal_mean {

        template<typename Element>
        using DefaultPolicies = policy::DefaultSpatialOperationPolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::focal_mean


    namespace default_policies {

        template<std::floating_point Element, typename Kernel>
            requires std::same_as<ElementT<Kernel>, BooleanElement> && (rank<Kernel> == 2)
        auto focal_mean(PartitionedArray<Element, 2> const& array, Kernel const& kernel)
            -> PartitionedArray<Element, 2>
        {
            using Policies = policy::focal_mean::DefaultPolicies<Element>;

            Element const fill_value{0};

            return focal_mean(Policies{fill_value}, array, kernel);
        }

    }  // namespace default_policies
}  // namespace lue
