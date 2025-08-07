#pragma once
#include "lue/framework/algorithm/accu_info.hpp"
#include "lue/framework/algorithm/component/array_partition_io.hpp"
#include "lue/framework/algorithm/detail/accumulate.hpp"
#include "lue/framework/algorithm/inflow_count.hpp"
#include "lue/framework/algorithm/policy/flow_direction_halo.hpp"
#include "lue/framework/algorithm/policy/mark_no_data_by_value.hpp"
#include "lue/framework/algorithm/type_traits.hpp"
#include "lue/framework/core/annotate.hpp"
#include "lue/framework/core/component/component_array.hpp"


namespace lue {

    using StreamClass = CellClass;
    // static constexpr StreamClass intra_partition_stream_cell = 0;
    // static constexpr StreamClass inter_partition_stream_cell = 1;


    template<typename Index, Rank rank>
    class InterPartitionStreamMaterial
    {

        public:

            using CellIdxs = lue::Indices<Index, rank>;
            using CellsIdxs = std::vector<CellIdxs>;

            using Count = CountElement;


            InterPartitionStreamMaterial():

                InterPartitionStreamMaterial{{}, intra_partition_stream_cell, 0}

            {
            }


            InterPartitionStreamMaterial(std::tuple<CellsIdxs, StreamClass, Count> const& material):

                InterPartitionStreamMaterial{
                    CellsIdxs{std::get<0>(material)}, std::get<1>(material), std::get<2>(material)}

            {
            }


            InterPartitionStreamMaterial(
                CellsIdxs&& input_cells_idxs, StreamClass const stream_class, Count const count):

                _input_cells_idxs{std::move(input_cells_idxs)},
                _stream_class{stream_class},
                _count{count}

            {
                lue_hpx_assert(
                    _stream_class == intra_partition_stream_cell ||
                    _stream_class == inter_partition_stream_cell);
            }


            auto input_cells_idxs() const -> CellsIdxs const&
            {
                return _input_cells_idxs;
            }


            StreamClass stream_class() const
            {
                return _stream_class;
            }


            Count count() const
            {
                return _count;
            }


        private:

            friend class hpx::serialization::access;


            void serialize(
                hpx::serialization::input_archive& archive, [[maybe_unused]] unsigned int const version)
            {
                archive & _input_cells_idxs & _stream_class & _count;
            }


            void serialize(
                hpx::serialization::output_archive& archive,
                [[maybe_unused]] unsigned int const version) const
            {
                archive & _input_cells_idxs & _stream_class & _count;
            }


            CellsIdxs _input_cells_idxs;

            StreamClass _stream_class;

            Count _count;
    };


    namespace detail {
        namespace inter_partition_stream {

            template<typename CellsIdxs>
            class InputMaterial
            {

                public:

                    using CellIdxs = typename CellsIdxs::value_type;

                    using MaterialElement = CellsIdxs;


                    InputMaterial(CellsIdxs const& input_cell_idxs):

                        _count_by_cell_idxs{}

                    {
                        {
                            lue_hpx_assert(std::is_sorted(input_cell_idxs.begin(), input_cell_idxs.end()));

                            Count count;

                            for (auto it = input_cell_idxs.begin(); it != input_cell_idxs.end(); ++it)
                            {
                                // Figure out how many cells in neighbouring
                                // partitions flow into each input cell
                                count = 1;
                                while (it + 1 != input_cell_idxs.end() && *(it + 1) == *it)
                                {
                                    ++count;
                                    ++it;
                                }

                                lue_hpx_assert(_count_by_cell_idxs.count(*it) == 0);
                                _count_by_cell_idxs.emplace(std::make_pair(*it, count));
                            }
                        }
                    }


                    Count inflow_count(CellIdxs const input_cell_idxs) const
                    {
                        lue_hpx_assert(_count_by_cell_idxs.count(input_cell_idxs) == 1);

                        return _count_by_cell_idxs.at(input_cell_idxs);
                    }


                    CellsIdxs operator()(Index const idx0, Index const idx1) const
                    {
                        // If idx0, idx1 is an input cell, return a
                        // collection of count IDs. Otherwise return an
                        // empty collection.

                        CellsIdxs cell_idxs{};

                        if (auto it = _count_by_cell_idxs.find({idx0, idx1}); it != _count_by_cell_idxs.end())
                        {
                            auto const& [idxs, count] = *it;
                            cell_idxs.insert(cell_idxs.begin(), count, idxs);
                        }

                        return cell_idxs;
                    }


                private:

