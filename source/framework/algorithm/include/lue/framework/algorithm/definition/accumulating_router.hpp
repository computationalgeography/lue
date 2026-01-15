#pragma once
#include "lue/framework/algorithm/definition/inflow_count.hpp"
#include "lue/framework/algorithm/detail/communicator.hpp"
#include "lue/framework/algorithm/detail/communicator_array.hpp"
#include "lue/framework/algorithm/detail/idx_converter.hpp"
#include "lue/framework/algorithm/functor_traits.hpp"
#include "lue/framework/algorithm/policy/detail/type_list.hpp"
#include "lue/framework/algorithm/policy/policy_traits.hpp"
#include "lue/framework/algorithm/scalar.hpp"
#include "lue/framework/core/annotate.hpp"
#include "lue/framework/core/component.hpp"
#include "lue/framework/core/type_traits.hpp"
#include "lue/framework.hpp"
#include <concepts>


// Policies determine how to check argument and return element types. Therefore:
// - Argument types can be expressed in terms of element types obtained from the policies. Argument types of
//   algorithms and of functors.

// Functors determine what gets calculated. Therefore:
// - Functors define return types. They can be queried for this information. ReturnTypes<Functor>...
// - It must implement the call operator(s) that eventually get called by accumulating_router. This operator
//   must return results for the same number of values as the overall number of return types.


namespace lue {

    template<typename Functor>
    using ResultsT = typename Functor::Results;

    template<typename Functor>
    using MaterialT = typename Functor::Material;


    template<typename... Ts>
    auto hpx_tuple_to_std_tuple(hpx::tuple<Ts...>&& tuple) -> std::tuple<Ts...>
    {
        return std::move(tuple);
    }


    template<typename... Ts>
    auto when_all(std::tuple<hpx::future<Ts>...>&& futures) -> hpx::future<hpx::tuple<hpx::future<Ts>...>>
    {
        return std::apply([](auto&&... futures) { return hpx::when_all(futures...); }, std::move(futures));
    }


    template<Arithmetic... Element, typename... Arguments>
    auto tied_references1(
        std::tuple<hpx::future<ArrayPartitionData<Element, 2>>...>& results_partition_data_fs,
        Arguments&... arguments) -> std::tuple<hpx::future<ArrayPartitionData<Element, 2>>&..., Arguments&...>
    {
        return std::apply(
            [&arguments...](auto&... results_partition_data_fs) -> auto
            { return std::tie(results_partition_data_fs..., arguments...); },
            results_partition_data_fs);
    }


    template<Arithmetic... Element>
    auto tied_references2(
        std::tuple<hpx::future<ArrayPartitionData<Element, 2>>...>& results_partition_data_fs)
        -> std::tuple<hpx::future<ArrayPartitionData<Element, 2>>&...>
    {
        return std::apply(
            [](auto&... results_partition_data_fs) -> auto { return std::tie(results_partition_data_fs...); },
            results_partition_data_fs);
    }


    /*!
        @brief      Given a tuple of arrays containing partitions and a partition index, return a tuple of
                    l-value references to partitions
    */
    template<typename... Element>
    auto partition_references(
        std::tuple<ArrayPartitionData<lue::ArrayPartition<Element, 2>, 2>...>& results_partitions,
        Index const partition_idx) -> std::tuple<ArrayPartition<Element, 2>&...>
    {
        return std::apply(
            [partition_idx](auto&... result_partitions)
            { return std::tie(result_partitions[partition_idx]...); },
            results_partitions);
    }


    template<typename... Element>
    auto results_partitions_to_arrays(
        std::tuple<ArrayPartitionData<lue::ArrayPartition<Element, 2>, 2>...>& results_partitions,
        auto const& array_shape,
        auto const& localities) -> std::tuple<PartitionedArray<Element, 2>...>
    {
        return std::apply(
            [&array_shape, &localities](auto&... partitions)
            {
                return std::make_tuple(
                    PartitionedArray<Element, 2>{
                        array_shape, Localities<2>{localities}, std::move(partitions)}...);
            },
            results_partitions);
    }


    template<Arithmetic Element, Rank rank>
    auto partition_data_to_partition(
        hpx::id_type const locality_id,
        Offset<Index, rank> const& partition_offset,
        ArrayPartitionData<Element, rank>&& partition_data) -> ArrayPartition<Element, rank>
    {
        using Partition = ArrayPartition<Element, rank>;
        using PartitionServer = typename Partition::Server;

        return Partition{
            hpx::new_<PartitionServer>(locality_id, partition_offset, std::move(partition_data))};
    }


    template<Arithmetic Element, Rank rank>
    auto partition_f_to_partition(hpx::future<ArrayPartition<Element, rank>>&& partition_f)
        -> ArrayPartition<Element, rank>
    {
        return ArrayPartition<Element, rank>(std::move(partition_f));
    }


    template<typename Functor>
    using ActionResultT = typename Functor::ActionResults;

    template<typename Functor>
    using IntraPartitionStreamCellsResult = typename Functor::IntraPartitionStreamCellsResult;

    template<typename Functor>
    using InterPartitionStreamCellsResult = typename Functor::InterPartitionStreamCellsResult;


    namespace detail {

        enum class AccumulationExitCellClass : std::uint8_t {
            sink,
            partition_output,
            confluence,
        };


