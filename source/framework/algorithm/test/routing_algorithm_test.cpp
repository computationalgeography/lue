#define BOOST_TEST_MODULE lue framework algorithm routing_algorithm
#include "flow_accumulation.hpp"
#include "lue/framework/algorithm/definition/accumulating_router.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"

#include "lue/framework/algorithm/value_policies/all.hpp"           // Only for kinematic_wave
#include "lue/framework/algorithm/value_policies/greater_than.hpp"  // Only for kinematic_wave
#include <boost/math/tools/roots.hpp>                               // Only for kinematic_wave

// Policies determine how to check argument and return element types. Therefore:
// - Argument types can be expressed in terms of element types obtained from the policies. Argument types of
//   algorithms and of functors.

// Functors determine what gets calculated. Therefore:
// - Functors define return types. They can be queried for this information. ReturnTypes<Functor>...
// - It must implement the call operator(s) that eventually get called by accumulating_router. This operator
//   must likely return results for the same number of values as the overall number of return types.

// TODO:
// - Can we assume that a accumulating router always returns one or more arrays?
//     - If so, then the call operator returns values for the same number of arrays, with corresponding
//       element types.
// - Create a general (pseudo) algorithm and see if all accumulating routing algorithms can be mapped on it.
//   What about the related algorithms, like the one classifying cells into inter/intra partition stream
//   cells.


namespace lue {

    using CellClass = SmallestUnsignedIntegralElement;
    static constexpr CellClass ridge_cell = 11;
    static constexpr CellClass intra_partition_stream_cell = 12;
    static constexpr CellClass partition_output_cell = 13;
    static constexpr CellClass partition_input_cell = 14;
    static constexpr CellClass partition_input_output_cell = 15;
    static constexpr CellClass inter_partition_stream_cell = 16;
    static constexpr CellClass sink_cell = 17;


    template<typename Policies>
    class AccuInfo:
        public AccumulatingRouterFunctor<
            policy::detail::TypeList<>,
            policy::detail::TypeList<policy::OutputElementT<Policies, 0>>>
    {

        private:

            using Base = AccumulatingRouterFunctor<
                policy::detail::TypeList<>,
                policy::detail::TypeList<policy::OutputElementT<Policies, 0>>>;

        public:

            template<typename... Arguments>
            class CellAccumulator
            {

                    static_assert(sizeof...(Arguments) == 0);

                public:

                    using CellClassNoDataPolicy =
                        policy::OutputNoDataPolicy2T<policy::OutputPoliciesT<Policies, 0>>;

                    using MaterialElement = CellClass;

                    static_assert(std::is_same_v<policy::ElementT<CellClassNoDataPolicy>, CellClass>);

                    using CellClassData = DataT<PartitionedArray<CellClass, 2>>;


                    CellAccumulator(Policies const& policies, CellClassData& cell_class):

                        _ondp_cell_class{std::get<0>(policies.outputs_policies()).output_no_data_policy()},

                        _cell_class_data{cell_class}

                    {
                    }


                    void enter_at_ridge(Index const idx0, Index const idx1)
                    {
                        // Initialize the contents of the cell to propagate downstream
                        _cell_class_data(idx0, idx1) = intra_partition_stream_cell;
                    }


                    void enter_at_partition_input(Index const idx0, Index const idx1)
                    {
                        // Overwrite the contents of the cell to propagate downstream
                        _cell_class_data(idx0, idx1) = inter_partition_stream_cell;
                    }


                    void leave_intra_partition_stream(Index const idx0, Index const idx1)
                    {
                        _cell_class_data(idx0, idx1) = ridge_cell;
                    }


                    void leave_inter_partition_stream(Index const idx0, Index const idx1)
                    {
                        // In rare cases, a cell is both a partition input cell and an partition output cell.
                        if (_cell_class_data(idx0, idx1) != partition_output_cell) [[likely]]
                        {
                            _cell_class_data(idx0, idx1) = partition_input_cell;
                        }
                        else [[unlikely]]
                        {
                            _cell_class_data(idx0, idx1) = partition_input_output_cell;
                        }
                    }


                    void leave_at_output_cell(Index const idx0, Index const idx1)
                    {
                        _cell_class_data(idx0, idx1) = partition_output_cell;
                    }


                    void leave_at_sink_cell(Index const idx0, Index const idx1)
                    {
                        _cell_class_data(idx0, idx1) = sink_cell;
                    }


                    void accumulate_external_inflow(
                        [[maybe_unused]] Index const idx0, [[maybe_unused]] Index const idx1)
                    {
                    }


                    void accumulate_downstream(
                        Index const idx0_from,
                        Index const idx1_from,
                        Index const idx0_to,
                        Index const idx1_to)
                    {
                        CellClass const& upstream_cell_class{_cell_class_data(idx0_from, idx1_from)};

                        CellClass& down_stream_cell_class{_cell_class_data(idx0_to, idx1_to)};

                        if (!_ondp_cell_class.is_no_data(down_stream_cell_class))
                        {
                            if (_ondp_cell_class.is_no_data(upstream_cell_class))
                            {
                                _ondp_cell_class.mark_no_data(down_stream_cell_class);
                            }
                            else
                            {
                                down_stream_cell_class = upstream_cell_class;
                            }
                        }
                    }


                    void accumulate_downstream(
                        CellClass const& cell_class, Index const idx0_to, Index const idx1_to)
                    {
                        CellClass& downstream_cell_class{_cell_class_data(idx0_to, idx1_to)};

                        if (!_ondp_cell_class.is_no_data(downstream_cell_class))
                        {
                            if (_ondp_cell_class.is_no_data(cell_class))
                            {
                                _ondp_cell_class.mark_no_data(downstream_cell_class);
                            }
                            else
                            {
                                downstream_cell_class = cell_class;
                            }
                        }
                    }


                    CellClass const& outflow(Index const idx0, Index const idx1) const
                    {
                        return _cell_class_data(idx0, idx1);
                    }


                    void mark_no_data(Index const idx0, Index const idx1)
                    {
                        _ondp_cell_class.mark_no_data(_cell_class_data(idx0, idx1));
                    }


                private:

                    CellClassNoDataPolicy _ondp_cell_class;

                    CellClassData& _cell_class_data;
            };


            static constexpr char const* name{"accu_info4"};

            //! Type to represent the "material" that is accumulated
            using Material = std::tuple_element_t<0, typename Base::ResultElements>;

            using MaterialPartitions = typename PartitionedArray<Material, 2>::Partitions;
            using MaterialPartition = ArrayPartition<Material, 2>;
            using MaterialData = DataT<MaterialPartition>;

            using IntraPartitionStreamCellsResult = std::tuple<
                hpx::future<MaterialData>,
                hpx::future<typename Base::InflowCountData>,
                hpx::future<std::array<typename Base::CellsIdxs, detail::nr_neighbours<2>()>>>;

            using InterPartitionStreamCellsResult = std::tuple<hpx::future<MaterialData>>;
    };


    template<typename Policies, std::integral FlowDirectionElement>
    auto accu_info4(Policies const& policies, PartitionedArray<FlowDirectionElement, 2> const& flow_direction)
        -> PartitionedArray<CellClass, 2>;


    template<typename Policies, std::integral FlowDirectionElement>
    auto accu_info4(Policies const& policies, PartitionedArray<FlowDirectionElement, 2> const& flow_direction)
        -> PartitionedArray<CellClass, 2>
    {
        return std::get<0>(accumulating_router(policies, AccuInfo<Policies>{}, flow_direction));
    }


    template<typename Policies>
    class AccuThreshold:
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
                    static_assert(std::is_same_v<ElementT<Threshold>, policy::InputElementT<Policies, 2>>);

                    using MaterialElement = policy::ElementT<InflowNoDataPolicy>;

                    static_assert(std::is_same_v<policy::ElementT<InflowNoDataPolicy>, MaterialElement>);
                    static_assert(std::is_same_v<policy::ElementT<ThresholdNoDataPolicy>, MaterialElement>);
                    static_assert(std::is_same_v<policy::ElementT<OutflowNoDataPolicy>, MaterialElement>);
                    static_assert(std::is_same_v<policy::ElementT<RemainderNoDataPolicy>, MaterialElement>);

                    using MaterialData = DataT<PartitionedArray<MaterialElement, 2>>;


