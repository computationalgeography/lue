#pragma once
#include "lue/framework/algorithm/accu_info.hpp"
#include "lue/framework/algorithm/definition/accumulating_router.hpp"
#include "lue/framework/algorithm/routing_operation_export.hpp"
#include "lue/macro.hpp"


namespace lue {

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
                        // In rare cases, a cell is both a partition input cell and an partition output
                        // cell.
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


            static constexpr char const* name{"accu_info"};

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


    template<typename Policies>
    auto accu_info(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& flow_direction)
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>
    {
        return std::get<0>(accumulating_router(policies, AccuInfo<Policies>{}, flow_direction));
    }

}  // namespace lue


#define LUE_INSTANTIATE_ACCU_INFO(Policies)                                                                  \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT auto accu_info<ArgumentType<void(Policies)>>(                      \
        ArgumentType<void(Policies)> const&, PartitionedArray<policy::InputElementT<Policies, 0>, 2> const&) \
        -> PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;