        inline auto destination_cell(
            Count const extent0,
            Count const extent1,
            Index const idx0,
            Index const idx1,
            Index const offset0,
            Index const offset1) -> std::tuple<accu::Direction, Index>
        {
            lue_hpx_assert((idx0 == 0 || idx0 == extent0 - 1) || (idx1 == 0 || idx1 == extent1 - 1));
            lue_hpx_assert(offset0 >= -1 && offset0 <= 1);
            lue_hpx_assert(offset1 >= -1 && offset1 <= 1);

            // Which neighbouring partition to send a message to
            accu::Direction direction{};

            // Idx of cell in the neighbouring partition
            Index idx{-1};

            if (idx0 == 0)
            {
                // north border
                if (idx1 == 0)
                {
                    // north-west source cell → determine destination
                    if (offset0 >= 0)
                    {
                        lue_hpx_assert(offset1 == -1);
                        direction = accu::Direction::west;
                        idx = idx0 + offset0;
                    }
                    else if (offset1 >= 0)
                    {
                        lue_hpx_assert(offset0 == -1);
                        direction = accu::Direction::north;
                        idx = idx1 + offset1;
                    }
                    else
                    {
                        lue_hpx_assert(offset0 == -1);
                        lue_hpx_assert(offset1 == -1);
                        direction = accu::Direction::north_west;
                        idx = -1;  // Whatever
                    }
                }
                else if (idx1 == extent1 - 1)
                {
                    // north-east source cell → determine destination
                    if (offset0 >= 0)
                    {
                        lue_hpx_assert(offset1 == 1);
                        direction = accu::Direction::east;
                        idx = idx0 + offset0;
                    }
                    else if (offset1 <= 0)
                    {
                        lue_hpx_assert(offset0 == -1);
                        direction = accu::Direction::north;
                        idx = idx1 + offset1;
                    }
                    else
                    {
                        lue_hpx_assert(offset0 == -1);
                        lue_hpx_assert(offset1 == 1);
                        direction = accu::Direction::north_east;
                        idx = 0;  // Whatever
                    }
                }
                else
                {
                    // north source cell → determine destination
                    lue_hpx_assert(offset0 == -1);
                    direction = accu::Direction::north;
                    idx = idx1 + offset1;
                }
            }
            else if (idx0 == extent0 - 1)
            {
                // south border
                if (idx1 == 0)
                {
                    // south-west source cell → determine destination
                    if (offset0 <= 0)
                    {
                        lue_hpx_assert(offset1 == -1);
                        direction = accu::Direction::west;
                        idx = idx0 + offset0;
                    }
                    else if (offset1 >= 0)
                    {
                        lue_hpx_assert(offset0 == 1);
                        direction = accu::Direction::south;
                        idx = idx1 + offset1;
                    }
                    else
                    {
                        lue_hpx_assert(offset0 == 1);
                        lue_hpx_assert(offset1 == -1);
                        direction = accu::Direction::south_west;
                        idx = -1;  // Whatever
                    }
                }
                else if (idx1 == extent1 - 1)
                {
                    // south-east source cell → determine destination
                    if (offset0 <= 0)
                    {
                        lue_hpx_assert(offset1 == 1);
                        direction = accu::Direction::east;
                        idx = idx0 + offset0;
                    }
                    else if (offset1 <= 0)
                    {
                        lue_hpx_assert(offset0 == 1);
                        direction = accu::Direction::south;
                        idx = idx1 + offset1;
                    }
                    else
                    {
                        lue_hpx_assert(offset0 == 1);
                        lue_hpx_assert(offset1 == 1);
                        direction = accu::Direction::south_east;
                        idx = -1;  // Whatever
                    }
                }
                else
                {
                    // south source cell → determine destination
                    lue_hpx_assert(offset0 == 1);
                    direction = accu::Direction::south;
                    idx = idx1 + offset1;
                }
            }
            else if (idx1 == 0)
            {
                // west source cell → determine destination
                lue_hpx_assert(offset1 == -1);
                direction = accu::Direction::west;
                idx = idx0 + offset0;
            }
            else if (idx1 == extent1 - 1)
            {
                // east source cell → determine destination
                lue_hpx_assert(offset1 == 1);
                direction = accu::Direction::east;
                idx = idx0 + offset0;
            }

            return std::make_tuple(direction, idx);
        }


        template<
            typename MaterialElement,
            Rank rank>  // Remove parameter
        class ChannelMaterial
        {

            public:

                ChannelMaterial() = default;


                ChannelMaterial(Index const& idx, MaterialElement const& value):

                    _cell_idx{idx},
                    _value{value}

                {
                }


                auto cell_idx() const -> Index const&
                {
                    return _cell_idx;
                }


                auto value() const -> MaterialElement const&
                {
                    return _value;
                }


            private:

                friend class hpx::serialization::access;


                template<typename Archive>
                void serialize(Archive& archive, [[maybe_unused]] unsigned int const version)
                {
                    archive & _cell_idx & _value;
                }


                Index _cell_idx;

                MaterialElement _value;
        };


        template<typename MaterialElement, Rank rank>
        class MaterialCommunicator: public Communicator<ChannelMaterial<MaterialElement, rank>, rank>
        {

            public:

                using Base = Communicator<ChannelMaterial<MaterialElement, rank>, rank>;


                MaterialCommunicator() = default;


                MaterialCommunicator(
                    hpx::id_type const locality_id,
                    std::string const& basename,
                    lue::Shape<Count, rank> const& shape_in_partitions,
                    lue::Indices<Index, rank> const& partition_idxs):

                    Base{locality_id, basename, shape_in_partitions, partition_idxs}

                {
                }


            private:

                friend class hpx::serialization::access;


                template<typename Archive>
                void serialize(Archive& archive, unsigned int const version)
                {
                    Base::serialize(archive, version);
                }
        };


        template<typename CellAccumulator, typename Communicator>
        class Accumulator
        {

            public:

                using MaterialElement = typename CellAccumulator::MaterialElement;


                Accumulator(
                    CellAccumulator&& cell_accumulator,
                    Communicator& communicator,
                    std::array<std::vector<std::array<Index, 2>>, 8>& output_cells_idxs):

                    _cell_accumulator{std::forward<CellAccumulator>(cell_accumulator)},
                    _communicator{communicator},
                    _output_cells_idxs{output_cells_idxs}

                {
                }


                void enter_intra_partition_stream(Index const idx0, Index const idx1)
                {
                    // What to do when we enter an intra-partition stream. The current cell is a ridge cell.
                    _cell_accumulator.enter_intra_partition_stream(idx0, idx1);
                }


                void leave_intra_partition_stream(Index const ridge_idx0, Index const ridge_idx1)
                {
                    // What to do when we leave an intra-partition stream
                    _cell_accumulator.leave_intra_partition_stream(ridge_idx0, ridge_idx1);
                }