                    CellAccumulator(
                        Policies const& policies,
                        Material const& inflow,
                        Threshold const& threshold,
                        MaterialData& outflow,
                        MaterialData& remainder):

                        _dp{policies.domain_policy()},
                        _indp_inflow{std::get<1>(policies.inputs_policies()).input_no_data_policy()},
                        _indp_threshold{std::get<2>(policies.inputs_policies()).input_no_data_policy()},
                        _ondp_outflow{std::get<0>(policies.outputs_policies()).output_no_data_policy()},
                        _ondp_remainder{std::get<1>(policies.outputs_policies()).output_no_data_policy()},

                        _inflow{inflow},
                        _threshold{threshold},
                        _outflow{outflow},
                        _remainder{remainder}

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
                        MaterialElement const& inflow{detail::to_value(_inflow, idx0, idx1)};
                        MaterialElement const& threshold{detail::to_value(_threshold, idx0, idx1)};

                        MaterialElement& outflow{_outflow(idx0, idx1)};
                        MaterialElement& remainder{_remainder(idx0, idx1)};

                        lue_hpx_assert(
                            (_ondp_outflow.is_no_data(outflow) && _ondp_remainder.is_no_data(remainder)) ||
                            (!_ondp_outflow.is_no_data(outflow) && !_ondp_remainder.is_no_data(remainder)));

                        if (!_ondp_outflow.is_no_data(outflow))
                        {
                            if (_indp_inflow.is_no_data(inflow) || _indp_threshold.is_no_data(threshold) ||
                                !_dp.within_domain(inflow, threshold))
                            {
                                _ondp_outflow.mark_no_data(outflow);
                                _ondp_remainder.mark_no_data(remainder);
                            }
                            else
                            {
                                lue_hpx_assert(inflow >= 0);
                                lue_hpx_assert(threshold >= 0);

                                // Now we know the final, total amount
                                // of inflow that enters this cell
                                outflow += inflow;

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

                    Material const _inflow;  // External inflow

                    Threshold const _threshold;

                    MaterialData& _outflow;  // Upstream inflow, outflow

                    MaterialData& _remainder;
            };


            static constexpr char const* name{"accu_threshold4"};

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
    };


