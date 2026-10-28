#pragma once
#include "lue/framework/algorithm/focal_sum.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {
    namespace policy::focal_sum {

        template<Arithmetic Element>
        using DefaultPolicies = policy::DefaultSpatialOperationPolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::focal_sum


    namespace default_policies {

        template<Arithmetic Element, typename Kernel>
            requires std::same_as<ElementT<Kernel>, BooleanElement> && (rank<Kernel> == 2)
        auto focal_sum(PartitionedArray<Element, 2> const& array, Kernel const& kernel)
            -> PartitionedArray<Element, 2>
        {
            using Policies = policy::focal_sum::DefaultPolicies<Element>;

            return focal_sum(Policies{}, array, kernel);
        }

    }  // namespace default_policies
}  // namespace lue