                void enter_inter_partition_stream(
                    MaterialElement const& value, Index const idx0_to, Index const idx1_to)
                {
                    // What to do when we enter an inter-partition stream. The current cell is a partition
                    // input cell.
                    _cell_accumulator.enter_inter_partition_stream(value, idx0_to, idx1_to);
                }


                void leave_inter_partition_stream(Index const ridge_idx0, Index const ridge_idx1)
                {
                    // What to do when we leave an inter-partition stream
                    _cell_accumulator.leave_inter_partition_stream(ridge_idx0, ridge_idx1);
                }


                void enter_cell(Index const idx0, Index const idx1)
                {
                    // What to do when we enter a cell
                    // We are entering a cell. Material from upstream is already present.
                    _cell_accumulator.enter_cell(idx0, idx1);
                }


                void leave_cell(
                    Index const idx0_from, Index const idx1_from, Index const idx0_to, Index const idx1_to)
                {
                    // What to do when we leave a cell
                    // We are about the leave the current cell. This is the final step.
                    _cell_accumulator.leave_cell(idx0_from, idx1_from, idx0_to, idx1_to);
                }


                void stop_at_sink_cell(Index const idx0, Index const idx1)
                {
                    // What to do when we leave at a sink
                    _cell_accumulator.stop_at_sink_cell(idx0, idx1);
                }


                void stop_at_confluence_cell(Index const idx0, Index const idx1)
                {
                    // What to do when we leave at a confluence
                    _cell_accumulator.stop_at_confluence_cell(idx0, idx1);
                }


                void stop_at_partition_output_cell(
                    Count const extent0,
                    Count const extent1,
                    Index const idx0,
                    Index const idx1,
                    Index const offset0,
                    Index const offset1)
                {
                    // What to do when we leave at a partition output cell
                    _cell_accumulator.stop_at_partition_output_cell(idx0, idx1);

                    // Remove the partition output cell from the collection. Once all output cells
                    // are solved / handled, the sending channel can be closed. This will end
                    // the reading for loop on the other side of the channel.
                    auto [direction, idx] = destination_cell(extent0, extent1, idx0, idx1, offset0, offset1);

                    auto& output_cells_idxs{_output_cells_idxs[direction]};
                    auto iterator = std::find_if(
                        output_cells_idxs.begin(),
                        output_cells_idxs.end(),
                        [idx0, idx1](std::array<Index, 2> const& cell_idxs2) -> auto
                        { return idx0 == cell_idxs2[0] && idx1 == cell_idxs2[1]; });
                    lue_hpx_assert(iterator != output_cells_idxs.end());
                    output_cells_idxs.erase(iterator);

                    // Send material to cell in neighbouring partition
                    if (_communicator.has_neighbour(direction))
                    {
                        // We are not at the border of the array
                        using Value = typename Communicator::Value;

                        _communicator.send(direction, Value{idx, _cell_accumulator.outflow(idx0, idx1)});

                        // The sending channel can be closed
                        if (output_cells_idxs.empty())
                        {
                            _communicator.close(direction);
                        }
                    }
                }


                void mark_no_data(Index const idx0, Index const idx1)
                {
                    _cell_accumulator.mark_no_data(idx0, idx1);
                }


            private:

                CellAccumulator _cell_accumulator;

                Communicator& _communicator;

                std::array<std::vector<std::array<Index, 2>>, 8>& _output_cells_idxs;
        };


        template<typename Accumulator, typename Index, typename FlowDirectionData, typename InflowCountData>
        auto accumulate(
            Accumulator& accumulator,
            Index const ridge_idx0,
            Index const ridge_idx1,
            FlowDirectionData const& flow_direction_data,
            InflowCountData& inflow_count_data) -> std::tuple<std::array<Index, 2>, AccumulationExitCellClass>
        {
            Index idx0{ridge_idx0};
            Index idx1{ridge_idx1};
            auto const [nr_elements0, nr_elements1] = flow_direction_data.shape();
            Index offset0{};
            Index offset1{};
            bool is_within_partition{};
            AccumulationExitCellClass cell_class{};

            while (true)
            {
                lue_hpx_assert(inflow_count_data(idx0, idx1) == 0);

                // We are about to leave the current cell. All material from
                // upstream (if any) has been accumulated in the cell already.
                // We still need to add the external input.
                accumulator.enter_cell(idx0, idx1);

                // Determine what to do next
                is_within_partition = downstream_cell(
                    flow_direction_data, nr_elements0, nr_elements1, idx0, idx1, offset0, offset1);

                // First check conditions that will end the current stream ---------
                if (offset0 == 0 && offset1 == 0)
                {
                    // Current cell is a sink. This is the end of this
                    // stream.
                    accumulator.stop_at_sink_cell(idx0, idx1);
                    cell_class = AccumulationExitCellClass::sink;
                    break;
                }

                // Downstream cell is pointed to by idx0 + offset0 and idx1 + offset1
                if (!is_within_partition)
                {
                    // Current cell is partition output cell. This is the
                    // end of this stream in the current partition. Finish
                    // calculations an exit.
                    accumulator.stop_at_partition_output_cell(
                        nr_elements0, nr_elements1, idx0, idx1, offset0, offset1);
                    cell_class = AccumulationExitCellClass::partition_output;
                    break;
                }

                // Stream continues within this partition --------------------------

                // Push material to the downstream cell
                accumulator.leave_cell(idx0, idx1, idx0 + offset0, idx1 + offset1);

                // Prepare for next iteration
                idx0 += offset0;
                idx1 += offset1;

                // Update the inflow count of the downstream cell
                lue_hpx_assert(inflow_count_data(idx0, idx1) >= 1);
                --inflow_count_data(idx0, idx1);

                if (inflow_count_data(idx0, idx1) > 0)
                {
                    // There are other streams flowing into the new / downstream cell → stop
                    accumulator.stop_at_confluence_cell(idx0, idx1);
                    cell_class = AccumulationExitCellClass::confluence;
                    break;
                }
            }

            return std::make_tuple(std::array<Index, 2>{idx0, idx1}, cell_class);
        }