    // raster, raster
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_threshold4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        PartitionedArray<MaterialElement, 2> const& material,
        PartitionedArray<MaterialElement, 2> const& threshold)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>;


    // raster, scalar
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_threshold4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        PartitionedArray<MaterialElement, 2> const& material,
        Scalar<MaterialElement> const& threshold)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>;


    // raster, value
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_threshold4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        PartitionedArray<MaterialElement, 2> const& material,
        MaterialElement const& threshold)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>;


    // scalar, scalar
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_threshold4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        Scalar<MaterialElement> const& material,
        Scalar<MaterialElement> const& threshold)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>;


    // scalar, raster
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_threshold4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        Scalar<MaterialElement> const& material,
        PartitionedArray<MaterialElement, 2> const& threshold)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>;


    // scalar, value
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_threshold4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        Scalar<MaterialElement> const& material,
        MaterialElement const& threshold)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>;


    // value, value
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_threshold4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        MaterialElement const& material,
        MaterialElement const& threshold)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>;


    // value, raster
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_threshold4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        MaterialElement const& material,
        PartitionedArray<MaterialElement, 2> const& threshold)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>;


    // value, scalar
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_threshold4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        MaterialElement const& material,
        Scalar<MaterialElement> const& threshold)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>;


    // raster, raster
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_threshold4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        PartitionedArray<MaterialElement, 2> const& material,
        PartitionedArray<MaterialElement, 2> const& threshold)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
    {
        return accumulating_router(policies, AccuThreshold<Policies>{}, flow_direction, material, threshold);
    }


    // raster, scalar
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_threshold4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        PartitionedArray<MaterialElement, 2> const& material,
        Scalar<MaterialElement> const& threshold)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
    {
        return accumulating_router(policies, AccuThreshold<Policies>{}, flow_direction, material, threshold);
    }


    // raster, value
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_threshold4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        PartitionedArray<MaterialElement, 2> const& material,
        MaterialElement const& threshold)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
    {
        return accumulating_router(
            policies,
            AccuThreshold<Policies>{},
            flow_direction,
            material,
            Scalar<MaterialElement>{threshold});
    }


    // scalar, scalar
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_threshold4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        Scalar<MaterialElement> const& material,
        Scalar<MaterialElement> const& threshold)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
    {
        return accumulating_router(policies, AccuThreshold<Policies>{}, flow_direction, material, threshold);
    }


    // scalar, raster
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_threshold4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        Scalar<MaterialElement> const& material,
        PartitionedArray<MaterialElement, 2> const& threshold)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
    {
        return accumulating_router(policies, AccuThreshold<Policies>{}, flow_direction, material, threshold);
    }


    // scalar, value
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_threshold4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        Scalar<MaterialElement> const& material,
        MaterialElement const& threshold)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
    {
        return accumulating_router(
            policies,
            AccuThreshold<Policies>{},
            flow_direction,
            material,
            Scalar<MaterialElement>{threshold});
    }


    // value, value
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_threshold4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        MaterialElement const& material,
        MaterialElement const& threshold)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>

    {
        return accu_threshold4(
            policies, flow_direction, Scalar<MaterialElement>{material}, Scalar<MaterialElement>{threshold});
    }


    // value, raster
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_threshold4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        MaterialElement const& material,
        PartitionedArray<MaterialElement, 2> const& threshold)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
    {
        return accu_threshold4(policies, flow_direction, Scalar<MaterialElement>{material}, threshold);
    }


    // value, scalar
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_threshold4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        MaterialElement const& material,
        Scalar<MaterialElement> const& threshold)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
    {
        return accu_threshold4(policies, flow_direction, Scalar<MaterialElement>{material}, threshold);
    }


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
                        Material const& inflow,
                        Fraction const& fraction,
                        MaterialData& outflow,
                        MaterialData& remainder):

                        _dp{policies.domain_policy()},
                        _indp_inflow{std::get<1>(policies.inputs_policies()).input_no_data_policy()},
                        _indp_fraction{std::get<2>(policies.inputs_policies()).input_no_data_policy()},
                        _ondp_outflow{std::get<0>(policies.outputs_policies()).output_no_data_policy()},
                        _ondp_remainder{std::get<1>(policies.outputs_policies()).output_no_data_policy()},

                        _inflow{inflow},
                        _fraction{fraction},
                        _outflow{outflow},
                        _remainder{remainder}

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
                        MaterialElement const& inflow{detail::to_value(_inflow, idx0, idx1)};
                        MaterialElement const& fraction{detail::to_value(_fraction, idx0, idx1)};

                        MaterialElement& outflow{_outflow(idx0, idx1)};
                        MaterialElement& remainder{_remainder(idx0, idx1)};

                        lue_hpx_assert(
                            (_ondp_outflow.is_no_data(outflow) && _ondp_remainder.is_no_data(remainder)) ||
                            (!_ondp_outflow.is_no_data(outflow) && !_ondp_remainder.is_no_data(remainder)));

                        if (!_ondp_outflow.is_no_data(outflow))
                        {
                            if (_indp_inflow.is_no_data(inflow) || _indp_fraction.is_no_data(fraction) ||
                                !_dp.within_domain(inflow, fraction))
                            {
                                _ondp_outflow.mark_no_data(outflow);
                                _ondp_remainder.mark_no_data(remainder);
                            }
                            else
                            {
                                lue_hpx_assert(inflow >= 0);
                                lue_hpx_assert(fraction >= 0 && fraction <= 1);

                                // Now we know the final, total amount
                                // of inflow that enters this cell
                                outflow += inflow;

                                // Split this amount into outflow and remainder,
                                // based on the fraction passed in
                                MaterialElement delta_flux{fraction * outflow};
                                remainder = outflow - delta_flux;
                                outflow = delta_flux;
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

                    FractionNoDataPolicy _indp_fraction;

                    OutflowNoDataPolicy _ondp_outflow;

                    RemainderNoDataPolicy _ondp_remainder;

                    Material const _inflow;  // External inflow

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
    };


    // raster, raster
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_fraction4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        PartitionedArray<MaterialElement, 2> const& material,
        PartitionedArray<MaterialElement, 2> const& fraction)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>;


    // raster, scalar
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_fraction4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        PartitionedArray<MaterialElement, 2> const& material,
        Scalar<MaterialElement> const& fraction)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>;


    // raster, value
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_fraction4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        PartitionedArray<MaterialElement, 2> const& material,
        MaterialElement const& fraction)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>;


    // scalar, scalar
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_fraction4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        Scalar<MaterialElement> const& material,
        Scalar<MaterialElement> const& fraction)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>;


    // scalar, raster
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_fraction4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        Scalar<MaterialElement> const& material,
        PartitionedArray<MaterialElement, 2> const& fraction)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>;


    // scalar, value
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_fraction4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        Scalar<MaterialElement> const& material,
        MaterialElement const& fraction)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>;


    // value, value
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_fraction4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        MaterialElement const& material,
        MaterialElement const& fraction)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>;


    // value, raster
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_fraction4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        MaterialElement const& material,
        PartitionedArray<MaterialElement, 2> const& fraction)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>;


    // value, scalar
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_fraction4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        MaterialElement const& material,
        Scalar<MaterialElement> const& fraction)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>;


    // raster, raster
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_fraction4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        PartitionedArray<MaterialElement, 2> const& material,
        PartitionedArray<MaterialElement, 2> const& fraction)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
    {
        return accumulating_router(policies, AccuFraction<Policies>{}, flow_direction, material, fraction);
    }


    // raster, scalar
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_fraction4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        PartitionedArray<MaterialElement, 2> const& material,
        Scalar<MaterialElement> const& fraction)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
    {
        return accumulating_router(policies, AccuFraction<Policies>{}, flow_direction, material, fraction);
    }


    // raster, value
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_fraction4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        PartitionedArray<MaterialElement, 2> const& material,
        MaterialElement const& fraction)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
    {
        return accumulating_router(
            policies, AccuFraction<Policies>{}, flow_direction, material, Scalar<MaterialElement>{fraction});
    }


    // scalar, scalar
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_fraction4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        Scalar<MaterialElement> const& material,
        Scalar<MaterialElement> const& fraction)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
    {
        return accumulating_router(policies, AccuFraction<Policies>{}, flow_direction, material, fraction);
    }


    // scalar, raster
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_fraction4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        Scalar<MaterialElement> const& material,
        PartitionedArray<MaterialElement, 2> const& fraction)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
    {
        return accumulating_router(policies, AccuFraction<Policies>{}, flow_direction, material, fraction);
    }


    // scalar, value
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_fraction4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        Scalar<MaterialElement> const& material,
        MaterialElement const& fraction)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
    {
        return accumulating_router(
            policies, AccuFraction<Policies>{}, flow_direction, material, Scalar<MaterialElement>{fraction});
    }


    // value, value
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_fraction4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        MaterialElement const& material,
        MaterialElement const& fraction)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>

    {
        return accu_fraction4(
            policies, flow_direction, Scalar<MaterialElement>{material}, Scalar<MaterialElement>{fraction});
    }


    // value, raster
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_fraction4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        MaterialElement const& material,
        PartitionedArray<MaterialElement, 2> const& fraction)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
    {
        return accu_fraction4(policies, flow_direction, Scalar<MaterialElement>{material}, fraction);
    }


    // value, scalar
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu_fraction4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        MaterialElement const& material,
        Scalar<MaterialElement> const& fraction)
        -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
    {
        return accu_fraction4(policies, flow_direction, Scalar<MaterialElement>{material}, fraction);
    }


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

    }  // namespace detail


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
                    using BetaNoDataPolicy = policy::InputNoDataPolicy2T<policy::InputPoliciesT<Policies, 4>>;
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

                        _indp_current_outflow{std::get<1>(policies.inputs_policies()).input_no_data_policy()},
                        _indp_inflow{std::get<2>(policies.inputs_policies()).input_no_data_policy()},
                        _indp_alpha{std::get<3>(policies.inputs_policies()).input_no_data_policy()},
                        _indp_beta{std::get<4>(policies.inputs_policies()).input_no_data_policy()},
                        _indp_time_step_duration{
                            std::get<5>(policies.inputs_policies()).input_no_data_policy()},
                        _indp_channel_length{std::get<6>(policies.inputs_policies()).input_no_data_policy()},

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
                        MaterialElement const& current_outflow{
                            detail::to_value(_current_outflow, idx0, idx1)};
                        MaterialElement const& inflow{detail::to_value(_inflow, idx0, idx1)};
                        MaterialElement const& alpha{detail::to_value(_alpha, idx0, idx1)};
                        MaterialElement const& beta{detail::to_value(_beta, idx0, idx1)};
                        MaterialElement const& time_step_duration{
                            detail::to_value(_time_step_duration, idx0, idx1)};
                        MaterialElement const& channel_length{detail::to_value(_channel_length, idx0, idx1)};

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

                                outflow = detail::iterate_to_new_discharge(
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


                    void accumulate_downstream(
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
                                // Just add the outflow from upstream to the inflow of the downstream cell
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
                hpx::future<std::array<typename Base::CellsIdxs, detail::nr_neighbours<2>()>>>;

            using InterPartitionStreamCellsResult = std::tuple<hpx::future<MaterialData>>;
    };


    // TODO: Add declarations for kinematic_wave4


    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto kinematic_wave4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        PartitionedArray<MaterialElement, 2> const& current_outflow,
        PartitionedArray<MaterialElement, 2> const& inflow,
        PartitionedArray<MaterialElement, 2> const& alpha,
        PartitionedArray<MaterialElement, 2> const& beta,
        Scalar<MaterialElement> const& time_step_duration,
        PartitionedArray<MaterialElement, 2> const& channel_length) -> PartitionedArray<MaterialElement, 2>
    {
        return std::get<0>(accumulating_router(
            policies,
            KinematicWave<Policies>{},
            flow_direction,
            current_outflow,
            inflow,
            alpha,
            beta,
            time_step_duration,
            channel_length));
    }


    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto kinematic_wave4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        PartitionedArray<MaterialElement, 2> const& current_outflow,
        PartitionedArray<MaterialElement, 2> const& inflow,
        PartitionedArray<MaterialElement, 2> const& alpha,
        PartitionedArray<MaterialElement, 2> const& beta,
        MaterialElement const& time_step_duration,
        PartitionedArray<MaterialElement, 2> const& channel_length) -> PartitionedArray<MaterialElement, 2>
    {
        return kinematic_wave4(
            policies,
            flow_direction,
            current_outflow,
            inflow,
            alpha,
            beta,
            Scalar<MaterialElement>{time_step_duration},
            channel_length);
    }


    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto kinematic_wave4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        PartitionedArray<MaterialElement, 2> const& current_outflow,
        Scalar<MaterialElement> const& inflow,
        Scalar<MaterialElement> const& alpha,
        Scalar<MaterialElement> const& beta,
        Scalar<MaterialElement> const& time_step_duration,
        Scalar<MaterialElement> const& channel_length) -> PartitionedArray<MaterialElement, 2>
    {
        return std::get<0>(accumulating_router(
            policies,
            KinematicWave<Policies>{},
            flow_direction,
            current_outflow,
            inflow,
            alpha,
            beta,
            time_step_duration,
            channel_length));
    }


    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto kinematic_wave4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        PartitionedArray<MaterialElement, 2> const& current_outflow,
        MaterialElement const& inflow,
        MaterialElement const& alpha,
        MaterialElement const& beta,
        MaterialElement const& time_step_duration,
        MaterialElement const& channel_length) -> PartitionedArray<MaterialElement, 2>
    {
        return kinematic_wave4(
            policies,
            flow_direction,
            current_outflow,
            Scalar<MaterialElement>{inflow},
            Scalar<MaterialElement>{alpha},
            Scalar<MaterialElement>{beta},
            Scalar<MaterialElement>{time_step_duration},
            Scalar<MaterialElement>{channel_length});
    }


    namespace policy::accu_info4 {

        /*!
            @brief      Policies to use by accu_info4
            @tparam     FlowDirectionElement Element type used to represent flow directions
        */
        template<std::integral FlowDirectionElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<>,
            OutputElements<CellClass>,
            InputElements<FlowDirectionElement>>;

    }  // namespace policy::accu_info4


    namespace value_policies {

        template<std::integral FlowDirectionElement>
        auto accu_info4(PartitionedArray<FlowDirectionElement, 2> const& flow_direction)
            -> PartitionedArray<CellClass, 2>
        {
            using Policies = lue::policy::accu_info4::DefaultValuePolicies<FlowDirectionElement>;

            return accu_info4(Policies{}, flow_direction);
        }

    }  // namespace value_policies


    namespace policy::accu_threshold4 {

        template<std::floating_point MaterialElement>
        class DomainPolicy
        {

            public:

                static constexpr auto within_domain(
                    MaterialElement const inflow, MaterialElement const threshold) noexcept -> bool
                {
                    return inflow >= 0 && threshold >= 0;
                }
        };


        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            DomainPolicy<MaterialElement>,
            OutputElements<MaterialElement, MaterialElement>,
            InputElements<FlowDirectionElement, MaterialElement, MaterialElement>>;

    }  // namespace policy::accu_threshold4

    namespace value_policies {

        // raster, raster
        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        auto accu_threshold4(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            PartitionedArray<MaterialElement, 2> const& inflow,
            PartitionedArray<MaterialElement, 2> const& threshold)
            -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
        {
            using Policies =
                lue::policy::accu_threshold4::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu_threshold4(Policies{}, flow_direction, inflow, threshold);
        }


        // raster, scalar
        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        auto accu_threshold4(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            PartitionedArray<MaterialElement, 2> const& inflow,
            Scalar<MaterialElement> const& threshold)
            -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
        {
            using Policies =
                lue::policy::accu_threshold4::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu_threshold4(Policies{}, flow_direction, inflow, threshold);
        }


        // raster, value
        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        auto accu_threshold4(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            PartitionedArray<MaterialElement, 2> const& inflow,
            MaterialElement const& threshold)
            -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
        {
            using Policies =
                lue::policy::accu_threshold4::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu_threshold4(Policies{}, flow_direction, inflow, threshold);
        }


        // scalar, scalar
        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        auto accu_threshold4(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            Scalar<MaterialElement> const& inflow,
            Scalar<MaterialElement> const& threshold)
            -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
        {
            using Policies =
                lue::policy::accu_threshold4::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu_threshold4(Policies{}, flow_direction, inflow, threshold);
        }


        // scalar, raster
        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        auto accu_threshold4(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            Scalar<MaterialElement> const& inflow,
            PartitionedArray<MaterialElement, 2> const& threshold)
            -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
        {
            using Policies =
                lue::policy::accu_threshold4::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu_threshold4(Policies{}, flow_direction, inflow, threshold);
        }


        // scalar, value
        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        auto accu_threshold4(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            Scalar<MaterialElement> const& inflow,
            MaterialElement const& threshold)
            -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
        {
            using Policies =
                lue::policy::accu_threshold4::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu_threshold4(Policies{}, flow_direction, inflow, threshold);
        }


        // value, value
        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        auto accu_threshold4(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            MaterialElement const& inflow,
            MaterialElement const& threshold)
            -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
        {
            using Policies =
                lue::policy::accu_threshold4::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu_threshold4(Policies{}, flow_direction, inflow, threshold);
        }

        // value, raster
        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        auto accu_threshold4(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            MaterialElement const& inflow,
            PartitionedArray<MaterialElement, 2> const& threshold)
            -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
        {
            using Policies =
                lue::policy::accu_threshold4::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu_threshold4(Policies{}, flow_direction, inflow, threshold);
        }


        // value, scalar
        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        auto accu_threshold4(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            MaterialElement const& inflow,
            Scalar<MaterialElement> const& threshold)
            -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
        {
            using Policies =
                lue::policy::accu_threshold4::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu_threshold4(Policies{}, flow_direction, inflow, threshold);
        }

    }  // namespace value_policies


    namespace policy::accu_fraction4 {

        template<std::floating_point MaterialElement>
        class DomainPolicy
        {

            public:

                static constexpr auto within_domain(
                    MaterialElement const inflow, MaterialElement const fraction) noexcept -> bool
                {
                    return inflow >= 0 && fraction >= 0 && fraction <= 1;
                }
        };


        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            DomainPolicy<MaterialElement>,
            OutputElements<MaterialElement, MaterialElement>,
            InputElements<FlowDirectionElement, MaterialElement, MaterialElement>>;

    }  // namespace policy::accu_fraction4


    namespace value_policies {

        // raster, raster
        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        auto accu_fraction4(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            PartitionedArray<MaterialElement, 2> const& inflow,
            PartitionedArray<MaterialElement, 2> const& fraction)
            -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
        {
            using Policies =
                lue::policy::accu_fraction4::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu_fraction4(Policies{}, flow_direction, inflow, fraction);
        }


        // raster, scalar
        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        auto accu_fraction4(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            PartitionedArray<MaterialElement, 2> const& inflow,
            Scalar<MaterialElement> const& fraction)
            -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
        {
            using Policies =
                lue::policy::accu_fraction4::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu_fraction4(Policies{}, flow_direction, inflow, fraction);
        }


        // raster, value
        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        auto accu_fraction4(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            PartitionedArray<MaterialElement, 2> const& inflow,
            MaterialElement const& fraction)
            -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
        {
            using Policies =
                lue::policy::accu_fraction4::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu_fraction4(Policies{}, flow_direction, inflow, fraction);
        }


        // scalar, scalar
        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        auto accu_fraction4(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            Scalar<MaterialElement> const& inflow,
            Scalar<MaterialElement> const& fraction)
            -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
        {
            using Policies =
                lue::policy::accu_fraction4::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu_fraction4(Policies{}, flow_direction, inflow, fraction);
        }


        // scalar, raster
        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        auto accu_fraction4(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            Scalar<MaterialElement> const& inflow,
            PartitionedArray<MaterialElement, 2> const& fraction)
            -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
        {
            using Policies =
                lue::policy::accu_fraction4::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu_fraction4(Policies{}, flow_direction, inflow, fraction);
        }


        // scalar, value
        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        auto accu_fraction4(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            Scalar<MaterialElement> const& inflow,
            MaterialElement const& fraction)
            -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
        {
            using Policies =
                lue::policy::accu_fraction4::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu_fraction4(Policies{}, flow_direction, inflow, fraction);
        }


        // value, value
        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        auto accu_fraction4(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            MaterialElement const& inflow,
            MaterialElement const& fraction)
            -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
        {
            using Policies =
                lue::policy::accu_fraction4::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu_fraction4(Policies{}, flow_direction, inflow, fraction);
        }

        // value, raster
        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        auto accu_fraction4(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            MaterialElement const& inflow,
            PartitionedArray<MaterialElement, 2> const& fraction)
            -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
        {
            using Policies =
                lue::policy::accu_fraction4::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu_fraction4(Policies{}, flow_direction, inflow, fraction);
        }


        // value, scalar
        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        auto accu_fraction4(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            MaterialElement const& inflow,
            Scalar<MaterialElement> const& fraction)
            -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
        {
            using Policies =
                lue::policy::accu_fraction4::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu_fraction4(Policies{}, flow_direction, inflow, fraction);
        }

    }  // namespace value_policies


    namespace policy::kinematic_wave4 {

        template<std::floating_point Element>
        class DomainPolicy
        {

            public:

                static constexpr auto within_domain(
                    Element const current_outflow,
                    Element const inflow,
                    Element const channel_length) noexcept -> bool
                {
                    return current_outflow >= 0 && inflow >= 0 && channel_length > 0;
                }
        };


        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            DomainPolicy<MaterialElement>,
            OutputElements<MaterialElement>,
            InputElements<
                FlowDirectionElement,
                MaterialElement,
                MaterialElement,
                MaterialElement,
                MaterialElement,
                MaterialElement,
                MaterialElement>>;

    }  // namespace policy::kinematic_wave4


    namespace value_policies {

        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        auto kinematic_wave4(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            PartitionedArray<MaterialElement, 2> const& current_outflow,
            PartitionedArray<MaterialElement, 2> const& inflow,
            PartitionedArray<MaterialElement, 2> const& alpha,
            PartitionedArray<MaterialElement, 2> const& beta,
            MaterialElement const& time_step_duration,
            PartitionedArray<MaterialElement, 2> const& channel_length)
            -> PartitionedArray<MaterialElement, 2>
        {
            using Policies =
                lue::policy::kinematic_wave4::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return kinematic_wave4(
                Policies{},
                flow_direction,
                current_outflow,
                inflow,
                alpha,
                beta,
                time_step_duration,
                channel_length);
        }


        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        auto kinematic_wave4(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            PartitionedArray<MaterialElement, 2> const& current_outflow,
            MaterialElement const& inflow,
            MaterialElement const& alpha,
            MaterialElement const& beta,
            MaterialElement const& time_step_duration,
            MaterialElement const& channel_length) -> PartitionedArray<MaterialElement, 2>
        {
            using Policies =
                lue::policy::kinematic_wave4::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return kinematic_wave4(
                Policies{},
                flow_direction,
                current_outflow,
                inflow,
                alpha,
                beta,
                time_step_duration,
                channel_length);
        }

    }  // namespace value_policies

}  // namespace lue


