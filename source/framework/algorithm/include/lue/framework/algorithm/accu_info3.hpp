#pragma once
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {

    using CellClass = std::uint8_t;
    static constexpr CellClass intra_partition_stream_cell = 11;
    static constexpr CellClass ridge_cell = 12;

    static constexpr CellClass inter_partition_stream_cell = 21;
    static constexpr CellClass partition_input_cell = 13;
    static constexpr CellClass partition_output_cell = 14;
    static constexpr CellClass undefined_cell = 55;


    namespace policy::accu_info3 {

        template<typename FlowDirectionElement>
        using DomainPolicy = AllValuesWithinDomain<FlowDirectionElement>;


        template<typename FlowDirectionElement>
        using DefaultPolicies = policy::DefaultPolicies<
            DomainPolicy<FlowDirectionElement>,
            OutputElements<CellClass>,
            InputElements<FlowDirectionElement>>;


        template<typename FlowDirectionElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            DomainPolicy<FlowDirectionElement>,
            OutputElements<CellClass>,
            InputElements<FlowDirectionElement>>;

    }  // namespace policy::accu_info3


    template<typename Policies, typename FlowDirectionElement, Rank rank>
    PartitionedArray<CellClass, rank> accu_info3(
        Policies const& policies, PartitionedArray<FlowDirectionElement, rank> const& flow_direction);

}  // namespace lue
