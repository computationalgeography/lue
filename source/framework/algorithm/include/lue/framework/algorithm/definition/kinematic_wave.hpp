#pragma once
#include "lue/framework/algorithm/accu_threshold.hpp"
#include "lue/framework/algorithm/definition/accumulating_router.hpp"
#include "lue/framework/algorithm/detail/kinematic_wave.hpp"
#include "lue/framework/algorithm/detail/verify_compatible.hpp"
#include "lue/framework/algorithm/routing_operation_export.hpp"
#include "lue/macro.hpp"


namespace lue {
    namespace detail {

        template<typename Policies>
        class KinematicWave:
            public AccumulatingRouterFunctor<
                policy::detail::TypeList<
                    policy::InputElementT<Policies, 1>,
                    policy::InputElementT<Policies, 2>,
                    policy::InputElementT<Policies, 3>,
                    policy::InputElementT<Policies, 4>,
                    policy::InputElementT<Policies, 5>,
                    policy::InputElementT<Policies, 6>>,
                policy::detail::TypeList<policy::OutputElementT<Policies, 0>>>
        {

            private:

                using Base = AccumulatingRouterFunctor<
                    policy::detail::TypeList<
                        policy::InputElementT<Policies, 1>,
                        policy::InputElementT<Policies, 2>,
                        policy::InputElementT<Policies, 3>,
                        policy::InputElementT<Policies, 4>,
                        policy::InputElementT<Policies, 5>,
                        policy::InputElementT<Policies, 6>>,
                    policy::detail::TypeList<policy::OutputElementT<Policies, 0>>>;

            public:

                template<
                    typename CurrentOutflow,
                    typename Inflow,
                    typename Alpha,
                    typename Beta,
                    typename TimeStepDuration,
                    typename ChannelLength>
                class CellAccumulator
                {

                    public:

                        using DomainPolicy = policy::DomainPolicyT<Policies>;

                        using CurrentOutflowNoDataPolicy =
                            policy::InputNoDataPolicy2T<policy::InputPoliciesT<Policies, 1>>;
                        using InflowNoDataPolicy =
                            policy::InputNoDataPolicy2T<policy::InputPoliciesT<Policies, 2>>;
                        using AlphaNoDataPolicy =
                            policy::InputNoDataPolicy2T<policy::InputPoliciesT<Policies, 3>>;
                        using BetaNoDataPolicy =
                            policy::InputNoDataPolicy2T<policy::InputPoliciesT<Policies, 4>>;
                        using TimeStepDurationNoDataPolicy =
                            policy::InputNoDataPolicy2T<policy::InputPoliciesT<Policies, 5>>;
                        using ChannelLengthNoDataPolicy =
                            policy::InputNoDataPolicy2T<policy::InputPoliciesT<Policies, 6>>;

                        using OutflowNoDataPolicy =
                            policy::OutputNoDataPolicy2T<policy::OutputPoliciesT<Policies, 0>>;

                        static_assert(
                            std::is_same_v<ElementT<CurrentOutflow>, policy::InputElementT<Policies, 1>>);
                        static_assert(std::is_same_v<ElementT<Inflow>, policy::InputElementT<Policies, 2>>);
                        static_assert(std::is_same_v<ElementT<Alpha>, policy::InputElementT<Policies, 3>>);
                        static_assert(std::is_same_v<ElementT<Beta>, policy::InputElementT<Policies, 4>>);
                        static_assert(
                            std::is_same_v<ElementT<TimeStepDuration>, policy::InputElementT<Policies, 5>>);
                        static_assert(
                            std::is_same_v<ElementT<ChannelLength>, policy::InputElementT<Policies, 6>>);

                        using MaterialElement = policy::ElementT<InflowNoDataPolicy>;

                        static_assert(
                            std::is_same_v<policy::ElementT<CurrentOutflowNoDataPolicy>, MaterialElement>);
                        static_assert(std::is_same_v<policy::ElementT<InflowNoDataPolicy>, MaterialElement>);
                        static_assert(std::is_same_v<policy::ElementT<AlphaNoDataPolicy>, MaterialElement>);
                        static_assert(std::is_same_v<policy::ElementT<BetaNoDataPolicy>, MaterialElement>);
                        static_assert(
                            std::is_same_v<policy::ElementT<TimeStepDurationNoDataPolicy>, MaterialElement>);
                        static_assert(
                            std::is_same_v<policy::ElementT<ChannelLengthNoDataPolicy>, MaterialElement>);
                        static_assert(std::is_same_v<policy::ElementT<OutflowNoDataPolicy>, MaterialElement>);

                        using MaterialData = DataT<PartitionedArray<MaterialElement, 2>>;


                        CellAccumulator(
                            Policies const& policies,

                            CurrentOutflow const& current_outflow,
                            Inflow const& inflow,
                            Alpha const& alpha,
                            Beta const& beta,
                            TimeStepDuration const& time_step_duration,
                            ChannelLength const& channel_length,
                            MaterialData& outflow):

                            _dp{policies.domain_policy()},

                            _indp_current_outflow{
                                std::get<1>(policies.inputs_policies()).input_no_data_policy()},
                            _indp_inflow{std::get<2>(policies.inputs_policies()).input_no_data_policy()},
                            _indp_alpha{std::get<3>(policies.inputs_policies()).input_no_data_policy()},
                            _indp_beta{std::get<4>(policies.inputs_policies()).input_no_data_policy()},
                            _indp_time_step_duration{
                                std::get<5>(policies.inputs_policies()).input_no_data_policy()},
                            _indp_channel_length{
                                std::get<6>(policies.inputs_policies()).input_no_data_policy()},

                            _ondp_outflow{std::get<0>(policies.outputs_policies()).output_no_data_policy()},

                            _current_outflow{current_outflow},
                            _inflow{inflow},
                            _alpha{alpha},
                            _beta{beta},
                            _time_step_duration{time_step_duration},
                            _channel_length{channel_length},
                            _outflow{outflow}

                        {
                        }


                        void enter_intra_partition_stream(
                            [[maybe_unused]] Index const idx0, [[maybe_unused]] Index const idx1)
                        {
                        }


                        void leave_intra_partition_stream(
                            [[maybe_unused]] Index const idx0, [[maybe_unused]] Index const idx1)
                        {
                        }


                        void enter_inter_partition_stream(
                            MaterialElement const& outflow, Index const idx0, Index const idx1)
                        {
                            // The results for the upstream cell are ready
                            MaterialElement& inflow{_outflow(idx0, idx1)};

                            if (!_ondp_outflow.is_no_data(inflow))
                            {
                                if (_ondp_outflow.is_no_data(outflow))
                                {
                                    _ondp_outflow.mark_no_data(inflow);
                                }
                                else
                                {
                                    // Just add the outflow from upstream to the inflow of the downstream cell
                                    inflow += outflow;
                                }
                            }
                        }


                        void leave_inter_partition_stream(
                            [[maybe_unused]] Index const idx0, [[maybe_unused]] Index const idx1)
                        {
                        }


                        void enter_cell(Index const idx0, Index const idx1)
                        {
                            MaterialElement const& current_outflow{to_value(_current_outflow, idx0, idx1)};
                            MaterialElement const& inflow{to_value(_inflow, idx0, idx1)};
                            MaterialElement const& alpha{to_value(_alpha, idx0, idx1)};
                            MaterialElement const& beta{to_value(_beta, idx0, idx1)};
                            MaterialElement const& time_step_duration{
                                to_value(_time_step_duration, idx0, idx1)};
                            MaterialElement const& channel_length{to_value(_channel_length, idx0, idx1)};

                            MaterialElement& outflow{_outflow(idx0, idx1)};

                            // TODO: what about domain of alpha and beta? time step duration?

                            if (!_ondp_outflow.is_no_data(outflow))
                            {
                                if (_indp_current_outflow.is_no_data(current_outflow) ||
                                    _indp_inflow.is_no_data(inflow) || _indp_alpha.is_no_data(alpha) ||
                                    _indp_beta.is_no_data(beta) ||
                                    _indp_time_step_duration.is_no_data(time_step_duration) ||
                                    _indp_channel_length.is_no_data(channel_length) ||
                                    !_dp.within_domain(current_outflow, inflow, channel_length))
                                {
                                    _ondp_outflow.mark_no_data(outflow);
                                }
                                else
                                {
                                    lue_hpx_assert(outflow >= 0);
                                    lue_hpx_assert(inflow >= 0);

                                    outflow = iterate_to_new_discharge(
                                        outflow,
                                        current_outflow,
                                        inflow,
                                        alpha,
                                        beta,
                                        time_step_duration,
                                        channel_length);
                                }
                            }
                        }


                        void leave_cell(
                            Index const idx0_from,
                            Index const idx1_from,
                            Index const idx0_to,
                            Index const idx1_to)
                        {
                            // The results for the upstream cell are ready. Use its outflow as inflow for the
                            // downstream cell.
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

                                    // Just add the outflow from upstream to the inflow of the downstream cell
                                    inflow += outflow;
                                }
                            }
                        }