        template<typename MaterialElement, typename IdxConverter, typename Accumulate, Rank rank>
        void monitor_material_inputs(
            std::vector<std::array<Index, rank>>&& input_cells_idxs,
            typename MaterialCommunicator<MaterialElement, rank>::Channel&& channel,
            IdxConverter&& idx_to_idxs,
            Accumulate accumulate)
        {
            AnnotateFunction annotation{"inter_partition_stream"};
            lue_hpx_assert(channel);
            lue_hpx_assert(!input_cells_idxs.empty());

            // Whenever material arrives in the channel, call the
            // accumulator to accumulate it through the partition

            // The number of times material should arrive in the channel
            // is equal to the number of input cells at the specific side
            // of the partition.

            for (auto const& material : channel)
            {
                lue_hpx_assert(!input_cells_idxs.empty());

                auto const cell_idxs{idx_to_idxs(material.cell_idx())};

                auto it = std::find_if(
                    input_cells_idxs.begin(),
                    input_cells_idxs.end(),
                    [cell_idxs1 = cell_idxs](std::array<Index, rank> const& cell_idxs2) -> auto
                    { return cell_idxs1 == cell_idxs2; });
                lue_hpx_assert(it != input_cells_idxs.end());
                input_cells_idxs.erase(it);

                accumulate(cell_idxs, material.value());

                if (input_cells_idxs.empty())
                {
                    // No material should be sent trough this channel
                    // again. We don't need it. It would be a bug.
                    break;
                }
            }

            lue_hpx_assert(input_cells_idxs.empty());
        }


        /*!
            @brief      Create communicators used to exchange information about inflow counts and accumulated
                        material between localities
            @tparam     Type of algorithm-specific functor
            @param      localities Collection of localities
            @return     Tuple of arrays containing communicators for exchanging inflow counts and accumulated
                        material, in that order
        */
        template<typename Functor>
        auto create_communicators(Localities<2> const& localities) -> std::tuple<
            CommunicatorArray<InflowCountCommunicator<2>, 2>,
            CommunicatorArray<MaterialCommunicator<MaterialT<Functor>, 2>, 2>>
        {
            return {
                CommunicatorArray<InflowCountCommunicator<2>, 2>{
                    std::format("/lue/{}/inflow_count/", functor_name<Functor>), localities},
                CommunicatorArray<MaterialCommunicator<MaterialT<Functor>, 2>, 2>{
                    std::format("/lue/{}/{}/", functor_name<Functor>, as_string<MaterialT<Functor>>),
                    localities}};
        }


        template<typename Partitions, typename MaterialElement>
        void keep_communicators_alive(
            Partitions const& partitions,
            CommunicatorArray<InflowCountCommunicator<2>, 2>&& inflow_count_communicators,
            CommunicatorArray<MaterialCommunicator<MaterialElement, 2>, 2>&& material_communicators)
        {
            // The communicators used by the tasks calculating the results must be kept alive until these
            // results are ready. We do this by attaching a continuation, passing in the communicators. Once
            // the results are ready, these communicators go out of scope. Once they do, free up AGAS
            // resources. Note that component clients are reference-counted.
            // TODO: Is this actually necessary?
            hpx::when_all(
                partitions.begin(),
                partitions.end(),
                [inflow_count_communicators = std::move(inflow_count_communicators),
                 material_communicators =
                     std::move(material_communicators)]([[maybe_unused]] auto&& partitions) mutable -> auto
                {
                    auto f1{inflow_count_communicators.unregister()};
                    auto f2{material_communicators.unregister()};

                    hpx::wait_all(f1, f2);
                });
        }


        template<typename T>
        class ArgumentTraits
        {
        };


        template<Arithmetic Element, Rank rank>
        class ArgumentTraits<PartitionedArray<Element, rank>>
        {
            public:

                using PassedArgument = PartitionT<PartitionedArray<Element, rank>>;
        };


        template<Arithmetic Element>
        class ArgumentTraits<Scalar<Element>>
        {
            public:

                using PassedArgument = Scalar<Element>;
        };


        template<typename Argument>
        using PassedArgumentT = ArgumentTraits<Argument>::PassedArgument;


        /*!
            @brief      Given a partitioned array and a partition index, return a reference to a partition
        */
        template<Arithmetic Element, Rank rank>
        auto pass_argument(PartitionedArray<Element, rank> const& array, Index const partition_idx)
            -> PartitionT<PartitionedArray<Element, rank>> const&
        {
            return array.partitions()[partition_idx];
        }


        /*!
            @brief      Given a scalar and a partition index, return the scalar, unchanged
        */
        template<Arithmetic Element>
        auto pass_argument(Scalar<Element> const& scalar, [[maybe_unused]] Index const partition_idx)
            -> Scalar<Element>
        {
            return scalar;
        }


        template<Arithmetic Element, Rank rank>
        auto get_data(ArrayPartition<Element, rank> const& partition) -> DataT<ArrayPartition<Element, rank>>
        {
            auto const partition_ptr{ready_component_ptr(partition)};
            return partition_ptr->data();
        }


        template<Arithmetic Element>
        auto get_data(Scalar<Element> const& scalar) -> Element
        {
            return scalar.future().get();
        }


        template<Arithmetic Element, Rank rank>
        auto get_data(hpx::future<ArrayPartitionData<Element, rank>>& data_f)
            -> ArrayPartitionData<Element, rank>
        {
            lue_hpx_assert(data_f.is_ready());
            return data_f.get();
        }


        template<Arithmetic Element, Rank rank>
        auto to_value(ArrayPartitionData<Element, rank> const& data, Index const idx0, Index const idx1)
            -> Element
        {
            static_assert(rank == 2);

            return data(idx0, idx1);
        }


        template<Arithmetic Element>
        auto to_value(
            Element const& value, [[maybe_unused]] Index const idx0, [[maybe_unused]] Index const idx1)
            -> Element
        {
            return value;
        }