                    // For each input cell idxs the inflow count (from
                    // neighbouring partitions)
                    std::map<CellIdxs, Count> _count_by_cell_idxs;
            };


            template<typename CellsIdxsArray, typename StreamClassArray, typename CountArray>
            class OutputMaterial
            {

                public:

                    using CellsIdxsElement = ElementT<CellsIdxsArray>;
                    using StreamClassElement = ElementT<StreamClassArray>;
                    using CountElement = ElementT<CountArray>;
                    using MaterialElement = std::tuple<CellsIdxsElement&, StreamClassElement&, CountElement&>;

                    using Element = std::tuple<CellsIdxsElement, StreamClassElement, CountElement>;

                    OutputMaterial(
                        CellsIdxsArray& cells_idxs, StreamClassArray& stream_class, CountArray& count):

                        _cells_idxs{cells_idxs},
                        _stream_class{stream_class},
                        _count{count}

                    {
                    }


                    MaterialElement operator()(Index const idx0, Index const idx1)
                    {
                        return std::forward_as_tuple(
                            _cells_idxs(idx0, idx1), _stream_class(idx0, idx1), _count(idx0, idx1));
                    }

                private:

                    CellsIdxsArray& _cells_idxs;

                    StreamClassArray& _stream_class;

                    CountArray& _count;
            };


            template<typename CellsIdxs, typename StreamClass, typename Count>
            class CellAccumulator
            {

                public:

                    using InputMaterialElement = std::vector<std::array<Index, 2>>;

                    using OutputMaterialElement = std::tuple<CellsIdxs&, StreamClass&, Count&>;


                    static void accumulate_input(
                        InputMaterialElement const& input_material_element,
                        OutputMaterialElement output_material_element)
                    {
                        // The input_material_element contains a collection
                        // of input cell IDs. Append them to the
                        // collection already present in the
                        // output_material_element.

                        auto& input_cell_idxs = input_material_element;
                        auto& [output_cell_idxs, output_stream_class, output_count] = output_material_element;

                        // Copy all cell indices from the source collection to the
                        // destination collection
                        output_cell_idxs.insert(
                            output_cell_idxs.end(), input_cell_idxs.begin(), input_cell_idxs.end());

                        // Stream class is already set in the output by
                        // the caller (ridge cells and partition input cells),
                        // and forwarded downstream from upstream (all
                        // other cells).
                        lue_hpx_assert(
                            output_stream_class == intra_partition_stream_cell ||
                            output_stream_class == inter_partition_stream_cell);

                        // Count is already set in the output by the
                        // caller (ridge cells and partition input cells),
                        // and accumulated downstream from upstream
                        // (all other cells).
                        lue_hpx_assert(output_count >= 1);
                    }


                    static void accumulate_downstream(
                        OutputMaterialElement const /* & */ upstream_material_element,
                        OutputMaterialElement /* & */ downstream_material_element)
                    {
                        // Input cell IDS: merge the collections
                        // Stream class: forward the upstream class
                        // Count: sum

                        auto& [upstream_cell_idxs, upstream_stream_class, upstream_count] =
                            upstream_material_element;
                        lue_hpx_assert(
                            upstream_stream_class == intra_partition_stream_cell ||
                            upstream_stream_class == inter_partition_stream_cell);
                        lue_hpx_assert(upstream_count >= 1);

                        auto& [downstream_cell_idxs, downstream_stream_class, downstream_count] =
                            downstream_material_element;
                        lue_hpx_assert(downstream_count >= 1);

                        downstream_cell_idxs.insert(
                            downstream_cell_idxs.end(), upstream_cell_idxs.begin(), upstream_cell_idxs.end());

                        downstream_stream_class = upstream_stream_class;
                        downstream_count += upstream_count;
                    }
            };

        }  // namespace inter_partition_stream


        template<typename CellsIdxsArray, typename StreamClassArray, typename CountArray>
        class ArrayTraits<
            inter_partition_stream::OutputMaterial<CellsIdxsArray, StreamClassArray, CountArray>>
        {

            public:

                using Element = typename inter_partition_stream::
                    OutputMaterial<CellsIdxsArray, StreamClassArray, CountArray>::Element;
        };


