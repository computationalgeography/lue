#pragma once
#include "lue/framework/algorithm/accu.hpp"
#include "lue/framework/algorithm/definition/accumulating_router.hpp"
#include "lue/framework/algorithm/detail/verify_compatible.hpp"
#include "lue/framework/algorithm/routing_operation_export.hpp"
#include "lue/macro.hpp"


namespace lue {

    template<typename Policies>
    class Accu:
        public AccumulatingRouterFunctor<
            policy::detail::TypeList<policy::InputElementT<Policies, 1>>,
            policy::detail::TypeList<policy::OutputElementT<Policies, 0>>>
    {

        private:

            using Base = AccumulatingRouterFunctor<
                policy::detail::TypeList<policy::InputElementT<Policies, 1>>,
                policy::detail::TypeList<policy::OutputElementT<Policies, 0>>>;

        public:

            template<typename Material>
            class CellAccumulator
            {

                public:

                    using DomainPolicy = policy::DomainPolicyT<Policies>;
                    using InflowNoDataPolicy =
                        policy::InputNoDataPolicy2T<policy::InputPoliciesT<Policies, 1>>;
                    using OutflowNoDataPolicy =
                        policy::OutputNoDataPolicy2T<policy::OutputPoliciesT<Policies, 0>>;

                    static_assert(std::is_same_v<ElementT<Material>, policy::InputElementT<Policies, 1>>);

                    using MaterialElement = policy::ElementT<InflowNoDataPolicy>;

                    static_assert(std::is_same_v<policy::ElementT<InflowNoDataPolicy>, MaterialElement>);
                    static_assert(std::is_same_v<policy::ElementT<OutflowNoDataPolicy>, MaterialElement>);

                    using MaterialData = DataT<PartitionedArray<MaterialElement, 2>>;


                    CellAccumulator(
                        Policies const& policies, Material const& external_inflow, MaterialData& outflow):

                        _dp{policies.domain_policy()},
                        _indp_inflow{std::get<1>(policies.inputs_policies()).input_no_data_policy()},
                        _ondp_outflow{std::get<0>(policies.outputs_policies()).output_no_data_policy()},

                        _external_inflow{external_inflow},
                        _outflow{outflow}

                    {
                    }


                    void enter_at_ridge([[maybe_unused]] Index const idx0, [[maybe_unused]] Index const idx1)
                    {
                    }


                    void enter_at_partition_input(
                        [[maybe_unused]] Index const idx0, [[maybe_unused]] Index const idx1)
                    {
                    }


                    void leave_at_output_cell(
                        [[maybe_unused]] Index const idx0, [[maybe_unused]] Index const idx1)
                    {
                    }


                    void leave_at_sink_cell(
                        [[maybe_unused]] Index const idx0, [[maybe_unused]] Index const idx1)
                    {
                    }


                    void leave_intra_partition_stream(
                        [[maybe_unused]] Index const idx0, [[maybe_unused]] Index const idx1)
                    {
                    }


                    void leave_inter_partition_stream(
                        [[maybe_unused]] Index const idx0, [[maybe_unused]] Index const idx1)
                    {
                    }


                    void accumulate_external_inflow(Index const idx0, Index const idx1)
                    {
                        MaterialElement const& external_inflow{
                            detail::to_value(_external_inflow, idx0, idx1)};

                        MaterialElement& outflow{_outflow(idx0, idx1)};

                        if (!_ondp_outflow.is_no_data(outflow))
                        {
                            if (_indp_inflow.is_no_data(external_inflow) ||
                                !_dp.within_domain(external_inflow))
                            {
                                _ondp_outflow.mark_no_data(outflow);
                            }
                            else
                            {
                                // Now we know the final, total amount
                                // of inflow that enters this cell
                                outflow += external_inflow;
                            }
                        }
                    }


                    void accumulate_downstream(
                        Index const idx0_from,
                        Index const idx1_from,
                        Index const idx0_to,
                        Index const idx1_to)
                    {
                        // The results for the upstream cell are ready. Use
                        // its outflow as inflow for the downstream cell.
                        MaterialElement const& outflow{_outflow(idx0_from, idx1_from)};

                        MaterialElement& inflow{_outflow(idx0_to, idx1_to)};

                        if (!_ondp_outflow.is_no_data(inflow))
                        {
                            if (_ondp_outflow.is_no_data(outflow))
                            {
                                _ondp_outflow.mark_no_data(inflow);
                            }
                            else
                            {
                                lue_hpx_assert(outflow >= 0);

                                // Just add the outflow from upstream to
                                // the inflow of the downstream cell
                                inflow += outflow;
                            }
                        }
                    }


                    void accumulate_downstream(
                        MaterialElement const& outflow, Index const idx0_to, Index const idx1_to)
                    {
                        // The results for the upstream cell are ready
                        MaterialElement& inflow{_outflow(idx0_to, idx1_to)};

                        if (!_ondp_outflow.is_no_data(inflow))
                        {
                            if (_ondp_outflow.is_no_data(outflow))
                            {
                                _ondp_outflow.mark_no_data(inflow);
                            }
                            else
                            {
                                // Just add the outflow from upstream to
                                // the inflow of the downstream cell
                                inflow += outflow;
                            }
                        }
                    }


                    auto outflow(Index const idx0, Index const idx1) const -> MaterialElement const&
                    {
                        return _outflow(idx0, idx1);
                    }


                    void mark_no_data(Index const idx0, Index const idx1)
                    {
                        _ondp_outflow.mark_no_data(_outflow, idx0, idx1);
                    }


                private:

                    DomainPolicy _dp;

                    InflowNoDataPolicy _indp_inflow;

                    OutflowNoDataPolicy _ondp_outflow;

                    Material const _external_inflow;  // External inflow

                    MaterialData& _outflow;  // Upstream inflow, outflow
            };


            static constexpr char const* name{"accu"};

            using Material = policy::InputElementT<Policies, 1>;

            using MaterialPartitions = typename PartitionedArray<Material, 2>::Partitions;
            using MaterialPartition = ArrayPartition<Material, 2>;
            using MaterialData = DataT<MaterialPartition>;

            using IntraPartitionStreamCellsResult = std::tuple<
                hpx::future<MaterialData>,
                hpx::future<typename Base::InflowCountData>,
                hpx::future<std::array<typename Base::CellsIdxs, detail::nr_neighbours<2>()>>>;

            using InterPartitionStreamCellsResult = std::tuple<hpx::future<MaterialData>>;
    };


    template<typename Policies>
    auto accu(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& flow_direction,
        PartitionedArray<policy::InputElementT<Policies, 1>, 2> const& inflow)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>
    {
        detail::verify_compatible(flow_direction, inflow);

        return std::get<0>(accumulating_router(policies, Accu<Policies>{}, flow_direction, inflow));
    }


    template<typename Policies>
    auto accu(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& flow_direction,
        Scalar<policy::InputElementT<Policies, 1>> const& inflow)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>
    {
        return std::get<0>(accumulating_router(policies, Accu<Policies>{}, flow_direction, inflow));
    }

}  // namespace lue


#define LUE_INSTANTIATE_ACCU(Policies)                                                                       \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT auto accu<ArgumentType<void(Policies)>>(                           \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const&,                                      \
        PartitionedArray<policy::InputElementT<Policies, 1>, 2> const&)                                      \
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;                                         \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT auto accu<ArgumentType<void(Policies)>>(                           \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const&,                                      \
        Scalar<policy::InputElementT<Policies, 1>> const&)                                                   \
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;
