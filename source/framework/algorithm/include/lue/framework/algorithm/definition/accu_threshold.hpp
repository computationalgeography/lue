#pragma once
#include "lue/framework/algorithm/accu_threshold.hpp"
#include "lue/framework/algorithm/definition/accumulating_router.hpp"
#include "lue/framework/algorithm/detail/verify_compatible.hpp"
#include "lue/framework/algorithm/routing_operation_export.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail {

        template<typename Policies>
        class AccuThreshold:
            public AccumulatingRouterFunctor<
                policy::detail::
                    TypeList<policy::InputElementT<Policies, 1>, policy::InputElementT<Policies, 2>>,
                policy::detail::
                    TypeList<policy::OutputElementT<Policies, 0>, policy::OutputElementT<Policies, 1>>>
        {

            private:

                using Base = AccumulatingRouterFunctor<
                    policy::detail::
                        TypeList<policy::InputElementT<Policies, 1>, policy::InputElementT<Policies, 2>>,
                    policy::detail::
                        TypeList<policy::OutputElementT<Policies, 0>, policy::OutputElementT<Policies, 1>>>;

            public:

                template<typename Material, typename Threshold>
                class CellAccumulator
                {

                    public:

                        using DomainPolicy = policy::DomainPolicyT<Policies>;
                        using InflowNoDataPolicy =
                            policy::InputNoDataPolicy2T<policy::InputPoliciesT<Policies, 1>>;
                        using ThresholdNoDataPolicy =
                            policy::InputNoDataPolicy2T<policy::InputPoliciesT<Policies, 2>>;
                        using OutflowNoDataPolicy =
                            policy::OutputNoDataPolicy2T<policy::OutputPoliciesT<Policies, 0>>;
                        using RemainderNoDataPolicy =
                            policy::OutputNoDataPolicy2T<policy::OutputPoliciesT<Policies, 1>>;

                        static_assert(std::is_same_v<ElementT<Material>, policy::InputElementT<Policies, 1>>);
                        static_assert(
                            std::is_same_v<ElementT<Threshold>, policy::InputElementT<Policies, 2>>);

                        using MaterialElement = policy::ElementT<InflowNoDataPolicy>;

                        static_assert(std::is_same_v<policy::ElementT<InflowNoDataPolicy>, MaterialElement>);
                        static_assert(
                            std::is_same_v<policy::ElementT<ThresholdNoDataPolicy>, MaterialElement>);
                        static_assert(std::is_same_v<policy::ElementT<OutflowNoDataPolicy>, MaterialElement>);
                        static_assert(
                            std::is_same_v<policy::ElementT<RemainderNoDataPolicy>, MaterialElement>);

                        using MaterialData = DataT<PartitionedArray<MaterialElement, 2>>;


                        CellAccumulator(
                            Policies const& policies,
                            Material const& external_inflow,
                            Threshold const& threshold,
                            MaterialData& outflow,
                            MaterialData& remainder):

                            _dp{policies.domain_policy()},
                            _indp_inflow{std::get<1>(policies.inputs_policies()).input_no_data_policy()},
                            _indp_threshold{std::get<2>(policies.inputs_policies()).input_no_data_policy()},
                            _ondp_outflow{std::get<0>(policies.outputs_policies()).output_no_data_policy()},
                            _ondp_remainder{std::get<1>(policies.outputs_policies()).output_no_data_policy()},

                            _external_inflow{external_inflow},
                            _threshold{threshold},
                            _outflow{outflow},
                            _remainder{remainder}

                        {
                        }


                        void enter_at_ridge(
                            [[maybe_unused]] Index const idx0, [[maybe_unused]] Index const idx1)
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
                            MaterialElement const& external_inflow{to_value(_external_inflow, idx0, idx1)};
                            MaterialElement const& threshold{to_value(_threshold, idx0, idx1)};

                            MaterialElement& outflow{_outflow(idx0, idx1)};
                            MaterialElement& remainder{_remainder(idx0, idx1)};

                            lue_hpx_assert(
                                (_ondp_outflow.is_no_data(outflow) &&
                                 _ondp_remainder.is_no_data(remainder)) ||
                                (!_ondp_outflow.is_no_data(outflow) &&
                                 !_ondp_remainder.is_no_data(remainder)));

                            if (!_ondp_outflow.is_no_data(outflow))
                            {
                                if (_indp_inflow.is_no_data(external_inflow) ||
                                    _indp_threshold.is_no_data(threshold) ||
                                    !_dp.within_domain(external_inflow, threshold))
                                {
                                    _ondp_outflow.mark_no_data(outflow);
                                    _ondp_remainder.mark_no_data(remainder);
                                }
                                else
                                {
                                    lue_hpx_assert(external_inflow >= 0);
                                    lue_hpx_assert(threshold >= 0);

                                    // Now we know the final, total amount
                                    // of inflow that enters this cell
                                    outflow += external_inflow;

                                    // Split this amount into outflow and remainder,
                                    // based on the threshold passed in
                                    if (outflow > threshold)
                                    {
                                        remainder = threshold;
                                        outflow -= threshold;
                                    }
                                    else
                                    {
                                        remainder = outflow;
                                        outflow = 0;
                                    }
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
                            MaterialElement const& upstream_remainder{_remainder(idx0_from, idx1_from)};

                            MaterialElement& inflow{_outflow(idx0_to, idx1_to)};
                            MaterialElement& remainder{_remainder(idx0_to, idx1_to)};

                            if (!_ondp_outflow.is_no_data(inflow))
                            {
                                lue_hpx_assert(!_ondp_remainder.is_no_data(remainder));

                                if (_ondp_outflow.is_no_data(outflow))
                                {
                                    lue_hpx_assert(_ondp_remainder.is_no_data(upstream_remainder));

                                    _ondp_outflow.mark_no_data(inflow);
                                    _ondp_remainder.mark_no_data(remainder);
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
                            MaterialElement& remainder{_remainder(idx0_to, idx1_to)};

                            if (!_ondp_outflow.is_no_data(inflow))
                            {
                                lue_hpx_assert(!_ondp_remainder.is_no_data(remainder));

                                if (_ondp_outflow.is_no_data(outflow))
                                {
                                    _ondp_outflow.mark_no_data(inflow);
                                    _ondp_remainder.mark_no_data(remainder);
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
                            _ondp_remainder.mark_no_data(_remainder, idx0, idx1);
                        }


                    private:

                        DomainPolicy _dp;

                        InflowNoDataPolicy _indp_inflow;

                        ThresholdNoDataPolicy _indp_threshold;

                        OutflowNoDataPolicy _ondp_outflow;

                        RemainderNoDataPolicy _ondp_remainder;

                        Material const _external_inflow;  // External inflow

                        Threshold const _threshold;

                        MaterialData& _outflow;  // Upstream inflow, outflow

                        MaterialData& _remainder;
                };


                static constexpr char const* name{"accu_threshold"};

                using Material = policy::InputElementT<Policies, 1>;

                using MaterialPartitions = typename PartitionedArray<Material, 2>::Partitions;
                using MaterialPartition = ArrayPartition<Material, 2>;
                using MaterialData = DataT<MaterialPartition>;

                using IntraPartitionStreamCellsResult = std::tuple<
                    hpx::future<MaterialData>,
                    hpx::future<MaterialData>,
                    hpx::future<typename Base::InflowCountData>,
                    hpx::future<std::array<typename Base::CellsIdxs, nr_neighbours<2>()>>>;

                using InterPartitionStreamCellsResult =
                    std::tuple<hpx::future<MaterialData>, hpx::future<MaterialData>>;
        };

    }  // namespace detail


    // raster, raster
    template<typename Policies>
    auto accu_threshold(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& flow_direction,
        PartitionedArray<policy::InputElementT<Policies, 1>, 2> const& inflow,
        PartitionedArray<policy::InputElementT<Policies, 2>, 2> const& threshold)
        -> std::tuple<
            PartitionedArray<policy::OutputElementT<Policies, 0>, 2>,
            PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>
    {
        detail::verify_compatible(flow_direction, inflow, threshold);

        return accumulating_router(
            policies, detail::AccuThreshold<Policies>{}, flow_direction, inflow, threshold);
    }


    // raster, scalar
    template<typename Policies>
    auto accu_threshold(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& flow_direction,
        PartitionedArray<policy::InputElementT<Policies, 1>, 2> const& inflow,
        Scalar<policy::InputElementT<Policies, 2>> const& threshold)
        -> std::tuple<
            PartitionedArray<policy::OutputElementT<Policies, 0>, 2>,
            PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>
    {
        detail::verify_compatible(flow_direction, inflow);

        return accumulating_router(
            policies, detail::AccuThreshold<Policies>{}, flow_direction, inflow, threshold);
    }


    // scalar, scalar
    template<typename Policies>
    auto accu_threshold(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& flow_direction,
        Scalar<policy::InputElementT<Policies, 1>> const& inflow,
        Scalar<policy::InputElementT<Policies, 2>> const& threshold)
        -> std::tuple<
            PartitionedArray<policy::OutputElementT<Policies, 0>, 2>,
            PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>
    {
        return accumulating_router(
            policies, detail::AccuThreshold<Policies>{}, flow_direction, inflow, threshold);
    }


    // scalar, raster
    template<typename Policies>
    auto accu_threshold(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& flow_direction,
        Scalar<policy::InputElementT<Policies, 1>> const& inflow,
        PartitionedArray<policy::InputElementT<Policies, 2>, 2> const& threshold)
        -> std::tuple<
            PartitionedArray<policy::OutputElementT<Policies, 0>, 2>,
            PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>
    {
        detail::verify_compatible(flow_direction, threshold);

        return accumulating_router(
            policies, detail::AccuThreshold<Policies>{}, flow_direction, inflow, threshold);
    }

}  // namespace lue


#define LUE_INSTANTIATE_ACCU_THRESHOLD(Policies)                                                             \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT auto accu_threshold<ArgumentType<void(Policies)>>(                 \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const&,                                      \
        PartitionedArray<policy::InputElementT<Policies, 1>, 2> const&,                                      \
        PartitionedArray<policy::InputElementT<Policies, 2>, 2> const&)                                      \
        -> std::tuple<                                                                                       \
            PartitionedArray<policy::OutputElementT<Policies, 0>, 2>,                                        \
            PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>;                                       \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT auto accu_threshold<ArgumentType<void(Policies)>>(                 \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const&,                                      \
        PartitionedArray<policy::InputElementT<Policies, 1>, 2> const&,                                      \
        Scalar<policy::InputElementT<Policies, 2>> const&)                                                   \
        -> std::tuple<                                                                                       \
            PartitionedArray<policy::OutputElementT<Policies, 0>, 2>,                                        \
            PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>;                                       \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT auto accu_threshold<ArgumentType<void(Policies)>>(                 \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const&,                                      \
        Scalar<policy::InputElementT<Policies, 1>> const&,                                                   \
        Scalar<policy::InputElementT<Policies, 2>> const&)                                                   \
        -> std::tuple<                                                                                       \
            PartitionedArray<policy::OutputElementT<Policies, 0>, 2>,                                        \
            PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>;                                       \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT auto accu_threshold<ArgumentType<void(Policies)>>(                 \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const&,                                      \
        Scalar<policy::InputElementT<Policies, 1>> const&,                                                   \
        PartitionedArray<policy::InputElementT<Policies, 2>, 2> const&)                                      \
        -> std::tuple<                                                                                       \
            PartitionedArray<policy::OutputElementT<Policies, 0>, 2>,                                        \
            PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>;