        template<
            typename Policies,
            typename StreamClassPartition,
            typename PartitionIOComponent,
            typename FlowDirectionPartitions>
        hpx::tuple<StreamClassPartition, PartitionIOComponent> inter_partition_stream_partition(
            Policies const& policies,
            OffsetT<PartitionT<FlowDirectionPartitions>> const& partition_offset,
            FlowDirectionPartitions const& flow_direction_partitions)
        {
            // Request the offset in the overall array of the current partition
            using Offset = OffsetT<PartitionT<FlowDirectionPartitions>>;
            hpx::future<Offset> offset{flow_direction_partitions(1, 1).offset()};

            // Request shapes of all partitions
            auto const partition_shapes = detail::partition_shapes(flow_direction_partitions);

            // Request the minimum amount of required data from all partitions
            auto flow_direction_data = partition_data(flow_direction_partitions, partition_shapes);


            using FlowDirectionPartition = PartitionT<FlowDirectionPartitions>;
            using FlowDirectionData = DataT<FlowDirectionPartition>;
            using FlowDirectionElement = ElementT<FlowDirectionPartitions>;

            using InflowCountElement = SmallestIntegralElement;
            using InflowCountPartition = PartitionT<FlowDirectionPartition, InflowCountElement>;
            using InflowCountData = DataT<InflowCountPartition>;

            using StreamClassData = DataT<StreamClassPartition>;

            using PartitionIOData = typename PartitionIOComponent::Data;

            using Shape = ShapeT<InflowCountData>;

            Rank const rank{lue::rank<FlowDirectionData>};

            // The stuff we want to forward through the flow direction network:
            // - Cell indices of partition input cells. These end up at
            //     the partition output cells.
            // - Stream class code:
            //     - intra-partition stream cell
            //     - inter-partition stream cell
            // - Count of cells that flow into a cell

            // We first start accumulating 'material' from cells with
            // an inflow count of zero (ridges). For these cells we can
            // use a material instance like this:
            // - Cell indices: empty
            // - Stream class code: intra-partition stream cell
            // - Count of cells: 1

            // Then we accumulate material starting from partition
            // input cells. For these cells we can use a material instance
            // like this:
            // - Cell indices: {input_cell_idxs}
            // - Stream class code: inter-partition stream cell
            // - Count of cells: 1

            using InterPartitionStreamMaterial = typename PartitionIOData::Value;

            using CellsIdxs = typename InterPartitionStreamMaterial::CellsIdxs;
            using CellsIdxsArray = Array<CellsIdxs, rank>;

            using Count = typename InterPartitionStreamMaterial::Count;
            using CountArray = Array<Count, rank>;

            using InputMaterial = inter_partition_stream::InputMaterial<CellsIdxs>;
            using OutputMaterial =
                inter_partition_stream::OutputMaterial<CellsIdxsArray, StreamClassData, CountArray>;

            using CellAccumulator = inter_partition_stream::CellAccumulator<CellsIdxs, StreamClass, Count>;
            using Accumulator =
                detail::Accumulator<PartitionIOData, InputMaterial, CellAccumulator, OutputMaterial>;

            // Once the data from all partitions have arrived...
            return hpx::split_future(
                hpx::dataflow(
                    hpx::launch::async,
                    hpx::unwrapping(

                        [policies, partition_offset](
                            Offset offset, lue::Array<FlowDirectionData, rank> const& flow_direction_data)
                        {
                            AnnotateFunction annotation{"inter_partition_stream"};

                            // Calculate inflow_count

                            // Output policy type for CountElement. Base it
                            // on the output policy for the stream classification
                            // passed in.
                            using InflowCountPolicy =
                                policy::OutputPoliciesT<Policies, 0, InflowCountElement>;

                            // Input policy for FlowDirectionElement. This is
                            // the same one as passed in.
                            using FlowDirectionPolicy = policy::InputPoliciesT<Policies, 0>;

                            using InflowCountPolicies = policy::Policies<
                                policy::AllValuesWithinDomain<FlowDirectionElement>,
                                policy::OutputsPolicies<InflowCountPolicy>,
                                policy::InputsPolicies<FlowDirectionPolicy>>;
                            InflowCountPolicies inflow_count_policies{
                                policy::AllValuesWithinDomain<FlowDirectionElement>{},
                                InflowCountPolicy{},
                                std::get<0>(policies.inputs_policies())};

                            auto const [inflow_count_data, input_cell_idxs] =
                                detail::inflow_count_data<InflowCountData>(
                                    inflow_count_policies, flow_direction_data);
                            lue_hpx_assert(std::is_sorted(input_cell_idxs.begin(), input_cell_idxs.end()));

                            Shape const partition_shape{inflow_count_data.shape()};

                            PartitionIOData partition_io_data{partition_shape, std::move(input_cell_idxs)};

                            // Accumulate input data to output data
                            // Input data:
                            // - Input cell IDs, only at input cells
                            //     - Collection can be stored and managed
                            //         internally. No need to create a full array.
                            // - Stream class IDs, only at input and ridge cells
                            //     - Will (only) be set in the input for input
                            //         and ridge cell accumulate call
                            // - Counts, array filled with ones
                            //     - Don't need to be provided. Can be
                            //         handled while accumulating.
                            //
                            // Output data:
                            // - Array filled with collections of input cell IDs
                            // - Array filled with stream class IDs
                            // - Counts

                            InputMaterial input_material{input_cell_idxs};

                            CellsIdxsArray cells_idxs_array{partition_shape};
                            StreamClassData stream_class_array{partition_shape};

                            // Initialize output counts with
                            // 1. Accumulation will accumulate these counts
                            // downstream
                            CountArray count_array{partition_shape, 1};

                            OutputMaterial output_material{cells_idxs_array, stream_class_array, count_array};

                            auto inflow_count_data_copy{deep_copy(inflow_count_data)};

                            Accumulator accumulator{
                                partition_io_data, input_material, CellAccumulator{}, output_material};

                            auto const& indp_inflow_count =
                                std::get<0>(inflow_count_policies.outputs_policies()).output_no_data_policy();
                            auto const& ondp_stream =
                                std::get<0>(policies.outputs_policies()).output_no_data_policy();

                            auto const [nr_elements0, nr_elements1] = partition_shape;

                            for (Index idx0 = 0; idx0 < nr_elements0; ++idx0)
                            {
                                for (Index idx1 = 0; idx1 < nr_elements1; ++idx1)
                                {
                                    // Ridge cells are cells that don't receive
                                    // input from other cells
                                    if (indp_inflow_count.is_no_data(inflow_count_data, idx0, idx1))
                                    {
                                        ondp_stream.mark_no_data(stream_class_array, idx0, idx1);
                                    }
                                    else if (inflow_count_data(idx0, idx1) == 0)
                                    {
                                        // Provide input values at
                                        // the starting points of the
                                        // accumulation
                                        stream_class_array(idx0, idx1) = intra_partition_stream_cell;

                                        // Forward the material through the network
                                        accumulate(
                                            accumulator,
                                            idx0,
                                            idx1,
                                            flow_direction_data(1, 1),
                                            inflow_count_data_copy);
                                    }
                                }
                            }

                            Count count{0};
                            for (auto it = input_cell_idxs.begin(); it != input_cell_idxs.end(); it += count)
                            {
                                auto const [idx0, idx1] = *it;
                                count = input_material.inflow_count(*it);
                                lue_hpx_assert(count >= 1);

                                lue_hpx_assert(
                                    static_cast<Count>(inflow_count_data_copy(idx0, idx1)) >= count);
                                inflow_count_data_copy(idx0, idx1) -= count;

                                // If the remaining inflow count is
                                // larger than zero, then there is another
                                // stream, originating in a partition input
                                // cell, passing through this cell. In that
                                // case we can leave the accumulation to
                                // the latter cell.
                                if (inflow_count_data_copy(idx0, idx1) == 0)
                                {
                                    // Provide input values at
                                    // the starting points of the
                                    // accumulation
                                    stream_class_array(idx0, idx1) = inter_partition_stream_cell;

                                    // Forward the material through the network
                                    accumulate(
                                        accumulator,
                                        idx0,
                                        idx1,
                                        flow_direction_data(1, 1),
                                        inflow_count_data_copy);
                                }
                            }

                            // Done. All inflow counts must be zero now.
                            /*
                            // TODO assert this
                            {
                                equal_to_policy → base on inflow count policy
                                all_policy → base on equal_to_policy
                                lue_hpx_assert(all(all_policy, equal_to(equal_to_policy,
                            inflow_count_data_copy, 0)).get());
                            }
                            */

                            return hpx::make_tuple(
                                StreamClassPartition{hpx::find_here(), offset, std::move(stream_class_array)},
                                PartitionIOComponent{hpx::find_here(), std::move(partition_io_data)});
                        }

                        ),
                    std::move(offset),
                    when_all_get(std::move(flow_direction_data))));
        }