BOOST_AUTO_TEST_CASE(accu_info4_overloads)
{
    using FlowDirectionElement = lue::FlowDirectionElement;

    lue::PartitionedArray<FlowDirectionElement, 2> const flow_direction{};

    [[maybe_unused]] lue::PartitionedArray<lue::CellClass, 2> class_ =
        lue::value_policies::accu_info4(flow_direction);
}


BOOST_AUTO_TEST_CASE(accu_info4_all_no_data)
{
    auto const flow_direction = lue::test::all_no_data();

    lue::PartitionedArray<lue::CellClass, 2> const class_we_got =
        lue::value_policies::accu_info4(flow_direction);

    auto const nd_class{lue::policy::no_data_value<lue::CellClass>};

    lue::PartitionedArray<lue::CellClass, 2> class_we_want = lue::create_partitioned_array<lue::CellClass, 2>(
        lue::test::array_shape, lue::test::partition_shape, nd_class);

    lue::test::check_arrays_are_equal(class_we_got, class_we_want);
}


BOOST_AUTO_TEST_CASE(accu_info4_spiral_in)
{
    auto const flow_direction = lue::test::spiral_in();

    lue::PartitionedArray<lue::CellClass, 2> const class_we_got =
        lue::value_policies::accu_info4(flow_direction);

    auto const intra = lue::intra_partition_stream_cell;
    auto const ridge = lue::ridge_cell;
    auto const inter = lue::inter_partition_stream_cell;
    auto const input = lue::partition_input_cell;
    auto const output = lue::partition_output_cell;
    auto const input_output = lue::partition_input_output_cell;
    auto const sink = lue::sink_cell;

    auto const class_we_want = lue::test::create_partitioned_array<lue::PartitionedArray<lue::CellClass, 2>>(
        lue::test::array_shape,
        lue::test::partition_shape,
        // clang-format off
        {
            {
                ridge, intra, output,
                inter, inter, output,
                input, input, output,
            },
            {
                input, inter, output,
                input, inter, output,
                input, inter, output,
            },
            {
                input, inter, inter,
                input, inter, inter,
                input_output, output, output,
            },
            {
                output, output, output,
                inter, inter, inter,
                input, input, input,
            },
            {
                input, inter, inter,
                inter, sink, inter,
                inter, inter, inter,
            },
            {
                input, input, input,
                inter, inter, inter,
                output, output, output,
            },
            {
                output, output, input_output,
                inter, inter, input,
                inter, inter, input,
            },
            {
                output, inter, input,
                output, inter, input,
                output, inter, input,
            },
            {
                input_output, input, input,
                output, inter, inter,
                output, inter, inter,
            },
        }  // clang-format on
    );

    lue::test::check_arrays_are_equal(class_we_got, class_we_want);
}


