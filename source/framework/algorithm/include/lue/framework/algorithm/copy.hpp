#pragma once
#include "lue/framework/algorithm/policy/all_values_within_domain.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"
#include "lue/framework/algorithm/policy/default_value_policies.hpp"
#include "lue/framework/core/annotate.hpp"
#include "lue/framework/core/type_traits.hpp"
#include "lue/framework/partitioned_array_decl.hpp"
#include <hpx/include/lcos.hpp>


namespace lue {
    namespace detail {

        /*!
            @brief      Return a new copy the array partition passed in
            @tparam     Partition Client class of partition component
            @param      input_partition Client of array partition component
            @return     A copy of the array partition passed in
        */
        template<typename Policies, typename Partition>
        Partition copy_partition(Policies const& policies, Partition const& input_partition)
        {
            using Offset = OffsetT<Partition>;
            using InputData = DataT<Partition>;
            using OutputData = InputData;

            lue_hpx_assert(input_partition.is_ready());

            return hpx::dataflow(
                hpx::launch::async,
                hpx::unwrapping(

                    [policies, input_partition](Offset const& offset, InputData&& input_partition_data)
                    {
                        AnnotateFunction annotation{"copy_partition"};

                        HPX_UNUSED(input_partition);

                        OutputData output_partition_data{input_partition_data.shape()};

                        auto const& indp = std::get<0>(policies.inputs_policies()).input_no_data_policy();
                        auto const& ondp = std::get<0>(policies.outputs_policies()).output_no_data_policy();

                        Count const nr_elements{lue::nr_elements(output_partition_data)};

                        for (Index i = 0; i < nr_elements; ++i)
                        {
                            if (indp.is_no_data(input_partition_data, i))
                            {
                                ondp.mark_no_data(output_partition_data, i);
                            }
                            else
                            {
                                output_partition_data[i] = input_partition_data[i];
                            }
                        }

                        return Partition{hpx::find_here(), offset, std::move(output_partition_data)};
                    }

                    ),
                input_partition.offset(),
                input_partition.data());
        }

    }  // namespace detail


    namespace policy::copy {

        template<typename Element>
        using DefaultPolicies = policy::
            DefaultPolicies<AllValuesWithinDomain<Element>, OutputElements<Element>, InputElements<Element>>;

        template<typename Element>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<Element>,
            OutputElements<Element>,
            InputElements<Element>>;

    }  // namespace policy::copy


    template<typename Policies, typename Partition>
    struct CopyPartitionAction:
        hpx::actions::make_action<
            decltype(&detail::copy_partition<Policies, Partition>),
            &detail::copy_partition<Policies, Partition>,
            CopyPartitionAction<Policies, Partition>>::type
    {
    };


    /*!
        @brief      Return the result of copying a partitioned array
        @tparam     Element Type of elements in the arrays
        @tparam     rank Rank of the input array
        @param      array Partitioned array to copy
        @return     New partitioned array
    */
    template<typename Policies, typename Element, Rank rank>
    PartitionedArray<Element, rank> copy(
        Policies const& policies, PartitionedArray<Element, rank> const& input_array)
    {
        using InputArray = PartitionedArray<Element, rank>;
        using InputPartitions = PartitionsT<InputArray>;
        using InputPartition = PartitionT<InputArray>;

        using OutputArray = PartitionedArray<Element, rank>;
        using OutputPartitions = PartitionsT<OutputArray>;

        CopyPartitionAction<Policies, InputPartition> action;
        Localities<rank> localities{input_array.localities()};
        InputPartitions const& input_partitions{input_array.partitions()};
        OutputPartitions output_partitions{shape_in_partitions(input_array)};

        for (Index p = 0; p < nr_partitions(input_array); ++p)
        {
            output_partitions[p] = hpx::dataflow(
                hpx::launch::async,

                [locality_id = localities[p], policies, action](InputPartition const& input_partition)
                {
                    AnnotateFunction annotation{"copy"};
                    return action(locality_id, policies, input_partition);
                },

                input_partitions[p]);
        }

        return OutputArray{shape(input_array), std::move(localities), std::move(output_partitions)};
    }


    template<typename Element, Rank rank>
    PartitionedArray<Element, rank> copy(PartitionedArray<Element, rank> const& input_array)
    {
        using Policies = policy::copy::DefaultPolicies<Element>;

        return copy(Policies{}, input_array);
    }

}  // namespace lue