        template<typename Policies, typename Functor, typename... Arguments>
        auto solve_intra_partition_stream_cells(
            Policies const& policies,
            Functor functor,
            ArrayPartition<policy::InputElementT<Policies, 0>, 2> const& flow_direction_partition,
            Arguments const&... arguments,
            typename Functor::InflowCountPartition const& inflow_count_partition,
            MaterialCommunicator<MaterialT<Functor>, 2>& material_communicator,
            hpx::future<std::array<typename Functor::CellsIdxs, nr_neighbours<2>()>>&& output_cells_idxs_f)
            -> IntraPartitionStreamCellsResult<Functor>
        {
            /*
                - Once the flow direction partition is ready:
                    - Send locations of partition input cells in neighbouring partitions that will receive
                      material from the current partition using the corresponding channel in the NIDP
                      communicator.
                      Per neighbouring partition direction, store the location of the partition output cells.
                    - Asynchrounously receive locations of partition input cells that will receive material
                      from neighbouring partitions. Per neighbouring partition direction, store the location
                      of these partition input cells.
                    - Once the collections of partition input cells have been received, calculate the NIDP
                      for all cells in the partition.
            */

            using FlowDirectionPartition = ArrayPartition<FlowDirectionElement, 2>;
            using FlowDirectionData = DataT<FlowDirectionPartition>;
            using InflowCountData = Functor::InflowCountData;

            return hpx::split_future(
                hpx::dataflow(
                    hpx::launch::async,

                    [policies, functor = std::move(functor), material_communicator](
                        FlowDirectionPartition const& flow_direction_partition,
                        Arguments const&... arguments,
                        typename Functor::InflowCountPartition const& inflow_count_partition,
                        hpx::future<std::array<typename Functor::CellsIdxs, nr_neighbours<2>()>>&&
                            output_cells_idxs_f) mutable -> auto
                    {
                        AnnotateFunction const annotation{
                            "accumulating_router: partition: intra_partition_stream"};

                        auto const flow_direction_partition_ptr{
                            ready_component_ptr(flow_direction_partition)};
                        FlowDirectionData const& flow_direction_data{flow_direction_partition_ptr->data()};

                        auto const inflow_count_partition_ptr{ready_component_ptr(inflow_count_partition)};
                        InflowCountData const& inflow_count_data{inflow_count_partition_ptr->data()};

                        auto const& partition_shape{inflow_count_data.shape()};
                        auto const [nr_elements0, nr_elements1] = partition_shape;

                        // We need to copy inflow counts:
                        // - inflow counts are used to select ridge cells
                        // - downstream accumulation updates inflow counts
                        InflowCountData inflow_count_data_copy{deep_copy(inflow_count_data)};

                        auto const& indp_flow_direction =
                            std::get<0>(policies.inputs_policies()).input_no_data_policy();

                        auto output_cells_idxs{output_cells_idxs_f.get()};

                        // tuple<PartitionData...>
                        auto result_data = Functor::initialize_result_data(partition_shape);

                        auto cell_accumulator{std::apply(
                            // MSVC doesn't like this: [&policies, &arguments...](auto&... result_data)
                            [&](auto&... result_data) -> auto
                            {
                                return functor.template cell_accumulator<decltype(get_data(arguments))...>(
                                    policies, get_data(arguments)..., result_data...);
                            },
                            result_data)};

                        Accumulator accumulator{
                            std::move(cell_accumulator), material_communicator, output_cells_idxs};

                        for (Index idx0 = 0; idx0 < nr_elements0; ++idx0)
                        {
                            for (Index idx1 = 0; idx1 < nr_elements1; ++idx1)
                            {
                                if (indp_flow_direction.is_no_data(flow_direction_data, idx0, idx1))
                                {
                                    accumulator.mark_no_data(idx0, idx1);
                                }
                                else if (inflow_count_data(idx0, idx1) == 0)
                                {
                                    accumulator.enter_intra_partition_stream(idx0, idx1);
                                    accumulate(
                                        accumulator, idx0, idx1, flow_direction_data, inflow_count_data_copy);
                                    accumulator.leave_intra_partition_stream(idx0, idx1);
                                }
                            }
                        }

                        return std::apply(
                            [&inflow_count_data_copy, &output_cells_idxs](auto&&... result_data)
                            {
                                return std::make_tuple(
                                    std::move(result_data)...,
                                    std::move(inflow_count_data_copy),
                                    std::move(output_cells_idxs));
                            },
                            std::move(result_data));
                    },

                    flow_direction_partition,
                    arguments...,
                    inflow_count_partition,
                    std::move(output_cells_idxs_f)));
        }


