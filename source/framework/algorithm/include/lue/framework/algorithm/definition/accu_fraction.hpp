#pragma once
#include "lue/framework/algorithm/accu_fraction.hpp"
#include "lue/framework/algorithm/definition/accumulating_router.hpp"
#include "lue/framework/algorithm/detail/verify_compatible.hpp"
#include "lue/framework/algorithm/routing_operation_export.hpp"
#include "lue/macro.hpp"


namespace lue {

    template<typename Policies>
    class AccuFraction:
        public AccumulatingRouterFunctor<
            policy::detail::TypeList<policy::InputElementT<Policies, 1>, policy::InputElementT<Policies, 2>>,
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

            template<typename Material, typename Fraction>
            class CellAccumulator
            {

                public:

                    using DomainPolicy = policy::DomainPolicyT<Policies>;
                    using InflowNoDataPolicy =
                        policy::InputNoDataPolicy2T<policy::InputPoliciesT<Policies, 1>>;
                    using FractionNoDataPolicy =
                        policy::InputNoDataPolicy2T<policy::InputPoliciesT<Policies, 2>>;
                    using OutflowNoDataPolicy =
                        policy::OutputNoDataPolicy2T<policy::OutputPoliciesT<Policies, 0>>;
                    using RemainderNoDataPolicy =
                        policy::OutputNoDataPolicy2T<policy::OutputPoliciesT<Policies, 1>>;

                    static_assert(std::is_same_v<ElementT<Material>, policy::InputElementT<Policies, 1>>);
                    static_assert(std::is_same_v<ElementT<Fraction>, policy::InputElementT<Policies, 2>>);

                    using MaterialElement = policy::ElementT<InflowNoDataPolicy>;

                    static_assert(std::is_same_v<policy::ElementT<InflowNoDataPolicy>, MaterialElement>);
                    static_assert(std::is_same_v<policy::ElementT<FractionNoDataPolicy>, MaterialElement>);
                    static_assert(std::is_same_v<policy::ElementT<OutflowNoDataPolicy>, MaterialElement>);
                    static_assert(std::is_same_v<policy::ElementT<RemainderNoDataPolicy>, MaterialElement>);

                    using MaterialData = DataT<PartitionedArray<MaterialElement, 2>>;


                    CellAccumulator(
                        Policies const& policies,
                        Material const& external_inflow,
                        Fraction const& fraction,
                        MaterialData& outflow,
                        MaterialData& remainder):

                        _dp{policies.domain_policy()},
                        _indp_inflow{std::get<1>(policies.inputs_policies()).input_no_data_policy()},
                        _indp_fraction{std::get<2>(policies.inputs_policies()).input_no_data_policy()},
                        _ondp_outflow{std::get<0>(policies.outputs_policies()).output_no_data_policy()},
                        _ondp_remainder{std::get<1>(policies.outputs_policies()).output_no_data_policy()},

                        _external_inflow{external_inflow},
                        _fraction{fraction},
                        _outflow{outflow},
                        _remainder{remainder}

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
                        MaterialElement& remainder{_remainder(idx0, idx1)};

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


                    void leave_inter_partition_stream(
                        [[maybe_unused]] Index const idx0, [[maybe_unused]] Index const idx1)
                    {
                    }


                    void enter_cell(Index const idx0, Index const idx1)
                    {
                        MaterialElement const& external_inflow{
                            detail::to_value(_external_inflow, idx0, idx1)};
                        MaterialElement const& fraction{detail::to_value(_fraction, idx0, idx1)};

                        MaterialElement& outflow{_outflow(idx0, idx1)};
                        MaterialElement& remainder{_remainder(idx0, idx1)};

                        lue_hpx_assert(
                            (_ondp_outflow.is_no_data(outflow) && _ondp_remainder.is_no_data(remainder)) ||
                            (!_ondp_outflow.is_no_data(outflow) && !_ondp_remainder.is_no_data(remainder)));

                        if (!_ondp_outflow.is_no_data(outflow))
                        {
                            if (_indp_inflow.is_no_data(external_inflow) ||
                                _indp_fraction.is_no_data(fraction) ||
                                !_dp.within_domain(external_inflow, fraction))
                            {
                                _ondp_outflow.mark_no_data(outflow);
                                _ondp_remainder.mark_no_data(remainder);
                            }
                            else
                            {
                                lue_hpx_assert(external_inflow >= 0);
                                lue_hpx_assert(fraction >= 0 && fraction <= 1);

                                // Now we know the final, total amount
                                // of inflow that enters this cell
                                outflow += external_inflow;

                                // Split this amount into outflow and remainder,
                                // based on the fraction passed in
                                MaterialElement delta_flux{fraction * outflow};
                                remainder = outflow - delta_flux;
                                outflow = delta_flux;
                            }
                        }
                    }


                    void leave_cell(
                        Index const idx0_from,
                        Index const idx1_from,
                        Index const idx0_to,
                        Index const idx1_to)
                    {
                        // The results for the upstream cell are ready. Use
                        // its outflow as inflow for the downstream cell.
                        MaterialElement const& outflow{_outflow(idx0_from, idx1_from)};
                        [[maybe_unused]] MaterialElement const& upstream_remainder{
                            _remainder(idx0_from, idx1_from)};

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
                        _ondp_remainder.mark_no_data(_remainder, idx0, idx1);
                    }


                private:

                    DomainPolicy _dp;

                    InflowNoDataPolicy _indp_inflow;

