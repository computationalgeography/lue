#pragma once
#include "lue/framework/algorithm/definition/inflow_count3.hpp"
#include "lue/framework/algorithm/detail/communicator.hpp"
#include "lue/framework/core/define.hpp"
#include "lue/framework.hpp"
#include <hpx/serialization.hpp>


namespace lue::detail {

    template<typename Policies, typename FlowDirectionElement, Rank rank>
    auto inflow_count3(
        Policies const& policies,
        ArrayPartition<FlowDirectionElement, rank> const& flow_direction_partition,
        InflowCountCommunicator<rank> inflow_count_communicator)
        -> hpx::tuple<
            ArrayPartition<SmallestIntegralElement, rank>,
            hpx::shared_future<std::array<std::vector<std::array<Index, rank>>, nr_neighbours<rank>()>>,
            hpx::future<std::array<std::vector<std::array<Index, rank>>, nr_neighbours<rank>()>>>
    {
        // As long as we only use flow direction, external inflow and threshold to detect
        // no-data in input, there is no need to mark no-data in the output of inflow_count. We
        // won't be reading these cells anyway.

        using CountElement = SmallestIntegralElement;
        using InflowCountOutputPolicies = policy::
            OutputPolicies<policy::DontMarkNoData<CountElement>, policy::AllValuesWithinRange<CountElement>>;
        InflowCountOutputPolicies inflow_count_output_policies{};

        using FlowDirectionInputPolicies = policy::InputPoliciesT<Policies, 0>;
        FlowDirectionInputPolicies flow_direction_input_policies{std::get<0>(policies.inputs_policies())};

        using InflowCountPolicies = policy::Policies<
            policy::AllValuesWithinDomain<FlowDirectionElement>,
            policy::OutputsPolicies<InflowCountOutputPolicies>,
            policy::InputsPolicies<FlowDirectionInputPolicies>>;

        InflowCountPolicies inflow_count_policies{
            policy::AllValuesWithinDomain<FlowDirectionElement>{},
            inflow_count_output_policies,
            flow_direction_input_policies};

        return inflow_count3_action<CountElement>(
            inflow_count_policies, flow_direction_partition, std::move(inflow_count_communicator));
    }

}  // namespace lue::detail