        template<typename Policies, typename Functor, typename... Arguments, typename... Results>
        auto solve_inter_partition_stream_cells(
            Policies const& policies,
            Functor functor,
            ArrayPartition<policy::InputElementT<Policies, 0>, 2> const& flow_direction_partition,
            Arguments const&... arguments,
            hpx::future<typename Functor::InflowCountData>&& inflow_count_data_f,
            MaterialCommunicator<MaterialT<Functor>, 2>&& material_communicator,
            hpx::shared_future<std::array<typename Functor::CellsIdxs, nr_neighbours<2>()>>&&
                input_cells_idxs_f,
            hpx::future<std::array<typename Functor::CellsIdxs, nr_neighbours<2>()>>&& output_cells_idxs_f,
            std::tuple<hpx::future<ArrayPartitionData<Results, 2>>...>&& results)
            -> InterPartitionStreamCellsResult<Functor>
        {
            using FlowDirectionPartition = ArrayPartition<FlowDirectionElement, 2>;
            using FlowDirectionData = DataT<FlowDirectionPartition>;
            using InflowCountData = Functor::InflowCountData;

            auto result = hpx::dataflow(
                hpx::launch::async,

                [policies,
                 functor = std::move(functor),
                 material_communicator = std::move(material_communicator)](
                    FlowDirectionPartition const& flow_direction_partition,
                    Arguments const&... arguments,
                    hpx::future<InflowCountData>&& inflow_count_data_f,
                    hpx::shared_future<std::array<typename Functor::CellsIdxs, nr_neighbours<2>()>> const&
                        input_cells_idxs_f,
                    hpx::future<std::array<typename Functor::CellsIdxs, nr_neighbours<2>()>>&&
                        output_cells_idxs_f,
                    std::tuple<hpx::future<ArrayPartitionData<Results, 2>>...>&& results) mutable -> auto
                {
                    AnnotateFunction const annotation{
                        "accumulating_router: partition: inter_partition_stream"};

                    std::vector<accu::Direction> const directions{
                        accu::Direction::north,
                        accu::Direction::north_east,
                        accu::Direction::east,
                        accu::Direction::south_east,
                        accu::Direction::south,
                        accu::Direction::south_west,
                        accu::Direction::west,
                        accu::Direction::north_west,
                    };

                    // The accumulate function must wrap this call:
                    //     accumulate(
                    //         accumulator, idx0, idx1, flow_direction_data,
                    //         inflow_count_data_copy);
                    // It must also synchronize access to the accumulate call so no two threads call it at
                    // the same time
                    //
                    // Calling the function must
                    // - lock access to the data
                    // - accumulate material
                    // - release the lock

                    auto const flow_direction_partition_ptr{ready_component_ptr(flow_direction_partition)};
                    FlowDirectionData const& flow_direction_data{flow_direction_partition_ptr->data()};

                    InflowCountData inflow_count_data{inflow_count_data_f.get()};

                    auto result_data = std::apply(
                        [](auto&... results) { return std::make_tuple(get_data(results)...); }, results);

                    // We need to pass references to the resulting data collections (sink arguments). This
                    // code passes the references to the values in the local variable (result_data).
                    auto cell_accumulator = std::apply(
                        // MSVC doesn't like this: [&policies, &arguments...](auto&... result_data)
                        [&](auto&... result_data) -> auto
                        {
                            return functor.template cell_accumulator<decltype(get_data(arguments))...>(
                                policies, get_data(arguments)..., result_data...);
                        },
                        result_data);

                    auto output_cells_idxs{output_cells_idxs_f.get()};
                    Accumulator accumulator{
                        std::move(cell_accumulator), material_communicator, output_cells_idxs};

                    hpx::mutex accu_mutex{};
                    using MaterialElement = policy::OutputElementT<Policies, 0>;

                    auto accumulate = [&accu_mutex, &accumulator, &flow_direction_data, &inflow_count_data](
                                          std::array<Index, 2> const& cell_idxs,
                                          MaterialElement const value) mutable -> auto
                    {
                        auto [idx0, idx1] = cell_idxs;

                        // Prevent multiple threads from touching this data at the same time
                        std::scoped_lock lock{accu_mutex};

                        lue_hpx_assert(inflow_count_data(idx0, idx1) >= 1);

                        accumulator.enter_inter_partition_stream(value, idx0, idx1);

                        --inflow_count_data(idx0, idx1);

                        // Note that multiple streams from other partitions can join in a single partition
                        // input cell. Only start an accumulation if this is the last one.
                        if (inflow_count_data(idx0, idx1) == 0)
                        {
                            detail::accumulate(
                                accumulator, idx0, idx1, flow_direction_data, inflow_count_data);
                            accumulator.leave_inter_partition_stream(idx0, idx1);
                        }
                    };
                    using Accumulate = decltype(accumulate);

                    std::array<typename Functor::CellsIdxs, nr_neighbours<2>()> const& input_cells_idxs{
                        input_cells_idxs_f.get()};

                    std::vector<hpx::future<void>> result_fs{};
                    result_fs.reserve(nr_neighbours<2>());

                    auto const [extent0, extent1] = flow_direction_data.shape();

                    {
                        typename Functor::CellsIdxs cells_idxs{input_cells_idxs[accu::Direction::north]};

                        if (!cells_idxs.empty())
                        {
                            RowIdxConverter north_idx_converter{};
                            result_fs.push_back(
                                hpx::async(
                                    monitor_material_inputs<
                                        MaterialElement,
                                        decltype(north_idx_converter),
                                        Accumulate,
                                        2>,
                                    std::move(cells_idxs),
                                    material_communicator.receive_channel(accu::Direction::north),
                                    north_idx_converter,
                                    accumulate));
                        }
                    }

                    {
                        typename Functor::CellsIdxs cells_idxs{input_cells_idxs[accu::Direction::south]};

                        if (!cells_idxs.empty())
                        {
                            RowIdxConverter south_idx_converter{extent0 - 1};
                            result_fs.push_back(
                                hpx::async(
                                    monitor_material_inputs<
                                        MaterialElement,
                                        decltype(south_idx_converter),
                                        Accumulate,
                                        2>,
                                    std::move(cells_idxs),
                                    material_communicator.receive_channel(accu::Direction::south),
                                    south_idx_converter,
                                    accumulate));
                        }
                    }

                    {
                        typename Functor::CellsIdxs cells_idxs{input_cells_idxs[accu::Direction::west]};

                        if (!cells_idxs.empty())
                        {
                            ColIdxConverter west_idx_converter{};
                            result_fs.push_back(
                                hpx::async(
                                    monitor_material_inputs<
                                        MaterialElement,
                                        decltype(west_idx_converter),
                                        Accumulate,
                                        2>,
                                    std::move(cells_idxs),
                                    material_communicator.receive_channel(accu::Direction::west),
                                    west_idx_converter,
                                    accumulate));
                        }
                    }

                    {
                        typename Functor::CellsIdxs cells_idxs{input_cells_idxs[accu::Direction::east]};

                        if (!cells_idxs.empty())
                        {
                            ColIdxConverter east_idx_converter{extent1 - 1};
                            result_fs.push_back(
                                hpx::async(
                                    monitor_material_inputs<
                                        MaterialElement,
                                        decltype(east_idx_converter),
                                        Accumulate,
                                        2>,
                                    std::move(cells_idxs),
                                    material_communicator.receive_channel(accu::Direction::east),
                                    east_idx_converter,
                                    accumulate));
                        }
                    }

                    {
                        typename Functor::CellsIdxs cells_idxs{input_cells_idxs[accu::Direction::north_west]};

                        if (!cells_idxs.empty())
                        {
                            CornerIdxConverter north_west_idx_converter{};
                            result_fs.push_back(
                                hpx::async(
                                    monitor_material_inputs<
                                        MaterialElement,
                                        decltype(north_west_idx_converter),
                                        Accumulate,
                                        2>,
                                    std::move(cells_idxs),
                                    material_communicator.receive_channel(accu::Direction::north_west),
                                    north_west_idx_converter,
                                    accumulate));
                        }
                    }

                    {
                        typename Functor::CellsIdxs cells_idxs{input_cells_idxs[accu::Direction::north_east]};

                        if (!cells_idxs.empty())
                        {
                            CornerIdxConverter north_east_idx_converter{0, extent1 - 1};
                            result_fs.push_back(
                                hpx::async(
                                    monitor_material_inputs<
                                        MaterialElement,
                                        decltype(north_east_idx_converter),
                                        Accumulate,
                                        2>,
                                    std::move(cells_idxs),
                                    material_communicator.receive_channel(accu::Direction::north_east),
                                    north_east_idx_converter,
                                    accumulate));
                        }
                    }

                    {
                        typename Functor::CellsIdxs cells_idxs{input_cells_idxs[accu::Direction::south_east]};

                        if (!cells_idxs.empty())
                        {
                            CornerIdxConverter south_east_idx_converter{extent0 - 1, extent1 - 1};
                            result_fs.push_back(
                                hpx::async(
                                    monitor_material_inputs<
                                        MaterialElement,
                                        decltype(south_east_idx_converter),
                                        Accumulate,
                                        2>,
                                    std::move(cells_idxs),
                                    material_communicator.receive_channel(accu::Direction::south_east),
                                    south_east_idx_converter,
                                    accumulate));
                        }
                    }

                    {
                        typename Functor::CellsIdxs cells_idxs{input_cells_idxs[accu::Direction::south_west]};

                        if (!cells_idxs.empty())
                        {
                            CornerIdxConverter south_west_idx_converter{extent0 - 1, 0};
                            result_fs.push_back(
                                hpx::async(
                                    monitor_material_inputs<
                                        MaterialElement,
                                        decltype(south_west_idx_converter),
                                        Accumulate,
                                        2>,
                                    std::move(cells_idxs),
                                    material_communicator.receive_channel(accu::Direction::south_west),
                                    south_west_idx_converter,
                                    accumulate));
                        }
                    }

                    // As a side effect of this wait, the partitions whose data is
                    // used in the above monitor_material threads do not go out of scope

                    hpx::wait_all(result_fs);
                    lue_hpx_assert(all_are_valid(result_fs));
                    lue_hpx_assert(all_are_ready(result_fs));

                    // All output idxs must have been solved by now
                    lue_hpx_assert(
                        std::all_of(
                            output_cells_idxs.begin(),
                            output_cells_idxs.end(),
                            [](auto const& idxs) { return idxs.empty(); }));

                    // TODO Assert all inflow counts are zero
                    // TODO Assert all sender channels are closed

                    return result_data;
                },

                flow_direction_partition,
                arguments...,
                std::move(inflow_count_data_f),
                input_cells_idxs_f,
                std::move(output_cells_idxs_f),
                std::move(results));

            return hpx::split_future(std::move(result));
        }