BOOST_AUTO_TEST_CASE(accu_threshold4_overloads)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    lue::PartitionedArray<FlowDirectionElement, 2> const flow_direction{};
    [[maybe_unused]] lue::PartitionedArray<MaterialElement, 2> outflow{};
    [[maybe_unused]] lue::PartitionedArray<MaterialElement, 2> residue{};

    lue::PartitionedArray<MaterialElement, 2> const inflow_raster{};
    lue::PartitionedArray<MaterialElement, 2> const threshold_raster{};
    lue::Scalar<MaterialElement> const inflow_scalar{};
    lue::Scalar<MaterialElement> const threshold_scalar{};
    MaterialElement const inflow_value{};
    MaterialElement const threshold_value{};

    // raster, raster
    std::tie(outflow, residue) =
        lue::value_policies::accu_threshold4(flow_direction, inflow_raster, threshold_raster);

    // scalar, scalar
    std::tie(outflow, residue) =
        lue::value_policies::accu_threshold4(flow_direction, inflow_scalar, threshold_scalar);

    // value, value
    std::tie(outflow, residue) =
        lue::value_policies::accu_threshold4(flow_direction, inflow_value, threshold_value);

    // raster, scalar
    std::tie(outflow, residue) =
        lue::value_policies::accu_threshold4(flow_direction, inflow_raster, threshold_scalar);

    // raster, value
    std::tie(outflow, residue) =
        lue::value_policies::accu_threshold4(flow_direction, inflow_raster, threshold_value);

    // scalar, raster
    std::tie(outflow, residue) =
        lue::value_policies::accu_threshold4(flow_direction, inflow_scalar, threshold_raster);

    // scalar, value
    std::tie(outflow, residue) =
        lue::value_policies::accu_threshold4(flow_direction, inflow_scalar, threshold_value);

    // value, raster
    std::tie(outflow, residue) =
        lue::value_policies::accu_threshold4(flow_direction, inflow_value, threshold_raster);

    // value, scalar
    std::tie(outflow, residue) =
        lue::value_policies::accu_threshold4(flow_direction, inflow_value, threshold_scalar);
}


BOOST_AUTO_TEST_CASE(accu_threshold4_pcraster_manual_example1)
{
    auto const flow_direction = lue::test::pcraster_example_flow_direction();
    auto const& array_shape{flow_direction.shape()};
    auto const partition_shape{array_shape};

    using MaterialElement = lue::FloatingPointElement<0>;

    auto const inflow = lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                6.0, 0.5, 2.0, 2.0, 2.0,
                0.5, 0.5, 2.0, 2.0, 2.0,
                0.5, 0.5, 2.0, 2.0, 0.0,
                0.5, 0.5, 6.0, 0.0, 0.0,
                0.5, 6.0, 6.0, 6.0, 6.0,
            }
            // clang-format on
            // NOLINTEND
        });

    MaterialElement const threshold = 1.5;

    auto const [outflow_we_got, residue_we_got] =
        lue::value_policies::accu_threshold4(flow_direction, inflow, threshold);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            array_shape,
            partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    4.5, 0.0, 0.5, 0.5, 0.5,
                    3.5, 0.0, 1.5, 1.0, 0.5,
                    2.5, 0.5, 2.5, 1.0, 0.0,
                    0.0, 4.5, 4.5, 0.0, 0.0,
                    0.0, 27.0, 13.5, 9.0, 4.5,
                }
                // clang-format on
                // NOLINTEND
            });

    auto const residue_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            array_shape,
            partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    1.5, 0.5, 1.5, 1.5, 1.5,
                    1.5, 0.5, 1.5, 1.5, 1.5,
                    1.5, 1.5, 1.5, 1.5, 0.0,
                    0.5, 1.5, 1.5, 0.0, 0.0,
                    0.5, 1.5, 1.5, 1.5, 1.5,
                }
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
    lue::test::check_arrays_are_equal(residue_we_got, residue_we_want);
}


