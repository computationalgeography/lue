#pragma once
#include "lue/framework/algorithm/definition/accumulating_router.hpp"
#include "lue/framework/algorithm/detail/verify_compatible.hpp"
#include "lue/framework/algorithm/partial_accu.hpp"
#include "lue/framework/algorithm/routing_operation_export.hpp"
#include "lue/macro.hpp"


// TODO: Get rid of inflow argument


namespace lue {

    template<typename Policies>
    class PartialAccu:
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
                        Policies const& policies,
                        Material const& external_inflow,
                        MaterialData& outflow,
                        Count const nr_steps):

                        _dp{policies.domain_policy()},
                        _indp_inflow{std::get<1>(policies.inputs_policies()).input_no_data_policy()},
                        _ondp_outflow{std::get<0>(policies.outputs_policies()).output_no_data_policy()},

                        _external_inflow{external_inflow},
                        _outflow{outflow},

                        _nr_steps{std::max<Count>(nr_steps, 0)},
                        _step{0}

                    {
                    }


                    void enter_intra_partition_stream(
                        [[maybe_unused]] Index const idx0, [[maybe_unused]] Index const idx1)
                    {
                        // All intra-partition stream cells will be filled with this one as they are all
                        // solved in the first phase
                        _outflow(idx0, idx1) = _step;
                    }


                    void leave_intra_partition_stream(
                        [[maybe_unused]] Index const idx0, [[maybe_unused]] Index const idx1)
                    {
                        ++_step;
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
                                // Multiple upstream streams can join at this cell. Pick the max step.
                                inflow = std::max(inflow, outflow);
                            }

                            if (inflow > _nr_steps)
                            {
                                // Stop accumulating
                                _ondp_outflow.mark_no_data(inflow);
                            }
                        }
                    }


                    void leave_inter_partition_stream(
                        [[maybe_unused]] Index const idx0, [[maybe_unused]] Index const idx1)
                    {
                        MaterialElement& outflow{_outflow(idx0, idx1)};

                        if (!_ondp_outflow.is_no_data(outflow))
                        {
                            ++_outflow(idx0, idx1);
                        }
                    }


                    void enter_cell([[maybe_unused]] Index const idx0, [[maybe_unused]] Index const idx1)
                    {
                        // TODO: Doc this: this function handles arguments (e.g. external inflow)
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
                                // inflow += outflow;

                                inflow = outflow;
                            }
                        }

                        if (inflow > _nr_steps)
                        {
                            // Stop accumulating
                            _ondp_outflow.mark_no_data(inflow);
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

                    InflowNoDataPolicy _indp_inflow;

                    OutflowNoDataPolicy _ondp_outflow;

                    Material const _external_inflow;  // External inflow

                    MaterialData& _outflow;  // Upstream inflow, outflow

                    Count const _nr_steps;

                    Count _step;
            };


            static constexpr char const* name{"partial_accu"};

            using Material = policy::InputElementT<Policies, 1>;

            using MaterialPartitions = typename PartitionedArray<Material, 2>::Partitions;
            using MaterialPartition = ArrayPartition<Material, 2>;
            using MaterialData = DataT<MaterialPartition>;

            using IntraPartitionStreamCellsResult = std::tuple<
                hpx::future<MaterialData>,
                hpx::future<typename Base::InflowCountData>,
                hpx::future<std::array<typename Base::CellsIdxs, detail::nr_neighbours<2>()>>>;

            using InterPartitionStreamCellsResult = std::tuple<hpx::future<MaterialData>>;


            PartialAccu() = default;

            PartialAccu(Count const nr_steps):

                _nr_steps{std::max<Count>(nr_steps, 0)}

            {
            }


            template<typename Material>
            CellAccumulator<Material> cell_accumulator(
                Policies const& policies, Material const& external_inflow, MaterialData& outflow) const
            {
                return CellAccumulator<Material>{policies, external_inflow, outflow, _nr_steps};
            }

        private:

            friend class hpx::serialization::access;

            template<typename Archive>
            void serialize(Archive& archive, [[maybe_unused]] unsigned version)
            {
                // clang-format off
                archive & _nr_steps;
                // clang-format on
            }

            Count _nr_steps;
    };


    template<typename Policies>
    auto partial_accu(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& flow_direction,
        Scalar<policy::InputElementT<Policies, 1>> const& inflow,
        Count const nr_steps) -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>
    {
        return std::get<0>(
            accumulating_router(policies, PartialAccu<Policies>{nr_steps}, flow_direction, inflow));
    }

}  // namespace lue


#define LUE_INSTANTIATE_PARTIAL_ACCU(Policies)                                                               \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT auto partial_accu<ArgumentType<void(Policies)>>(                   \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const&,                                      \
        Scalar<policy::InputElementT<Policies, 1>> const&,                                                   \
        Count) -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;