        template<typename Policies, typename FlowDirectionElement, Rank rank>
        auto inflow_count(
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
            using InflowCountOutputPolicies = policy::OutputPolicies<
                policy::DontMarkNoData<CountElement>,
                policy::AllValuesWithinRange<CountElement>>;
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

            return inflow_count_action<CountElement>(
                inflow_count_policies, flow_direction_partition, std::move(inflow_count_communicator));
        }


        template<typename Argument>
        using FutureT = hpx::future<Argument>;


        template<typename Policies, typename Functor, typename... Arguments>
        class OverloadPicker
        {
            public:

                static auto accumulate_partition(
                    Policies const& policies,
                    Functor functor,
                    ArrayPartition<policy::InputElementT<Policies, 0>, 2> const& flow_direction_partition,
                    Arguments const&... arguments,
                    InflowCountCommunicator<2> inflow_count_communicator,
                    MaterialCommunicator<MaterialT<Functor>, 2> material_communicator)
                    -> ActionResultT<Functor>
                {
                    auto [inflow_count_partition, input_cells_idxs_f, output_cells_idxs_f] =
                        inflow_count<Policies>(
                            policies, flow_direction_partition, std::move(inflow_count_communicator));

                    using FlowDirectionElement = policy::InputElementT<Policies, 0>;
                    using FlowDirectionPartition = ArrayPartition<FlowDirectionElement, 2>;

                    hpx::future<typename Functor::InflowCountData> inflow_count_data_f;
                    auto results_partition_data_fs = Functor::initialize_results_partition_data_fs();

                    // TODO: Merge the tied_references overloads
                    tied_references1(results_partition_data_fs, inflow_count_data_f, output_cells_idxs_f) =
                        solve_intra_partition_stream_cells<Policies, Functor, Arguments...>(
                            policies,
                            functor,
                            flow_direction_partition,
                            arguments...,
                            inflow_count_partition,
                            material_communicator,
                            std::move(output_cells_idxs_f));
                    tied_references2(results_partition_data_fs) =
                        solve_inter_partition_stream_cells<Policies, Functor, Arguments...>(
                            policies,
                            std::move(functor),
                            flow_direction_partition,
                            arguments...,
                            std::move(inflow_count_data_f),
                            std::move(material_communicator),
                            std::move(input_cells_idxs_f),
                            std::move(output_cells_idxs_f),
                            std::move(results_partition_data_fs));

                    // std::tuple<hpx::future<ArrayPartition>...>
                    auto results_partition_fs = hpx::split_future(
                        hpx::dataflow(
                            hpx::launch::async,

                            [](FlowDirectionPartition const& flow_direction_partition,
                               // hpx::future<hpx::tuple<hpx::future<lue::ArrayPartitionData<unsigned char, 2>
                               // > > >
                               auto&& results_partition_data_fs_f) -> auto
                            {
                                AnnotateFunction const annotation{
                                    "accumulate: partition: create_result_partitions"};

                                // hpx::tuple<hpx::future<lue::ArrayPartitionData<unsigned char, 2> > >
                                auto results_partition_data_fs = results_partition_data_fs_f.get();

                                auto const partition_offset{
                                    ready_component_ptr(flow_direction_partition)->offset()};

                                // std::apply icw hpx::tuple doesn't work
                                return std::apply(
                                    [&partition_offset](auto&&... partition_data_fs) -> auto
                                    {
                                        return std::make_tuple(partition_data_to_partition(
                                            hpx::find_here(), partition_offset, partition_data_fs.get())...);
                                    },
                                    hpx_tuple_to_std_tuple(std::move(results_partition_data_fs)));
                            },

                            flow_direction_partition,
                            when_all(std::move(results_partition_data_fs))));

                    return std::apply(
                        [](auto&&... partition_fs) -> auto
                        { return std::make_tuple(partition_f_to_partition(std::move(partition_fs))...); },
                        results_partition_fs);
                }