BOOST_AUTO_TEST_CASE(accu_threshold4_pcraster_manual_example2)
{
    auto const flow_direction = lue::test::pcraster_example_flow_direction();
    auto const& array_shape{flow_direction.shape()};
    auto const partition_shape{array_shape};

    using MaterialElement = lue::FloatingPointElement<0>;

    auto const inflow = lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                10.0, 10.0, 10.0, 10.0, 10.0,
                10.0, 10.0, 10.0, 10.0, 10.0,
                10.0, 10.0, 10.0, 10.0, 10.0,
                10.0, 10.0, 10.0, 10.0, 50.0,
                50.0, 50.0, 50.0, 50.0, 49.0,
            }
            // clang-format on
            // NOLINTEND
        });

    auto const threshold = lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                0.0, 40.0, 9.0, 9.0, 9.0,
                0.0, 40.0, 9.0, 9.0, 9.0,
                0.0, 40.0, 30.0, 9.0, 9.0,
                0.0, 40.0, 9.0, 9.0, 50.0,
                0.0, 40.0, 40.0, 40.0, 50.0,
            }
            // clang-format on
            // NOLINTEND
        });

    auto const [outflow_we_got, residue_we_got] =
        lue::value_policies::accu_threshold4(flow_direction, inflow, threshold);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            array_shape,
            partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    10.0, 0.0, 1.0, 1.0, 1.0,
                    20.0, 0.0, 3.0, 2.0, 1.0,
                    30.0, 0.0, 0.0, 2.0, 1.0,
                    10.0, 0.0, 3.0, 2.0, 0.0,
                    50.0, 93.0, 20.0, 10.0, 0.0,
                }
                // clang-format on
                // NOLINTEND
            });

    auto const residue_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            array_shape,
            partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    0.0, 10.0, 9.0, 9.0, 9.0,
                    0.0, 10.0, 9.0, 9.0, 9.0,
                    0.0, 13.0, 14.0, 9.0, 9.0,
                    0.0, 40.0, 9.0, 9.0, 50.0,
                    0.0, 40.0, 40.0, 40.0, 49.0,
                }
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
    lue::test::check_arrays_are_equal(residue_we_got, residue_we_want);
}


BOOST_AUTO_TEST_CASE(accu_threshold4_parallel_east)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    auto const flow_direction = lue::create_partitioned_array<FlowDirectionElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, lue::test::e);
    MaterialElement const inflow = 1;
    MaterialElement const threshold = 0;

    auto const [outflow_we_got, residue_we_got] =
        lue::value_policies::accu_threshold4(flow_direction, inflow, threshold);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            lue::test::array_shape,
            lue::test::partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    1, 2, 3,
                    1, 2, 3,
                    1, 2, 3,
                },
                {
                    4, 5, 6,
                    4, 5, 6,
                    4, 5, 6,
                },
                {
                    7, 8, 9,
                    7, 8, 9,
                    7, 8, 9,
                },
                {
                    1, 2, 3,
                    1, 2, 3,
                    1, 2, 3,
                },
                {
                    4, 5, 6,
                    4, 5, 6,
                    4, 5, 6,
                },
                {
                    7, 8, 9,
                    7, 8, 9,
                    7, 8, 9,
                },
                {
                    1, 2, 3,
                    1, 2, 3,
                    1, 2, 3,
                },
                {
                    4, 5, 6,
                    4, 5, 6,
                    4, 5, 6,
                },
                {
                    7, 8, 9,
                    7, 8, 9,
                    7, 8, 9,
                }
                // clang-format on
                // NOLINTEND
            });
    auto const residue_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            lue::test::array_shape,
            lue::test::partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    0, 0, 0,
                    0, 0, 0,
                    0, 0, 0,
                },
                {
                    0, 0, 0,
                    0, 0, 0,
                    0, 0, 0,
                },
                {
                    0, 0, 0,
                    0, 0, 0,
                    0, 0, 0,
                },
                {
                    0, 0, 0,
                    0, 0, 0,
                    0, 0, 0,
                },
                {
                    0, 0, 0,
                    0, 0, 0,
                    0, 0, 0,
                },
                {
                    0, 0, 0,
                    0, 0, 0,
                    0, 0, 0,
                },
                {
                    0, 0, 0,
                    0, 0, 0,
                    0, 0, 0,
                },
                {
                    0, 0, 0,
                    0, 0, 0,
                    0, 0, 0,
                },
                {
                    0, 0, 0,
                    0, 0, 0,
                    0, 0, 0,
                }
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
    lue::test::check_arrays_are_equal(residue_we_got, residue_we_want);
}


BOOST_AUTO_TEST_CASE(accu_fraction4_overloads)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    lue::PartitionedArray<FlowDirectionElement, 2> const flow_direction{};
    [[maybe_unused]] lue::PartitionedArray<MaterialElement, 2> outflow{};
    [[maybe_unused]] lue::PartitionedArray<MaterialElement, 2> residue{};

    lue::PartitionedArray<MaterialElement, 2> const inflow_raster{};
    lue::PartitionedArray<MaterialElement, 2> const fraction_raster{};
    lue::Scalar<MaterialElement> const inflow_scalar{};
    lue::Scalar<MaterialElement> const fraction_scalar{};
    MaterialElement const inflow_value{};
    MaterialElement const fraction_value{};

    // raster, raster
    std::tie(outflow, residue) =
        lue::value_policies::accu_fraction4(flow_direction, inflow_raster, fraction_raster);

    // scalar, scalar
    std::tie(outflow, residue) =
        lue::value_policies::accu_fraction4(flow_direction, inflow_scalar, fraction_scalar);

    // value, value
    std::tie(outflow, residue) =
        lue::value_policies::accu_fraction4(flow_direction, inflow_value, fraction_value);

    // raster, scalar
    std::tie(outflow, residue) =
        lue::value_policies::accu_fraction4(flow_direction, inflow_raster, fraction_scalar);

    // raster, value
    std::tie(outflow, residue) =
        lue::value_policies::accu_fraction4(flow_direction, inflow_raster, fraction_value);

    // scalar, raster
    std::tie(outflow, residue) =
        lue::value_policies::accu_fraction4(flow_direction, inflow_scalar, fraction_raster);

    // scalar, value
    std::tie(outflow, residue) =
        lue::value_policies::accu_fraction4(flow_direction, inflow_scalar, fraction_value);

    // value, raster
    std::tie(outflow, residue) =
        lue::value_policies::accu_fraction4(flow_direction, inflow_value, fraction_raster);

    // value, scalar
    std::tie(outflow, residue) =
        lue::value_policies::accu_fraction4(flow_direction, inflow_value, fraction_scalar);
}


BOOST_AUTO_TEST_CASE(accu_fraction4_spiral_in_case)
{
    // accu_fraction(flow_direction, material, ones) == accu(flow_direction, material)
    using MaterialElement = lue::FloatingPointElement<0>;

    auto const flow_direction = lue::test::spiral_in();
    auto const inflow = lue::test::ones<MaterialElement>();
    auto const fraction = lue::test::ones<MaterialElement>();

    auto const [outflow_we_got, residue_we_got] =
        lue::value_policies::accu_fraction4(flow_direction, inflow, fraction);

    auto outflow_we_want = lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
        lue::test::array_shape,
        lue::test::partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                1, 2, 3,
                32, 33, 34,
                31, 56, 57,
            },
            {
                4, 5, 6,
                35, 36, 37,
                58, 59, 60,
            },
            {
                7, 8, 9,
                38, 39, 10,
                61, 40, 11,
            },
            {
                30, 55, 72,
                29, 54, 71,
                28, 53, 70,
            },
            {
                73, 74, 75,
                80, 81, 76,
                79, 78, 77,
            },
            {
                62, 41, 12,
                63, 42, 13,
                64, 43, 14,
            },
            {
                27, 52, 69,
                26, 51, 50,
                25, 24, 23,
            },
            {
                68, 67, 66,
                49, 48, 47,
                22, 21, 20,
            },
            {
                65, 44, 15,
                46, 45, 16,
                19, 18, 17,
            },
            // clang-format on
            // NOLINTEND
        });
    auto const residue_we_want = lue::test::zeros<MaterialElement>();

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
    lue::test::check_arrays_are_equal(residue_we_got, residue_we_want);
}