                        void stop_at_sink_cell(
                            [[maybe_unused]] Index const idx0, [[maybe_unused]] Index const idx1)
                        {
                        }


                        void stop_at_confluence_cell(
                            [[maybe_unused]] Index const idx0, [[maybe_unused]] Index const idx1)
                        {
                        }


                        void stop_at_partition_output_cell(
                            [[maybe_unused]] Index const idx0, [[maybe_unused]] Index const idx1)
                        {
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

                        CurrentOutflowNoDataPolicy _indp_current_outflow;

                        InflowNoDataPolicy _indp_inflow;

                        AlphaNoDataPolicy _indp_alpha;

                        BetaNoDataPolicy _indp_beta;

                        TimeStepDurationNoDataPolicy _indp_time_step_duration;

                        ChannelLengthNoDataPolicy _indp_channel_length;

                        OutflowNoDataPolicy _ondp_outflow;

                        CurrentOutflow const _current_outflow;

                        Inflow const _inflow;

                        Alpha const _alpha;

                        Beta const _beta;

                        TimeStepDuration const _time_step_duration;

                        ChannelLength const _channel_length;

                        MaterialData& _outflow;
                };


                static constexpr char const* name{"kinematic_wave4"};

                using Material = policy::InputElementT<Policies, 1>;