        template<
            typename Policies,
            typename InterPartitionStreamPartition,
            typename PartitionIOComponent,
            typename FlowDirectionPartitions>
        struct InterPartitionStreamPartitionAction:
            hpx::actions::make_action<
                decltype(&inter_partition_stream_partition<
                         Policies,
                         InterPartitionStreamPartition,
                         PartitionIOComponent,
                         FlowDirectionPartitions>),
                &inter_partition_stream_partition<
                    Policies,
                    InterPartitionStreamPartition,
                    PartitionIOComponent,
                    FlowDirectionPartitions>,
                InterPartitionStreamPartitionAction<
                    Policies,
                    InterPartitionStreamPartition,
                    PartitionIOComponent,
                    FlowDirectionPartitions>>::type
        {
        };

    }  // namespace detail


    namespace policy::inter_partition_stream {

        template<typename FlowDirectionElement>
        using DefaultPolicies = Policies<
            AllValuesWithinDomain<FlowDirectionElement>,
            OutputsPolicies<OutputPolicies<
                DontMarkNoData<StreamClass>,
                AllValuesWithinRange<StreamClass, FlowDirectionElement>>>,
            InputsPolicies<SpatialOperationInputPolicies<
                SkipNoData<FlowDirectionElement>,
                FlowDirectionHalo<FlowDirectionElement>>>>;


