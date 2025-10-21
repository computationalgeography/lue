#pragma once
#include "lue/framework/algorithm/accu_threshold.hpp"
#include "lue/framework/algorithm/definition/accumulating_router.hpp"
#include "lue/framework/algorithm/detail/verify_compatible.hpp"
#include "lue/framework/algorithm/routing_operation_export.hpp"
#include "lue/macro.hpp"
#include <boost/math/tools/roots.hpp>


namespace lue {
    namespace detail {

        template<std::floating_point FloatingPoint>
        class NonLinearKinematicWave
        {

            public:

                NonLinearKinematicWave(
                    FloatingPoint const upstream_discharge,
                    FloatingPoint const current_discharge,
                    FloatingPoint const lateral_inflow,
                    FloatingPoint const alpha,
                    FloatingPoint const beta,
                    FloatingPoint const time_step_duration,
                    FloatingPoint const channel_length):

                    _upstream_discharge{upstream_discharge},
                    _current_discharge{current_discharge},
                    _lateral_inflow{lateral_inflow},
                    _alpha{alpha},
                    _beta{beta},
                    _alpha_beta{_alpha * _beta},
                    _time_step_duration{time_step_duration}

                {
                    lue_hpx_assert(_upstream_discharge >= FloatingPoint{0});
                    lue_hpx_assert(_current_discharge >= FloatingPoint{0});
                    lue_hpx_assert(
                        _upstream_discharge + _current_discharge + _lateral_inflow > FloatingPoint{0});
                    lue_hpx_assert(_alpha > FloatingPoint{0});
                    lue_hpx_assert(_beta > FloatingPoint{0});
                    lue_hpx_assert(_time_step_duration > FloatingPoint{0});
                    lue_hpx_assert(channel_length > FloatingPoint{0});

                    // Known terms, independent of new discharge
                    _time_step_duration_over_channel_length = _time_step_duration / channel_length;

                    _known_terms = (_time_step_duration_over_channel_length * _upstream_discharge) +
                                   (_alpha * std::pow(_current_discharge, _beta)) +
                                   (_time_step_duration * _lateral_inflow);
                }


                /*!
                    @brief      Return a valid initial guess for the new discharge

                    Note that fq is only defined for discharges larger than zero. In case the initial guess
                    ends up being zero or negative, a small positive value is returned.
                */
                auto guess() const -> FloatingPoint
                {
                    // Small, but not zero!
                    static FloatingPoint const min_discharge{std::numeric_limits<FloatingPoint>::min()};
                    FloatingPoint discharge_guess{min_discharge};

                    // pow(0, -) is not defined
                    if ((_current_discharge + _upstream_discharge != FloatingPoint{0}) ||
                        _beta >= FloatingPoint{1})
                    {
                        FloatingPoint const a_b_pq =
                            _alpha_beta * std::pow(
                                              (_current_discharge + _upstream_discharge) / FloatingPoint{2},
                                              _beta - FloatingPoint{1});

                        lue_hpx_assert(!std::isnan(a_b_pq));

                        discharge_guess =
                            (_time_step_duration_over_channel_length * _upstream_discharge +
                             a_b_pq * _current_discharge + _time_step_duration * _lateral_inflow) /
                            (_time_step_duration_over_channel_length + a_b_pq);

                        lue_hpx_assert(!std::isnan(discharge_guess));

                        discharge_guess = std::max<FloatingPoint>(discharge_guess, min_discharge);
                    }

                    lue_hpx_assert(discharge_guess > FloatingPoint{0});

                    return discharge_guess;
                }


                auto operator()(FloatingPoint const new_discharge) const
                    -> std::pair<FloatingPoint, FloatingPoint>
                {
                    return std::make_pair(fq(new_discharge), dfq(new_discharge));
                }


                auto fq(FloatingPoint const new_discharge) const -> FloatingPoint
                {
                    lue_hpx_assert(new_discharge > FloatingPoint{0});  // pow(0, -) is not defined

                    return (_time_step_duration_over_channel_length * new_discharge) +
                           (_alpha * std::pow(new_discharge, _beta)) - _known_terms;
                }


                auto dfq(FloatingPoint const new_discharge) const -> FloatingPoint
                {
                    lue_hpx_assert(new_discharge > FloatingPoint{0});  // pow(0, -) is not defined

                    return _time_step_duration_over_channel_length +
                           (_alpha_beta * std::pow(new_discharge, _beta - FloatingPoint{1}));
                }


            private:

                //! Updated / new discharge in the upstream cell
                FloatingPoint _upstream_discharge;

                //! Current / previous discharge in the current cell
                FloatingPoint _current_discharge;

                //! Lateral inflow
                FloatingPoint _lateral_inflow;

                FloatingPoint _alpha;

                //! Momentum coefficient / Boussinesq coefficient [1.01, 1.33] (Chow, p278)
                FloatingPoint _beta;