                using MaterialPartitions = typename PartitionedArray<Material, 2>::Partitions;
                using MaterialPartition = ArrayPartition<Material, 2>;
                using MaterialData = DataT<MaterialPartition>;

                using IntraPartitionStreamCellsResult = std::tuple<
                    hpx::future<MaterialData>,
                    hpx::future<typename Base::InflowCountData>,
                    hpx::future<std::array<typename Base::CellsIdxs, nr_neighbours<2>()>>>;

                using InterPartitionStreamCellsResult = std::tuple<hpx::future<MaterialData>>;


                template<
                    typename CurrentOutflow,
                    typename Inflow,
                    typename Alpha,
                    typename Beta,
                    typename TimeStepDuration,
                    typename ChannelLength>
                auto cell_accumulator(
                    Policies const& policies,
                    CurrentOutflow const& current_outflow,
                    Inflow const& inflow,
                    Alpha const& alpha,
                    Beta const& beta,
                    TimeStepDuration const& time_step_duration,
                    ChannelLength const& channel_length,
                    MaterialData& outflow) const
                {
                    return CellAccumulator<
                        CurrentOutflow,
                        Inflow,
                        Alpha,
                        Beta,
                        TimeStepDuration,
                        ChannelLength>{
                        policies,
                        current_outflow,
                        inflow,
                        alpha,
                        beta,
                        time_step_duration,
                        channel_length,
                        outflow};
                }
        };

    }  // namespace detail


    template<typename Policies>
    auto kinematic_wave(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& flow_direction,
        PartitionedArray<policy::InputElementT<Policies, 1>, 2> const& current_outflow,
        PartitionedArray<policy::InputElementT<Policies, 2>, 2> const& inflow,
        PartitionedArray<policy::InputElementT<Policies, 3>, 2> const& alpha,
        PartitionedArray<policy::InputElementT<Policies, 4>, 2> const& beta,
        Scalar<policy::InputElementT<Policies, 5>> const& time_step_duration,
        PartitionedArray<policy::InputElementT<Policies, 6>, 2> const& channel_length)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>
    {
        detail::verify_compatible(flow_direction, current_outflow, inflow, alpha, beta, channel_length);

        return std::get<0>(accumulating_router(
            policies,
            detail::KinematicWave<Policies>{},
            flow_direction,
            current_outflow,
            inflow,
            alpha,
            beta,
            time_step_duration,
            channel_length));
    }


    template<typename Policies>
    auto kinematic_wave(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& flow_direction,
        PartitionedArray<policy::InputElementT<Policies, 1>, 2> const& current_outflow,
        PartitionedArray<policy::InputElementT<Policies, 2>, 2> const& inflow,
        Scalar<policy::InputElementT<Policies, 3>> const& alpha,
        Scalar<policy::InputElementT<Policies, 4>> const& beta,
        Scalar<policy::InputElementT<Policies, 5>> const& time_step_duration,
        Scalar<policy::InputElementT<Policies, 6>> const& channel_length)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>
    {
        detail::verify_compatible(flow_direction, current_outflow, inflow);

        return std::get<0>(accumulating_router(
            policies,
            detail::KinematicWave<Policies>{},
            flow_direction,
            current_outflow,
            inflow,
            alpha,
            beta,
            time_step_duration,
            channel_length));
    }

}  // namespace lue


#define LUE_INSTANTIATE_KINEMATIC_WAVE(Policies)                                                             \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT auto kinematic_wave<ArgumentType<void(Policies)>>(                 \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const&,                                      \
        PartitionedArray<policy::InputElementT<Policies, 1>, 2> const&,                                      \
        PartitionedArray<policy::InputElementT<Policies, 2>, 2> const&,                                      \
        PartitionedArray<policy::InputElementT<Policies, 3>, 2> const&,                                      \
        PartitionedArray<policy::InputElementT<Policies, 4>, 2> const&,                                      \
        Scalar<policy::InputElementT<Policies, 5>> const&,                                                   \
        PartitionedArray<policy::InputElementT<Policies, 6>, 2> const&)                                      \
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;                                         \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT auto kinematic_wave<ArgumentType<void(Policies)>>(                 \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const&,                                      \
        PartitionedArray<policy::InputElementT<Policies, 1>, 2> const&,                                      \
        PartitionedArray<policy::InputElementT<Policies, 2>, 2> const&,                                      \
        Scalar<policy::InputElementT<Policies, 3>> const&,                                                   \
        Scalar<policy::InputElementT<Policies, 4>> const&,                                                   \
        Scalar<policy::InputElementT<Policies, 5>> const&,                                                   \
        Scalar<policy::InputElementT<Policies, 6>> const&)                                                   \
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>