BOOST_AUTO_TEST_CASE(accu_fraction4_merging_streams_case_01)
{
    // No-data material in ridge cell. From there on, it must propagate down to all downstream cells, in all
    // downstream partitions. Similar for a no-data fraction in ridge cell.

    using MaterialElement = lue::FloatingPointElement<0>;

    auto const flow_direction = lue::test::merging_streams();

    auto const x{lue::policy::no_data_value<MaterialElement>};
    auto const inflow = lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
        lue::test::array_shape,
        lue::test::partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                // 0, 0
                1, 1, 1,
                1, 1, 1,
                1, 1, 1,
            },
            {
                // 0, 1
                1, 1, 1,
                1, 1, 1,
                1, 1, 1,
            },
            {
                // 0, 2
                1, x, 1,
                1, 1, 1,
                1, 1, 1,
            },
            {
                // 1, 0
                1, 1, 1,
                1, 1, 1,
                1, 1, 1,
            },
            {
                // 1, 1
                1, 1, 1,
                1, 1, 1,
                1, 1, 1,
            },
            {
                // 1, 2
                1, 1, 1,
                1, 1, 1,
                1, 1, 1,
            },
            {
                // 2, 0
                1, 1, 1,
                1, 1, 1,
                1, 1, 1,
            },
            {
                // 2, 1
                1, 1, 1,
                1, 1, 1,
                1, 1, 1,
            },
            {
                // 2, 2
                1, 1, 1,
                1, 1, 1,
                1, 1, 1,
            }
            // clang-format on
            // NOLINTEND
        });
    auto const fraction = lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
        lue::test::array_shape,
        lue::test::partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                // 0, 0
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
            },
            {
                // 0, 1
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
            },
            {
                // 0, 2
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
            },
            {
                // 1, 0
                0.5, 0.5, 0.5,
                0.5, 0.5, x,
                0.5, 0.5, 0.5,
            },
            {
                // 1, 1
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
            },
            {
                // 1, 2
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
            },
            {
                // 2, 0
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
            },
            {
                // 2, 1
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
            },
            {
                // 2, 2
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
            },
            // clang-format on
            // NOLINTEND
        });

    auto const [outflow_we_got, residue_we_got] =
        lue::value_policies::accu_fraction4(flow_direction, inflow, fraction);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            lue::test::array_shape,
            lue::test::partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    // 0, 0
                    x, x, x,
                    x, x, x,
                    x, x, 0.50,
                },
                {
                    // 0, 1
                    x, 0.50, 0.50,
                    x, 0.50, 0.50,
                    x, 0.75, x,
                },
                {
                    // 0, 2
                    x, x, 0.50,
                    1.00, x, 0.50,
                    x, x, 0.50,
                },
                {
                    // 1, 0
                    x, x, 0.50,
                    x, x, x,
                    x, x, 0.50,
                },
                {
                    // 1, 1
                    x, x, x,
                    x, x, 0.75,
                    0.75, x, 1,
                },
                {
                    // 1, 2
                    x, x, x,
                    0.50, x, x,
                    0.50, x, x,
                },
                {
                    // 2, 0
                    x, x, x,
                    x, x, x,
                    x, x, x,
                },
                {
                    // 2, 1
                    x, x, x,
                    x, x, 0.75,
                    0.9375, 0.875, 0.75,
                },
                {
                    // 2, 2
                    0.50, x, x,
                    0.50, x, x,
                    0.50, x, x,
                }
                // clang-format on
                // NOLINTEND
            });
    auto const residue_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            lue::test::array_shape,
            lue::test::partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    // 0, 0
                    x, x, x,
                    x, x, x,
                    x, x, 0.5,
                },
                {
                    // 0, 1
                    x, 0.50, 0.50,
                    x, 0.50, 0.50,
                    x, 0.75, x,
                },
                {
                    // 0, 2
                    x, x, 0.50,
                    1.00, x, 0.50,
                    x, x, 0.50,
                },
                {
                    // 1, 0
                    x, x, 0.50,
                    x, x, x,
                    x, x, 0.50,
                },
                {
                    // 1, 1
                    x, x, x,
                    x, x, 0.75,
                    0.75, x, 1,
                },
                {
                    // 1, 2
                    x, x, x,
                    0.50, x, x,
                    0.50, x, x,
                },
                {
                    // 2, 0
                    x, x, x,
                    x, x, x,
                    x, x, x,
                },
                {
                    // 2, 1
                    x, x, x,
                    x, x, 0.75,
                    0.9375, 0.875, 0.75,
                },
                {
                    // 2, 2
                    0.50, x, x,
                    0.50, x, x,
                    0.50, x, x,
                },
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
    lue::test::check_arrays_are_equal(residue_we_got, residue_we_want);
}


BOOST_AUTO_TEST_CASE(kinematic_wave4_overloads)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    lue::PartitionedArray<FlowDirectionElement, 2> const flow_direction{};
    [[maybe_unused]] lue::PartitionedArray<MaterialElement, 2> outflow{};
    [[maybe_unused]] lue::PartitionedArray<MaterialElement, 2> residue{};

    // TODO: Which combinations make sense and which don't?
    // TODO: Revisit all routing overloads: supporting scalars only makes sense if these arguments are
    //       typically computed. Don't support them if that does not make sense.
    lue::PartitionedArray<MaterialElement, 2> const current_outflow_raster{};
    lue::PartitionedArray<MaterialElement, 2> const inflow_raster{};
    lue::PartitionedArray<MaterialElement, 2> const alpha_raster{};
    lue::PartitionedArray<MaterialElement, 2> const beta_raster{};
    lue::PartitionedArray<MaterialElement, 2> const channel_length_raster{};

    MaterialElement const inflow_value{};
    MaterialElement const alpha_value{};
    MaterialElement const beta_value{};
    MaterialElement const time_step_duration_value{};
    MaterialElement const channel_length_value{};

    {
        auto const outflow = lue::value_policies::kinematic_wave4(
            flow_direction,
            current_outflow_raster,
            inflow_raster,
            alpha_raster,
            beta_raster,
            time_step_duration_value,
            channel_length_raster);
    }

    {
        auto const outflow = lue::value_policies::kinematic_wave4(
            flow_direction,
            current_outflow_raster,
            inflow_value,
            alpha_value,
            beta_value,
            time_step_duration_value,
            channel_length_value);
    }
}


// namespace tt = boost::test_tools;