                FloatingPoint _alpha_beta;

                FloatingPoint _time_step_duration;

                FloatingPoint _time_step_duration_over_channel_length;

                FloatingPoint _known_terms;
        };


        template<std::floating_point FloatingPoint>
        auto iterate_to_new_discharge(
            FloatingPoint const upstream_discharge,  // Summed discharge for cells draining into current cell
            FloatingPoint const current_discharge,
            FloatingPoint const lateral_inflow,
            FloatingPoint const alpha,
            FloatingPoint const beta,
            FloatingPoint const time_step_duration,
            FloatingPoint const channel_length) -> FloatingPoint
        {
            lue_hpx_assert(upstream_discharge >= FloatingPoint{0});
            lue_hpx_assert(current_discharge >= FloatingPoint{0});
            lue_hpx_assert(alpha >= FloatingPoint{0});
            lue_hpx_assert(beta >= FloatingPoint{0});
            lue_hpx_assert(time_step_duration > FloatingPoint{0});
            lue_hpx_assert(channel_length > FloatingPoint{0});

            // Lateral inflow can represent two things:
            // - Actual inflow from an external source (positive value): e.g.: precepitation
            // - Potential extraction to an internal sink (negative value): e.g.: infiltration, pumping
            //
            // Inflow:
            // Add the amount of water to the discharge computed
            //
            // Extraction:
            // Subtract as much water from the discharge computed as possible. To allow for water balance
            // checks, we should output the actual amount of water that got extracted from the cell. This is
            // the difference between the discharge computed and the potential extraction passed in.
            // https://github.com/computationalgeography/lue/issues/527

            FloatingPoint new_discharge{0};

            if (upstream_discharge + current_discharge > 0 || lateral_inflow > 0)
            {
                // The cell receives water, from upstream and/or from an external source
                FloatingPoint const inflow = lateral_inflow >= 0 ? lateral_inflow : FloatingPoint{0};

                NonLinearKinematicWave<FloatingPoint> kinematic_wave{
                    upstream_discharge,
                    current_discharge,
                    inflow,
                    alpha,
                    beta,
                    time_step_duration,
                    channel_length};

                FloatingPoint const discharge_guess{kinematic_wave.guess()};

                // "If the function is 'Really Well Behaved' (is monotonic and has only one root) the bracket
                // bounds min and max may as well be set to the widest limits"
                FloatingPoint const min_discharge{0};
                FloatingPoint const max_discharge{std::numeric_limits<FloatingPoint>::max()};
                int const digits = static_cast<int>(std::numeric_limits<FloatingPoint>::digits * 0.6);

                // In general, 2-3 iterations are enough. In rare cases more are needed. The unit tests don't
                // seem to reach 8, so max 10 should be enough. This max is used in special cases:
                // - upstream_discharge + current_discharge == 0 and beta <  1
                std::uintmax_t const max_nr_iterations{10};
                std::uintmax_t actual_nr_iterations{max_nr_iterations};

                // https://www.boost.org/doc/libs/1_85_0/libs/math/doc/html/math_toolkit/roots_deriv.html
                // std::cout.precision(std::numeric_limits<FloatingPoint>::digits10);
                new_discharge = boost::math::tools::newton_raphson_iterate(
                    kinematic_wave,
                    discharge_guess,
                    min_discharge,
                    max_discharge,
                    digits,
                    actual_nr_iterations);

                // TODO We can't throw an exception as this actually happens once in a while
                // https://github.com/computationalgeography/lue/issues/703
                // if (actual_nr_iterations == max_nr_iterations)
                // {
                //     // This only seems to happen when upstream_discharge == 0, current_discharge == 0, and
                //     // lateral_inflow > 0
                //     throw std::runtime_error(std::format(
                //         "Iterating to a solution took more iterations than expected: "
                //         "    upstream discharge: {}, "
                //         "    current discharge: {}, "
                //         "    lateral inflow: {}, "
                //         "    alpha: {}, "
                //         "    beta: {}, "
                //         "    time step duration: {}, "
                //         "    channel length: {}, "
                //         "    initial guess: {}",
                //         upstream_discharge,
                //         current_discharge,
                //         inflow,
                //         alpha,
                //         beta,
                //         time_step_duration,
                //         channel_length,
                //         discharge_guess));
                // }
            }

            if (lateral_inflow < FloatingPoint{0})
            {
                // Convert units: m³ / m / s → m³ / s
                FloatingPoint const extraction{
                    std::min(channel_length * std::abs(lateral_inflow), new_discharge)};

                new_discharge -= extraction;
            }

            lue_hpx_assert(new_discharge >= FloatingPoint{0});

            return new_discharge;
        }


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


                        void leave_at_sink_cell(
                            [[maybe_unused]] Index const idx0, [[maybe_unused]] Index const idx1)
                        {
                        }


                        void leave_at_partition_output_cell(
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
