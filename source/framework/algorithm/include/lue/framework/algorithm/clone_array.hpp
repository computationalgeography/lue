#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue {

    namespace policy {

        // Move to policy/policy_traits.hpp
        template<
            // All policies related to an operation
            typename Policies,
            // Idx of policies related to a specific output
            std::size_t idx = 0>
        using OutputElementT = ElementT<PoliciesT<typename Policies::OutputsPolicies, idx>>;
    }  // namespace policy


    namespace policy::clone_array {

        template<typename OutputElement>
        using DefaultPolicies =
            policy::DefaultPolicies<AllValuesWithinDomain<>, OutputElements<OutputElement>, InputElements<>>;

        template<typename OutputElement>
        using DefaultValuePolicies = policy::
            DefaultValuePolicies<AllValuesWithinDomain<>, OutputElements<OutputElement>, InputElements<>>;

    }  // namespace policy::clone_array


    namespace detail {

        // TODO hier verder

        // template<typename Policies, typename Partitions>
        // std::tuple<
        //     ShapeT<PartitionedArray<OutputElementT<Partitions>, rank<Partitions>>>,
        //     Localities<rank<Partitions>>,
        //     PartitionsT<PartitionedArray<OutputElementT<Partitions>, rank<Partitions>>>>
        // instantiate_partitions(
        //     Policies const& policies,
        //     Partitions const& input_partitions)
        // {
        //     using OutputElement = OutputElementT<Partitions>;
        //     using OutputArray = PartitionedArray<OutputElement, rank<Partitions>>;
        //     using OutputPartitions = PartitionsT<OutputArray>;
        //     using Shape = ShapeT<OutputArray>;

        //     // auto partition_shapes = lue::partition_shapes(array_shape, partition_shape);
        //     Shape const shape_in_partitions{input_partitions.shape()};

        //     Partitions output_partitions{shape_in_partitions};
        //     Count const nr_partitions = lue::nr_elements(shape_in_partitions);
        //     lue_hpx_assert(output_partitions.nr_elements() == nr_partitions);

        //     // Iterate over each input partition and spawn a task that will create a new array partition,
        //     // on the same locality as the original, and with the same shape.


        //     for(auto const& input_partition: input_partitions)
        //     {
        //     }

        //     return {array_shape, localities, std::move(input_partitions)};
        // }


        template<typename Policies, typename Array>
        PartitionedArray<policy::OutputElementT<Policies>, lue::rank<Array>> clone_array(
            Policies const& policies, Array const& partitions)
        {
            // Rank const rank{lue::rank<Array>};

            // using OutputElement = OutputElementT<Policies>;
            // using OutputArray = PartitionedArray<OutputElement, lue::rank<Array>>;

            // auto [array_shape, localities, cloned_partitions] = detail::instantiate_partitions(policies,
            // partitions);

            // return OutputArray{array_shape, localities, std::move(cloned_partitions)};
        }

    }  // namespace detail


    template<typename Policies, typename Array>
    PartitionedArray<policy::OutputElementT<Policies>, lue::rank<Array>> clone_array(
        Policies const& policies, Array const& array)
    {
        return detail::clone_array(Policies{}, array);
    }


    template<typename Element, typename Array>
    PartitionedArray<Element, lue::rank<Array>> clone_array(Array const& array)
    {
        using Policies = policy::clone_array::DefaultPolicies<Element>;

        return clone_array(Policies{}, array);
    }

}  // namespace lue