BOOST_AUTO_TEST_CASE(kinematic_wave4_pcraster_manual_example)
{
    // https://pcraster.geo.uu.nl/pcraster/4.4.1/documentation/pcraster_manual/sphinx/op_kinematic.html
    auto const flow_direction = lue::test::pcraster_example_flow_direction();
    auto const& array_shape{flow_direction.shape()};
    auto const partition_shape{array_shape};

    using MaterialElement = lue::FloatingPointElement<0>;

    auto const current_outflow =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            array_shape,
            partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    10, 10, 10, 10, 10,
                    10, 10, 10, 10, 10,
                    10, 10, 10, 10, 10,
                    10, 10, 10, 10, 50,
                    50, 50, 50, 50, 49,
                }
                // clang-format on
                // NOLINTEND
            });

    MaterialElement const inflow{0};
    MaterialElement const alpha{1.5};
    MaterialElement const beta{0.6};
    MaterialElement const time_step_duration{15};
    MaterialElement const channel_length{10};

    auto const outflow_we_got = lue::value_policies::kinematic_wave4(
        flow_direction, current_outflow, inflow, alpha, beta, time_step_duration, channel_length);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            array_shape,
            partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    2.32293,  2.32293,  2.32293,  2.32293, 2.32293,
                    4.00491,  4.00491,  5.76591,  4.00491, 2.32293,
                    5.27401,  9.81514,  8.40418,  4.00491, 2.32293,
                    2.32293, 21.22170,  9.68365,  9.60760, 7.19019,
                    7.19019, 57.27763, 17.74742, 12.90772, 7.09124,
                }
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_close(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(kinematic_wave4_zero_discharge_and_inflow)
{
    auto const flow_direction = lue::test::pcraster_example_flow_direction();
    auto const& array_shape{flow_direction.shape()};
    auto const partition_shape{array_shape};

    using MaterialElement = lue::FloatingPointElement<0>;

    auto const current_outflow =
        lue::create_partitioned_array<MaterialElement>(array_shape, partition_shape, 0);
    MaterialElement const inflow{0};
    MaterialElement const alpha{1.5};
    MaterialElement const beta{0.6};
    MaterialElement const time_step_duration{15};
    MaterialElement const channel_length{10};

    auto const outflow_we_got = lue::value_policies::kinematic_wave4(
        flow_direction, current_outflow, inflow, alpha, beta, time_step_duration, channel_length);

    auto const outflow_we_want =
        lue::create_partitioned_array<MaterialElement>(array_shape, partition_shape, 0);

    lue::test::check_arrays_are_close(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(kinematic_wave4_non_zero_discharge_and_zero_inflow)
{
    auto const flow_direction = lue::test::pcraster_example_flow_direction();
    auto const& array_shape{flow_direction.shape()};
    auto const partition_shape{array_shape};

    using MaterialElement = lue::FloatingPointElement<0>;

    auto const current_outflow =
        lue::create_partitioned_array<MaterialElement>(array_shape, partition_shape, 1);
    MaterialElement const inflow{0};
    MaterialElement const alpha{1.5};
    MaterialElement const beta{0.6};
    MaterialElement const time_step_duration{15};
    MaterialElement const channel_length{10};

    auto const outflow_we_got = lue::value_policies::kinematic_wave4(
        flow_direction, current_outflow, inflow, alpha, beta, time_step_duration, channel_length);

    using namespace lue::value_policies;

    BOOST_CHECK(lue::value_policies::all(outflow_we_got > MaterialElement{0}).future().get());
}


BOOST_AUTO_TEST_CASE(kinematic_wave4_zero_discharge_and_non_zero_inflow)
{
    auto const flow_direction = lue::test::pcraster_example_flow_direction();
    auto const& array_shape{flow_direction.shape()};
    auto const partition_shape{array_shape};

    using MaterialElement = lue::FloatingPointElement<0>;

    auto const current_outflow =
        lue::create_partitioned_array<MaterialElement>(array_shape, partition_shape, 0);
    MaterialElement const inflow{1};
    MaterialElement const alpha{1.5};
    MaterialElement const beta{0.6};
    MaterialElement const time_step_duration{15};
    MaterialElement const channel_length{10};

    auto const outflow_we_got = lue::value_policies::kinematic_wave4(
        flow_direction, current_outflow, inflow, alpha, beta, time_step_duration, channel_length);

    using namespace lue::value_policies;

    BOOST_CHECK(lue::value_policies::all(outflow_we_got > MaterialElement{0}).future().get());
}


// TODO: Add tests for extreme inputs:
// - What about "wrong" values for alpha, beta, time_step_duration?


BOOST_AUTO_TEST_CASE(kinematic_wave4_dry_cell)
{
    using FloatingPoint = lue::FloatingPointElement<0>;

    {
        FloatingPoint const outflow_we_got{lue::detail::iterate_to_new_discharge<FloatingPoint>(
            0,     // upstream_discharge
            0,     // current_outflow
            0,     // lateral_inflow
            1.5,   // alpha
            0.6,   // beta
            15,    // time_step_duration
            10)};  // channel_length
        FloatingPoint const outflow_we_want{0};

        BOOST_TEST(outflow_we_got == outflow_we_want, tt::tolerance(1e-6));
    }

    {
        FloatingPoint const outflow_we_got{lue::detail::iterate_to_new_discharge<FloatingPoint>(
            1,     // upstream_discharge
            0,     // current_outflow
            -1,    // lateral_inflow
            1.5,   // alpha
            0.6,   // beta
            15,    // time_step_duration
            10)};  // channel_length
        FloatingPoint const outflow_we_want{0};

        BOOST_TEST(outflow_we_got == outflow_we_want, tt::tolerance(1e-6));
    }

    {
        FloatingPoint const outflow_we_got{lue::detail::iterate_to_new_discharge<FloatingPoint>(
            1,     // upstream_discharge
            1,     // current_outflow
            -2,    // lateral_inflow
            1.5,   // alpha
            0.6,   // beta
            15,    // time_step_duration
            10)};  // channel_length
        FloatingPoint const outflow_we_want{0};

        BOOST_TEST(outflow_we_got == outflow_we_want, tt::tolerance(1e-6));
    }

    {
        FloatingPoint const outflow_we_got{lue::detail::iterate_to_new_discharge<FloatingPoint>(
            0,     // upstream_discharge
            1,     // current_outflow
            -1,    // lateral_inflow
            1.5,   // alpha
            0.6,   // beta
            15,    // time_step_duration
            10)};  // channel_length
        FloatingPoint const outflow_we_want{0};

        BOOST_TEST(outflow_we_got == outflow_we_want, tt::tolerance(1e-6));
    }

    {
        FloatingPoint const outflow_we_got{lue::detail::iterate_to_new_discharge<FloatingPoint>(
            0,     // upstream_discharge
            0,     // current_outflow
            -1,    // lateral_inflow
            1.5,   // alpha
            0.6,   // beta
            15,    // time_step_duration
            10)};  // channel_length
        FloatingPoint const outflow_we_want{0};

        BOOST_TEST(outflow_we_got == outflow_we_want, tt::tolerance(1e-6));
    }
}


// BOOST_AUTO_TEST_CASE(kinematic_wave4_crashed_in_pcraster1)
// {
//     double const outflow_we_got{lue::detail::iterate_to_new_discharge<double>(
//         0.000201343,   // upstream_discharge
//         0.000115866,   // current_outflow
//         -0.000290263,  // lateral_inflow
//         1.73684,       // alpha
//         0.6,           // beta
//         15,            // time_step_duration
//         10)};          // channel_length
//     // /* epsilon */ 1E-12);  // epsilon
//     double const outflow_we_want{0.000031450866300937};
//
//     BOOST_TEST(outflow_we_got == outflow_we_want, tt::tolerance(1e-6));
// }


BOOST_AUTO_TEST_CASE(kinematic_wave4_crashed_in_pcraster2)
{
    if constexpr (lue::arithmetic_element_supported<double>)
    {
        // TODO Port to all float types
        using FloatingPoint = double;

        FloatingPoint const outflow_we_got{lue::detail::iterate_to_new_discharge<FloatingPoint>(
            0,             // upstream_discharge
            1.11659e-07,   // current_outflow
            -1.32678e-05,  // lateral_inflow
            1.6808,        // alpha
            0.6,           // beta
            15,            // time_step_duration
            10)};          // channel_length

        FloatingPoint const outflow_we_want{std::numeric_limits<FloatingPoint>::min()};

        BOOST_TEST(outflow_we_got == outflow_we_want, tt::tolerance(1e-6));
    }
}


BOOST_AUTO_TEST_CASE(kinematic_wave4_random_input)
{
    using FloatingPoint = lue::FloatingPointElement<0>;

    std::random_device random_device{};
    std::default_random_engine random_number_engine(random_device());

    std::uniform_real_distribution<FloatingPoint> discharge_distribution{0, 1000};
    std::uniform_real_distribution<FloatingPoint> lateral_inflow_distribution{-1000, 1000};
    std::uniform_real_distribution<FloatingPoint> alpha_distribution{0.5, 6.0};
    std::uniform_real_distribution<FloatingPoint> beta_distribution{0.5, 2.0};
    std::uniform_real_distribution<FloatingPoint> time_step_duration_distribution{1, 100};
    std::uniform_real_distribution<FloatingPoint> channel_length_distribution{1, 100};

    for (std::size_t i = 0; i < 10000; ++i)
    {
        FloatingPoint const upstream_discharge{discharge_distribution(random_number_engine)};
        FloatingPoint const current_outflow{discharge_distribution(random_number_engine)};
        FloatingPoint const alpha{alpha_distribution(random_number_engine)};
        FloatingPoint const beta{beta_distribution(random_number_engine)};
        FloatingPoint const time_step_duration{time_step_duration_distribution(random_number_engine)};
        FloatingPoint const channel_length{channel_length_distribution(random_number_engine)};

        FloatingPoint const lateral_inflow{
            lateral_inflow_distribution(random_number_engine) / channel_length};

        FloatingPoint outflow_we_got{-1};

        BOOST_TEST_INFO(
            std::format(
                "upstream_discharge: {}\n"
                "current_outflow: {}\n"
                "lateral_inflow: {}\n"
                "alpha: {}\n"
                "beta: {}\n"
                "time_step_duration: {}\n"
                "channel_length: {}\n",
                upstream_discharge,
                current_outflow,
                lateral_inflow,
                alpha,
                beta,
                time_step_duration,
                channel_length));

        // This call should not throw an exception
        outflow_we_got = lue::detail::iterate_to_new_discharge<FloatingPoint>(
            upstream_discharge,
            current_outflow,
            lateral_inflow,
            alpha,
            beta,
            time_step_duration,
            channel_length);

        BOOST_CHECK(outflow_we_got >= 0);
    }
}