                struct Action:
                    hpx::actions::
                        make_action<decltype(&accumulate_partition), &accumulate_partition, Action>::type
                {
                };
        };


        template<typename Policies, typename Functor, typename... Arguments>
        using AccumulateAction = OverloadPicker<Policies, Functor, Arguments...>::Action;

        // MSVC doesn't like this
        // template<typename Policies, typename Functor, typename... Arguments>
        // struct AccumulateAction:
        //     hpx::actions::make_action<
        //         decltype(&accumulate_partition<Policies, Functor, Arguments...>),
        //         &accumulate_partition<Policies, Functor, Arguments...>,
        //         AccumulateAction<Policies, Functor, Arguments...>>::type
        // {
        // };

    }  // namespace detail


    /*!
        @brief      Generic accumulating router algorithm
        @tparam     Policies Type of the algorithm-specific policies
        @tparam     Functor Type of the algorithm-specific types and behaviour
        @tparam     Arguments Types of the zero or more arguments used by the algorithm
        @param      policies Algorithm-specific policies
        @param      functor Algorithm-specific functor
        @param      flow_direction Array with flow directions
        @param      arguments Zero or more algorithm-specific arguments
        @return     One or more algorithm-specific results
    */
    template<typename Policies, typename Functor, typename... Arguments>
    auto accumulating_router(
        Policies const& policies,
        Functor const& functor,
        PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& flow_direction,
        Arguments const&... arguments) -> ResultsT<Functor>
    {
        Localities<2> const& localities{flow_direction.localities()};

        auto [inflow_count_communicators, material_communicators] =
            detail::create_communicators<Functor>(localities);

        auto const& shape_in_partitions{flow_direction.partitions().shape()};
        Count const nr_partitions{nr_elements(shape_in_partitions)};

        using Action = detail::AccumulateAction<Policies, Functor, detail::PassedArgumentT<Arguments>...>;
        Action action{};

        // For each result array a collection to store the final partitions in
        auto results_partitions = Functor::initialize_results_partitions(shape_in_partitions);

        // For each partition, create a task that will return one or more result partitions and store these
        // in the collections of partitions just created
        for (Index partition_idx = 0; partition_idx < nr_partitions; ++partition_idx)
        {
            partition_references(results_partitions, partition_idx) = hpx::split_future(
                hpx::async(
                    action,
                    localities[partition_idx],
                    policies,
                    functor,
                    flow_direction.partitions()[partition_idx],
                    detail::pass_argument(arguments, partition_idx)...,
                    inflow_count_communicators[partition_idx],
                    material_communicators[partition_idx]));
        }

        detail::keep_communicators_alive(
            std::get<0>(results_partitions),
            std::move(inflow_count_communicators),
            std::move(material_communicators));

        // Return partitioned arrays containing the collections of partitions just created
        return results_partitions_to_arrays(results_partitions, flow_direction.shape(), localities);
    }


    /*!
        @brief      Base class for accumulating router functors
        @tparam     ArgumentElements_ Element types of all arguments
        @tparam     ResultElements_ Element types of all results

        An accumulating router functor encapsulates the computation that needs to be done while visiting cells
        from upstream to downstream. Normally, this computation is about computing how mucht material is
        transported downstream, but this is not required.

        This is the primary template. There is a specialization that allows using type lists for passing
        element types.
    */
    template<typename ArgumentElements, typename ResultElements>
    class AccumulatingRouterFunctor;  // Not defined


    template<Arithmetic... ArgumentElements_, Arithmetic... ResultElements_>
    class AccumulatingRouterFunctor<
        policy::detail::TypeList<ArgumentElements_...>,
        policy::detail::TypeList<ResultElements_...>>
    {

        private:

            //! Type for representing the number of neighbouring cells draining into a cell (max 8)
            using InflowCountElement = SmallestIntegralElement;

        public:

            using ArgumentElements = std::tuple<ArgumentElements_...>;
            using ResultElements = std::tuple<ResultElements_...>;

            using Results = std::tuple<PartitionedArray<ResultElements_, 2>...>;
            using ActionResults = std::tuple<PartitionT<PartitionedArray<ResultElements_, 2>>...>;

            using InflowCountPartition = ArrayPartition<InflowCountElement, 2>;
            using InflowCountData = DataT<InflowCountPartition>;
            using CellsIdxs = std::vector<std::array<Index, 2>>;


            /*!
                @brief      Return a tuple of one or more collections of partitions to use for the
                            algorithm's resulting array(s)
                @param      shape_in_partitions Shape of each collection, in partitions

                The collections returned don't include final partitions themselves, just default constructed
                partitions. Each of these partitions will be overwritten later.
            */
            static auto initialize_results_partitions(auto const& shape_in_partitions)
                -> std::tuple<PartitionsT<PartitionedArray<ResultElements_, 2>>...>
            {
                return std::make_tuple(
                    PartitionsT<PartitionedArray<ResultElements_, 2>>{shape_in_partitions}...);
            }


            static auto initialize_results_partition_data_fs()
                -> std::tuple<hpx::future<DataT<PartitionT<PartitionedArray<ResultElements_, 2>>>>...>
            {
                return std::make_tuple(
                    hpx::future<DataT<PartitionT<PartitionedArray<ResultElements_, 2>>>>{}...);
            }


            static auto initialize_result_data(auto const& partition_shape)
                -> std::tuple<DataT<PartitionT<PartitionedArray<ResultElements_, 2>>>...>
            {
                return std::make_tuple(
                    DataT<PartitionT<PartitionedArray<ResultElements_, 2>>>{partition_shape /*, 0 */}...);
            }
    };

}  // namespace lue