                    FractionNoDataPolicy _indp_fraction;

                    OutflowNoDataPolicy _ondp_outflow;

                    RemainderNoDataPolicy _ondp_remainder;

                    Material const _external_inflow;  // External inflow

                    Fraction const _fraction;

                    MaterialData& _outflow;  // Upstream inflow, outflow

                    MaterialData& _remainder;
            };


            static constexpr char const* name{"accu_fraction4"};

            using Material = policy::InputElementT<Policies, 1>;

            using MaterialPartitions = typename PartitionedArray<Material, 2>::Partitions;
            using MaterialPartition = ArrayPartition<Material, 2>;
            using MaterialData = DataT<MaterialPartition>;

            using IntraPartitionStreamCellsResult = std::tuple<
                hpx::future<MaterialData>,
                hpx::future<MaterialData>,
                hpx::future<typename Base::InflowCountData>,
                hpx::future<std::array<typename Base::CellsIdxs, detail::nr_neighbours<2>()>>>;

            using InterPartitionStreamCellsResult =
                std::tuple<hpx::future<MaterialData>, hpx::future<MaterialData>>;

            template<typename Material, typename Fraction>
            auto cell_accumulator(
                Policies const& policies,
                Material const& external_inflow,
                Fraction const& fraction,
                MaterialData& outflow,
                MaterialData& remainder) const -> CellAccumulator<Material, Fraction>
            {
                return CellAccumulator<Material, Fraction>{
                    policies, external_inflow, fraction, outflow, remainder};
            }
    };


    template<typename Policies>
    auto accu_fraction(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& flow_direction,
        PartitionedArray<policy::InputElementT<Policies, 1>, 2> const& inflow,
        PartitionedArray<policy::InputElementT<Policies, 2>, 2> const& fraction)
        -> std::tuple<
            PartitionedArray<policy::OutputElementT<Policies, 0>, 2>,
            PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>
    {
        detail::verify_compatible(flow_direction, inflow, fraction);

        return accumulating_router(policies, AccuFraction<Policies>{}, flow_direction, inflow, fraction);
    }


    template<typename Policies>
    auto accu_fraction(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& flow_direction,
        PartitionedArray<policy::InputElementT<Policies, 1>, 2> const& inflow,
        Scalar<policy::InputElementT<Policies, 2>> const& fraction)
        -> std::tuple<
            PartitionedArray<policy::OutputElementT<Policies, 0>, 2>,
            PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>
    {
        detail::verify_compatible(flow_direction, inflow);

        return accumulating_router(policies, AccuFraction<Policies>{}, flow_direction, inflow, fraction);
    }


    template<typename Policies>
    auto accu_fraction(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& flow_direction,
        Scalar<policy::InputElementT<Policies, 1>> const& inflow,
        Scalar<policy::InputElementT<Policies, 2>> const& fraction)
        -> std::tuple<
            PartitionedArray<policy::OutputElementT<Policies, 0>, 2>,
            PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>
    {
        return accumulating_router(policies, AccuFraction<Policies>{}, flow_direction, inflow, fraction);
    }


    template<typename Policies>
    auto accu_fraction(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& flow_direction,
        Scalar<policy::InputElementT<Policies, 1>> const& inflow,
        PartitionedArray<policy::InputElementT<Policies, 2>, 2> const& fraction)
        -> std::tuple<
            PartitionedArray<policy::OutputElementT<Policies, 0>, 2>,
            PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>
    {
        detail::verify_compatible(flow_direction, fraction);

        return accumulating_router(policies, AccuFraction<Policies>{}, flow_direction, inflow, fraction);
    }

}  // namespace lue


#define LUE_INSTANTIATE_ACCU_FRACTION(Policies)                                                              \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT auto accu_fraction<ArgumentType<void(Policies)>>(                  \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const&,                                      \
        PartitionedArray<policy::InputElementT<Policies, 1>, 2> const&,                                      \
        PartitionedArray<policy::InputElementT<Policies, 2>, 2> const&)                                      \
        -> std::tuple<                                                                                       \
            PartitionedArray<policy::OutputElementT<Policies, 0>, 2>,                                        \
            PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>;                                       \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT auto accu_fraction<ArgumentType<void(Policies)>>(                  \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const&,                                      \
        PartitionedArray<policy::InputElementT<Policies, 1>, 2> const&,                                      \
        Scalar<policy::InputElementT<Policies, 2>> const&)                                                   \
        -> std::tuple<                                                                                       \
            PartitionedArray<policy::OutputElementT<Policies, 0>, 2>,                                        \
            PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>;                                       \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT auto accu_fraction<ArgumentType<void(Policies)>>(                  \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const&,                                      \
        Scalar<policy::InputElementT<Policies, 1>> const&,                                                   \
        Scalar<policy::InputElementT<Policies, 2>> const&)                                                   \
        -> std::tuple<                                                                                       \
            PartitionedArray<policy::OutputElementT<Policies, 0>, 2>,                                        \
            PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>;                                       \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT auto accu_fraction<ArgumentType<void(Policies)>>(                  \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const&,                                      \
        Scalar<policy::InputElementT<Policies, 1>> const&,                                                   \
        PartitionedArray<policy::InputElementT<Policies, 2>, 2> const&)                                      \
        -> std::tuple<                                                                                       \
            PartitionedArray<policy::OutputElementT<Policies, 0>, 2>,                                        \
            PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>;