        template<typename FlowDirectionElement>
        using DefaultValuePolicies = Policies<
            AllValuesWithinDomain<FlowDirectionElement>,
            OutputsPolicies<OutputPolicies<
                MarkNoDataByValue<StreamClass>,
                AllValuesWithinRange<StreamClass, FlowDirectionElement>>>,
            InputsPolicies<SpatialOperationInputPolicies<
                DetectNoDataByValue<FlowDirectionElement>,
                FlowDirectionHalo<FlowDirectionElement>>>>;

    }  // namespace policy::inter_partition_stream


    template<typename Policies, typename FlowDirectionElement, Rank rank>
    std::tuple<
        PartitionedArray<StreamClass, rank>,
        ComponentArray<
            ArrayPartitionIO<lue::Index, rank, InterPartitionStreamMaterial<lue::Index, rank>>,
            rank>>
    inter_partition_stream(
        Policies const& policies, PartitionedArray<FlowDirectionElement, rank> const& flow_direction)
    {
        using FlowDirectionArray = PartitionedArray<FlowDirectionElement, rank>;
        using FlowDirectionPartitions = PartitionsT<FlowDirectionArray>;

        using StreamClassArray = PartitionedArray<StreamClass, rank>;
        using StreamClassPartitions = PartitionsT<StreamClassArray>;
        using StreamClassPartition = PartitionT<StreamClassArray>;

        using PartitionIOArray = ComponentArray<
            ArrayPartitionIO<lue::Index, rank, InterPartitionStreamMaterial<lue::Index, rank>>,
            rank>;
        using PartitionIOComponents = ElementsT<PartitionIOArray>;
        using PartitionIOComponent = ElementT<PartitionIOArray>;

        using Shape = ShapeT<FlowDirectionArray>;

        Localities<rank> const& localities{flow_direction.localities()};
        FlowDirectionPartitions const& flow_direction_partitions{flow_direction.partitions()};

        lue_hpx_assert(all_are_valid(flow_direction_partitions));


        // Create halo partitions with flow directions away from the array
        Shape min_shape{};
        std::fill(min_shape.begin(), min_shape.end(), 1);

        std::array<FlowDirectionPartitions, 3> const halo_partitions = detail::halo_partitions(
            std::get<0>(policies.inputs_policies()), localities, min_shape, flow_direction_partitions);

        // Attach continuations to groups of nine partitions
        using Action = detail::InterPartitionStreamPartitionAction<
            Policies,
            StreamClassPartition,
            PartitionIOComponent,
            FlowDirectionPartitions>;

        auto [inter_partition_stream_partitions, partition_io_partitions] =
            detail::spawn_components<StreamClassPartitions, PartitionIOComponents>(
                policies, Action{}, localities, halo_partitions, flow_direction_partitions);

        lue_hpx_assert(all_are_valid(inter_partition_stream_partitions));
        lue_hpx_assert(all_are_valid(partition_io_partitions));


        return std::make_tuple(
            StreamClassArray{
                shape(flow_direction),
                Localities<rank>{localities},
                std::move(inter_partition_stream_partitions)},
            PartitionIOArray{Localities<rank>{localities}, std::move(partition_io_partitions)});
    }


    template<typename FlowDirectionElement, Rank rank>
    std::tuple<
        PartitionedArray<StreamClass, rank>,
        ComponentArray<
            ArrayPartitionIO<lue::Index, rank, InterPartitionStreamMaterial<lue::Index, rank>>,
            rank>>
    inter_partition_stream(PartitionedArray<FlowDirectionElement, rank> const& flow_direction)
    {
        using Policies = policy::inter_partition_stream::DefaultPolicies<FlowDirectionElement>;

        return inter_partition_stream<Policies, FlowDirectionElement, rank>(Policies{}, flow_direction);
    }

}  // namespace lue
