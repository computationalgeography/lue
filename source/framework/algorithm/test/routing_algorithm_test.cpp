#include "lue/framework/core/type_traits.hpp"
#include <concepts>
#define BOOST_TEST_MODULE lue framework algorithm routing_algorithm
#include "flow_accumulation.hpp"
#include "lue/framework/algorithm/definition/flow_accumulation3.hpp"
#include "lue/framework/algorithm/detail/communicator_array.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"

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
//   What about the related algorithms, like the one classifying cells into inter/intra parition stream
//   cells.


namespace lue {

    // TODO: Try to get rid of all the constexpr if

    template<typename Functor>
    using ResultT = typename Functor::Result;

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
    auto tied_references(
        std::tuple<hpx::future<ArrayPartitionData<Element, 2>>...>& results_partition_data_fs)
        -> std::tuple<hpx::future<ArrayPartitionData<Element, 2>>&...>
    {
        return std::apply(
            [](auto&... results_partition_data_fs) { return std::tie(results_partition_data_fs...); },
            results_partition_data_fs);
    }


    template<Arithmetic... Element, typename... Arguments>
    auto tied_references2(
        std::tuple<hpx::future<ArrayPartitionData<Element, 2>>...>& results_partition_data_fs,
        Arguments&... arguments) -> std::tuple<hpx::future<ArrayPartitionData<Element, 2>>&..., Arguments&...>
    {
        return std::apply(
            [&arguments...](auto&... results_partition_data_fs)
            { return std::tie(results_partition_data_fs..., arguments...); },
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
    using ActionResultT = typename Functor::ActionResult;

    template<typename Functor>
    using IntraPartitionStreamCellsResult = typename Functor::IntraPartitionStreamCellsResult;

    template<typename Functor>
    using InterPartitionStreamCellsResult = typename Functor::InterPartitionStreamCellsResult;


    namespace detail {

        /*!
            @brief      Create communicators used to exchange information about inflow counts and accumulated
                        material between localities
            @tparam     Type of algorithm-specific functor
            @param      localities Collection of localities
            @return     Tuple of arrays continaing communicators for exchanging inflow counts and accumulated
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
                     std::move(material_communicators)]([[maybe_unused]] auto&& partitions) mutable
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
            -> Scalar<Element> const&
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
            [[maybe_unused]] Policies const& policies,
            [[maybe_unused]] Functor const& functor,
            [[maybe_unused]] ArrayPartition<policy::InputElementT<Policies, 0>, 2> const&
                flow_direction_partition,
            [[maybe_unused]] Arguments const&... arguments,
            [[maybe_unused]] typename Functor::InflowCountPartition const& inflow_count_partition,
            [[maybe_unused]] MaterialCommunicator<MaterialT<Functor>, 2>& material_communicator,
            [[maybe_unused]] hpx::future<std::array<typename Functor::CellsIdxs, nr_neighbours<2>()>>&&
                output_cells_idxs_f) -> IntraPartitionStreamCellsResult<Functor>
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

                    [policies, material_communicator](
                        FlowDirectionPartition const& flow_direction_partition,
                        [[maybe_unused]] Arguments const&... arguments,
                        typename Functor::InflowCountPartition const& inflow_count_partition,
                        hpx::future<std::array<typename Functor::CellsIdxs, nr_neighbours<2>()>>&&
                            output_cells_idxs_f) mutable
                    {
                        AnnotateFunction const annotation{
                            "accumulating_router: partition: intra_partition_stream"};

                        auto const flow_direction_partition_ptr{
                            ready_component_ptr(flow_direction_partition)};
                        FlowDirectionData const& flow_direction_data{flow_direction_partition_ptr->data()};

                        auto const inflow_count_partition_ptr{ready_component_ptr(inflow_count_partition)};
                        InflowCountData const& inflow_count_data{inflow_count_partition_ptr->data()};

                        auto const& partition_shape{inflow_count_data.shape()};
                        [[maybe_unused]] auto const [nr_elements0, nr_elements1] = partition_shape;

                        // We need to copy inflow counts:
                        // - inflow counts are used to select ridge cells
                        // - downstream accumulation updates inflow counts
                        InflowCountData inflow_count_data_copy{deep_copy(inflow_count_data)};

                        [[maybe_unused]] auto const& indp_flow_direction =
                            std::get<0>(policies.inputs_policies()).input_no_data_policy();

                        auto output_cells_idxs{output_cells_idxs_f.get()};

                        // tuple<PartitionData...>
                        auto result_data = Functor::initialize_result_data(partition_shape);

                        auto cell_accumulator{std::apply(
                            [&policies, &arguments...](auto&... result_data)
                            {
                                return typename Functor::template CellAccumulator<decltype(get_data(
                                    arguments))...>{policies, get_data(arguments)..., result_data...};
                            },
                            result_data)};

                        Accumulator3 accumulator{
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
                                    accumulator.enter_at_ridge(idx0, idx1);

                                    accumulate3(
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
            [[maybe_unused]] Functor const& functor,
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

                [policies, material_communicator = std::move(material_communicator)](
                    FlowDirectionPartition const& flow_direction_partition,
                    Arguments const&... arguments,
                    hpx::future<InflowCountData>&& inflow_count_data_f,
                    hpx::shared_future<std::array<typename Functor::CellsIdxs, nr_neighbours<2>()>> const&
                        input_cells_idxs_f,
                    hpx::future<std::array<typename Functor::CellsIdxs, nr_neighbours<2>()>>&&
                        output_cells_idxs_f,
                    std::tuple<hpx::future<ArrayPartitionData<Results, 2>>...>&& results) mutable
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
                        [&policies, &arguments...](auto&... result_data)
                        {
                            return
                                typename Functor::template CellAccumulator<decltype(get_data(arguments))...>{
                                    policies, get_data(arguments)..., result_data...};
                        },
                        result_data);

                    auto output_cells_idxs{output_cells_idxs_f.get()};
                    Accumulator3 accumulator{
                        std::move(cell_accumulator), material_communicator, output_cells_idxs};

                    hpx::mutex accu_mutex{};
                    using MaterialElement = policy::OutputElementT<Policies, 0>;

                    auto accumulate =
                        [&accu_mutex, &accumulator, &flow_direction_data, &inflow_count_data](
                            std::array<Index, 2> const& cell_idxs, MaterialElement const value) mutable
                    {
                        auto [idx0, idx1] = cell_idxs;

                        // Prevent multiple threads from touching this data at the same time
                        std::scoped_lock lock{accu_mutex};

                        lue_hpx_assert(inflow_count_data(idx0, idx1) >= 1);

                        accumulator.enter_at_partition_input(value, idx0, idx1);

                        --inflow_count_data(idx0, idx1);

                        // Note that multiple streams from other partitions can join in a single partition
                        // input cell. Only start an accumulation if this is the last one.
                        if (inflow_count_data(idx0, idx1) == 0)
                        {
                            accumulate3(accumulator, idx0, idx1, flow_direction_data, inflow_count_data);
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


        template<typename Argument>
        using FutureT = hpx::future<Argument>;


        template<typename Policies, typename Functor, typename... Arguments>
        auto accumulate_partition(
            Policies const& policies,
            Functor const& functor,
            ArrayPartition<policy::InputElementT<Policies, 0>, 2> const& flow_direction_partition,
            Arguments const&... arguments,
            InflowCountCommunicator<2> inflow_count_communicator,
            MaterialCommunicator<MaterialT<Functor>, 2> material_communicator) -> ActionResultT<Functor>
        {
            auto [inflow_count_partition, input_cells_idxs_f, output_cells_idxs_f] = inflow_count3<Policies>(
                policies, flow_direction_partition, std::move(inflow_count_communicator));

            using FlowDirectionElement = policy::InputElementT<Policies, 0>;
            using FlowDirectionPartition = ArrayPartition<FlowDirectionElement, 2>;

            hpx::future<typename Functor::InflowCountData> inflow_count_data_f;
            auto results_partition_data_fs = Functor::initialize_results_partition_data_fs();

            // TODO: Fix the tied_references variants
            tied_references2(results_partition_data_fs, inflow_count_data_f, output_cells_idxs_f) =
                solve_intra_partition_stream_cells<Policies, Functor, Arguments...>(
                    policies,
                    functor,
                    flow_direction_partition,
                    arguments...,
                    inflow_count_partition,
                    material_communicator,
                    std::move(output_cells_idxs_f));
            tied_references(results_partition_data_fs) =
                solve_inter_partition_stream_cells<Policies, Functor, Arguments...>(
                    policies,
                    functor,
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
                       // hpx::future<hpx::tuple<hpx::future<lue::ArrayPartitionData<unsigned char, 2> > > >
                       auto&& results_partition_data_fs_f)
                    {
                        AnnotateFunction const annotation{"accumulate: partition: create_result_partitions"};

                        // hpx::tuple<hpx::future<lue::ArrayPartitionData<unsigned char, 2> > >
                        auto results_partition_data_fs = results_partition_data_fs_f.get();

                        Offset const partition_offset{
                            ready_component_ptr(flow_direction_partition)->offset()};

                        // std::apply icw hpx::tuple doesn't work
                        return std::apply(
                            [&partition_offset](auto&&... partition_data_fs)
                            {
                                return std::make_tuple(partition_data_to_partition(
                                    hpx::find_here(), partition_offset, partition_data_fs.get())...);
                            },
                            hpx_tuple_to_std_tuple(std::move(results_partition_data_fs)));
                    },

                    flow_direction_partition,
                    when_all(std::move(results_partition_data_fs))));

            return std::apply(
                [](auto&&... partition_fs)
                { return std::make_tuple(partition_f_to_partition(std::move(partition_fs))...); },
                results_partition_fs);
        }


        template<typename Policies, typename Functor, typename... Arguments>
        struct AccumulateAction:
            hpx::actions::make_action<
                decltype(&accumulate_partition<Policies, Functor, Arguments...>),
                &accumulate_partition<Policies, Functor, Arguments...>,
                AccumulateAction<Policies, Functor, Arguments...>>::type
        {
        };

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
        Arguments const&... arguments) -> ResultT<Functor>
    {
        Localities<2> const& localities{flow_direction.localities()};

        auto [inflow_count_communicators, material_communicators] =
            detail::create_communicators<Functor>(localities);

        auto const& shape_in_partitions{flow_direction.partitions().shape()};
        Count const nr_partitions{nr_elements(shape_in_partitions)};

        using Action = detail::AccumulateAction<Policies, Functor, detail::PassedArgumentT<Arguments>...>;
        Action action{};

        auto results_partitions = Functor::initialize_results_partitions(shape_in_partitions);

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

        return results_partitions_to_arrays(results_partitions, flow_direction.shape(), localities);
    }


    template<typename ArgumentElements, typename ResultElements>
    class AccumulatingRouterFunctor;  // Not defined


    /*!
        @brief      Base class for accumulating router functors
    */
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

            // TODO: Rename: ResultArrays
            using Result = std::tuple<PartitionedArray<ResultElements_, 2>...>;

            // TODO: Rename: ActionResults(?)
            using ActionResult = std::tuple<PartitionT<PartitionedArray<ResultElements_, 2>>...>;

            using InflowCountPartition = ArrayPartition<InflowCountElement, 2>;
            using InflowCountData = DataT<InflowCountPartition>;
            using CellsIdxs = std::vector<std::array<Index, 2>>;


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


                    CellAccumulator(Policies const& policies, Material const& inflow, MaterialData& outflow):

                        _dp{policies.domain_policy()},
                        _indp_inflow{std::get<1>(policies.inputs_policies()).input_no_data_policy()},
                        _ondp_outflow{std::get<0>(policies.outputs_policies()).output_no_data_policy()},

                        _inflow{inflow},
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
                        MaterialElement const& inflow{detail::to_value(_inflow, idx0, idx1)};

                        MaterialElement& outflow{_outflow(idx0, idx1)};

                        if (!_ondp_outflow.is_no_data(outflow))
                        {
                            if (_indp_inflow.is_no_data(inflow) || !_dp.within_domain(inflow))
                            {
                                _ondp_outflow.mark_no_data(outflow);
                            }
                            else
                            {
                                // Now we know the final, total amount
                                // of inflow that enters this cell
                                outflow += inflow;
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

                    Material const _inflow;  // External inflow

                    MaterialData& _outflow;  // Upstream inflow, outflow
            };


            static constexpr char const* name{"accu4"};

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


    // raster
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        PartitionedArray<MaterialElement, 2> const& inflow) -> PartitionedArray<MaterialElement, 2>;

    // scalar
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        Scalar<MaterialElement> const& inflow) -> PartitionedArray<MaterialElement, 2>;


    // value
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        MaterialElement const& inflow) -> PartitionedArray<MaterialElement, 2>;


    // raster
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        PartitionedArray<MaterialElement, 2> const& inflow) -> PartitionedArray<MaterialElement, 2>
    {
        return std::get<0>(accumulating_router(policies, Accu<Policies>{}, flow_direction, inflow));
    }


    // scalar
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        Scalar<MaterialElement> const& inflow) -> PartitionedArray<MaterialElement, 2>
    {
        return std::get<0>(accumulating_router(policies, Accu<Policies>{}, flow_direction, inflow));
    }


    // value
    template<typename Policies, std::integral FlowDirectionElement, std::floating_point MaterialElement>
    auto accu4(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
        MaterialElement const& inflow) -> PartitionedArray<MaterialElement, 2>
    {
        return std::get<0>(
            accumulating_router(policies, Accu<Policies>{}, flow_direction, Scalar<MaterialElement>{inflow}));
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


    // template<typename Policies>
    // class AccuCapacity: public AccumulatingRouterFunctor
    // {
    //     public:
    //
    //         using AccumulatingRouterFunctor::CellsIdxs;
    //         using AccumulatingRouterFunctor::InflowCountData;
    //
    //         static constexpr char const* name{"accu_capacity"};
    //
    //         using Material = policy::InputElementT<Policies, 1>;
    //
    //         using Result = s       PartitionedArray<policy::OutputElementT<Policies, 0>, 2>,
    //             PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>;
    //
    //         using ActionResult = std::tuple<
    //             PartitionT<PartitionedArray<policy::OutputElementT<Policies, 0>, 2>>,
    //             PartitionT<PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>>;
    //
    //         using MaterialPartition = ArrayPartition<Material, 2>;
    //         using MaterialData = DataT<MaterialPartition>;
    //
    //         using IntraPartitionStreamCellsResult = std::tuple<
    //             hpx::future<MaterialData>,
    //             hpx::future<MaterialData>,
    //             hpx::future<InflowCountData>,
    //             hpx::future<std::array<CellsIdxs, detail::nr_neighbours<2>()>>>;
    //
    //         using InterPartitionStreamCellsResult = hpx::future<std::tuple<MaterialData, MaterialData>>;
    // };
    //
    //
    // template<typename Policies>
    // class AccuTrigger: public AccumulatingRouterFunctor
    // {
    //     public:
    //
    //         using AccumulatingRouterFunctor::CellsIdxs;
    //         using AccumulatingRouterFunctor::InflowCountData;
    //
    //         static constexpr char const* name{"accu_trigger"};
    //
    //         using Material = policy::InputElementT<Policies, 1>;
    //
    //         using Result = std::tuple<
    //             PartitionedArray<policy::OutputElementT<Policies, 0>, 2>,
    //             PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>;
    //
    //         using ActionResult = std::tuple<
    //             PartitionT<PartitionedArray<policy::OutputElementT<Policies, 0>, 2>>,
    //             PartitionT<PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>>;
    //
    //         using MaterialPartition = ArrayPartition<Material, 2>;
    //         using MaterialData = DataT<MaterialPartition>;
    //
    //         using IntraPartitionStreamCellsResult = std::tuple<
    //             hpx::future<MaterialData>,
    //             hpx::future<MaterialData>,
    //             hpx::future<InflowCountData>,
    //             hpx::future<std::array<CellsIdxs, detail::nr_neighbours<2>()>>>;
    //
    //         using InterPartitionStreamCellsResult = hpx::future<std::tuple<MaterialData, MaterialData>>;
    // };
    //
    //
    // template<typename Policies>
    // class KinematicWave: public AccumulatingRouterFunctor
    // {
    //     public:
    //
    //         using AccumulatingRouterFunctor::CellsIdxs;
    //         using AccumulatingRouterFunctor::InflowCountData;
    //
    //         static constexpr char const* name{"kinematic_wave"};
    //
    //         using Material = policy::InputElementT<Policies, 1>;
    //
    //         using Result = std::tuple<
    //             PartitionedArray<policy::OutputElementT<Policies, 0>, 2>,
    //             PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>;
    //
    //         using ActionResult = std::tuple<
    //             PartitionT<PartitionedArray<policy::OutputElementT<Policies, 0>, 2>>,
    //             PartitionT<PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>>;
    //
    //         using MaterialPartition = ArrayPartition<Material, 2>;
    //         using MaterialData = DataT<MaterialPartition>;
    //
    //         using IntraPartitionStreamCellsResult = std::tuple<
    //             hpx::future<MaterialData>,
    //             hpx::future<MaterialData>,
    //             hpx::future<InflowCountData>,
    //             hpx::future<std::array<CellsIdxs, detail::nr_neighbours<2>()>>>;
    //
    //         using InterPartitionStreamCellsResult = hpx::future<std::tuple<MaterialData, MaterialData>>;
    // };


    // TODO: Hier verder
    // TODO: Add overloads of kinematic_wave4 below here and implement functor above here


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


    namespace policy::accu4 {

        template<std::floating_point MaterialElement>
        class DomainPolicy
        {

            public:

                static constexpr auto within_domain(MaterialElement const inflow) noexcept -> bool
                {
                    return inflow >= 0;
                }
        };


        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            DomainPolicy<MaterialElement>,
            OutputElements<MaterialElement>,
            InputElements<FlowDirectionElement, MaterialElement>>;

    }  // namespace policy::accu4


    namespace value_policies {

        // raster
        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        auto accu4(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            PartitionedArray<MaterialElement, 2> const& inflow) -> PartitionedArray<MaterialElement, 2>
        {
            using Policies = lue::policy::accu4::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu4(Policies{}, flow_direction, inflow);
        }


        // scalar
        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        auto accu4(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction,
            Scalar<MaterialElement> const& inflow) -> PartitionedArray<MaterialElement, 2>
        {
            using Policies = lue::policy::accu4::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu4(Policies{}, flow_direction, inflow);
        }


        // value
        template<std::integral FlowDirectionElement, std::floating_point MaterialElement>
        auto accu4(
            PartitionedArray<FlowDirectionElement, 2> const& flow_direction, MaterialElement const& inflow)
            -> PartitionedArray<MaterialElement, 2>
        {
            using Policies = lue::policy::accu4::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;

            return accu4(Policies{}, flow_direction, inflow);
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


    namespace policy::accu_capacity {

        // TODO: Check policies
        template<std::integral FlowDirection, std::floating_point FloatingPointElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<FlowDirection, FloatingPointElement, FloatingPointElement>,
            OutputElements<FloatingPointElement, FloatingPointElement>,
            InputElements<FlowDirection, FloatingPointElement, FloatingPointElement>>;

    }  // namespace policy::accu_capacity


    namespace policy::accu_trigger {

        // TODO: Check policies
        template<std::integral FlowDirection, std::floating_point FloatingPointElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<FlowDirection, FloatingPointElement, FloatingPointElement>,
            OutputElements<FloatingPointElement, FloatingPointElement>,
            InputElements<FlowDirection, FloatingPointElement, FloatingPointElement>>;

    }  // namespace policy::accu_trigger


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
            OutputElements<MaterialElement, MaterialElement>,
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
            -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
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
            MaterialElement const& channel_length)
            -> std::tuple<PartitionedArray<MaterialElement, 2>, PartitionedArray<MaterialElement, 2>>
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


BOOST_AUTO_TEST_CASE(accu4_overloads)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    lue::PartitionedArray<FlowDirectionElement, 2> const flow_direction{};
    lue::PartitionedArray<MaterialElement, 2> outflow{};

    lue::PartitionedArray<MaterialElement, 2> const material_raster{};
    lue::Scalar<MaterialElement> const material_scalar{};
    MaterialElement const material_value{};

    // raster
    outflow = lue::value_policies::accu4(flow_direction, material_raster);

    // scalar
    outflow = lue::value_policies::accu4(flow_direction, material_scalar);

    // value
    outflow = lue::value_policies::accu4(flow_direction, material_value);
}


BOOST_AUTO_TEST_CASE(accu4_parallel_east)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    auto const flow_direction = lue::create_partitioned_array<FlowDirectionElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, lue::test::e);
    auto const inflow = lue::create_partitioned_array<MaterialElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, 1);

    auto const outflow_we_got = lue::value_policies::accu4(flow_direction, inflow);

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
                },
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(accu4_parallel_south_east)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    auto const flow_direction = lue::create_partitioned_array<FlowDirectionElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, lue::test::se);
    auto const inflow = lue::create_partitioned_array<MaterialElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, 1);

    auto const outflow_we_got = lue::value_policies::accu4(flow_direction, inflow);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            lue::test::array_shape,
            lue::test::partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    1, 1, 1,
                    1, 2, 2,
                    1, 2, 3,
                },
                {
                    1, 1, 1,
                    2, 2, 2,
                    3, 3, 3,
                },
                {
                    1, 1, 1,
                    2, 2, 2,
                    3, 3, 3,
                },
                {
                    1, 2, 3,
                    1, 2, 3,
                    1, 2, 3,
                },
                {
                    4, 4, 4,
                    4, 5, 5,
                    4, 5, 6,
                },
                {
                    4, 4, 4,
                    5, 5, 5,
                    6, 6, 6,
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
                    7, 7, 7,
                    7, 8, 8,
                    7, 8, 9,
                },
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(accu4_parallel_south)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    auto const flow_direction = lue::create_partitioned_array<FlowDirectionElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, lue::test::s);
    auto const inflow = lue::create_partitioned_array<MaterialElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, 1);

    auto const outflow_we_got = lue::value_policies::accu4(flow_direction, inflow);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            lue::test::array_shape,
            lue::test::partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    1, 1, 1,
                    2, 2, 2,
                    3, 3, 3,
                },
                {
                    1, 1, 1,
                    2, 2, 2,
                    3, 3, 3,
                },
                {
                    1, 1, 1,
                    2, 2, 2,
                    3, 3, 3,
                },
                {
                    4, 4, 4,
                    5, 5, 5,
                    6, 6, 6,
                },
                {
                    4, 4, 4,
                    5, 5, 5,
                    6, 6, 6,
                },
                {
                    4, 4, 4,
                    5, 5, 5,
                    6, 6, 6,
                },
                {
                    7, 7, 7,
                    8, 8, 8,
                    9, 9, 9,
                },
                {
                    7, 7, 7,
                    8, 8, 8,
                    9, 9, 9,
                },
                {
                    7, 7, 7,
                    8, 8, 8,
                    9, 9, 9,
                },
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(accu4_parallel_south_west)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    auto const flow_direction = lue::create_partitioned_array<FlowDirectionElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, lue::test::sw);
    auto const inflow = lue::create_partitioned_array<MaterialElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, 1);

    auto const outflow_we_got = lue::value_policies::accu4(flow_direction, inflow);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            lue::test::array_shape,
            lue::test::partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    1, 1, 1,
                    2, 2, 2,
                    3, 3, 3,
                },
                {
                    1, 1, 1,
                    2, 2, 2,
                    3, 3, 3,
                },
                {
                    1, 1, 1,
                    2, 2, 1,
                    3, 2, 1,
                },
                {
                    4, 4, 4,
                    5, 5, 5,
                    6, 6, 6,
                },
                {
                    4, 4, 4,
                    5, 5, 4,
                    6, 5, 4,
                },
                {
                    3, 2, 1,
                    3, 2, 1,
                    3, 2, 1,
                },
                {
                    7, 7, 7,
                    8, 8, 7,
                    9, 8, 7,
                },
                {
                    6, 5, 4,
                    6, 5, 4,
                    6, 5, 4,
                },
                {
                    3, 2, 1,
                    3, 2, 1,
                    3, 2, 1,
                },
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(accu4_parallel_west)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    auto const flow_direction = lue::create_partitioned_array<FlowDirectionElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, lue::test::w);
    auto const inflow = lue::create_partitioned_array<MaterialElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, 1);

    auto const outflow_we_got = lue::value_policies::accu4(flow_direction, inflow);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            lue::test::array_shape,
            lue::test::partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    9, 8, 7,
                    9, 8, 7,
                    9, 8, 7,
                },
                {
                    6, 5, 4,
                    6, 5, 4,
                    6, 5, 4,
                },
                {
                    3, 2, 1,
                    3, 2, 1,
                    3, 2, 1,
                },
                {
                    9, 8, 7,
                    9, 8, 7,
                    9, 8, 7,
                },
                {
                    6, 5, 4,
                    6, 5, 4,
                    6, 5, 4,
                },
                {
                    3, 2, 1,
                    3, 2, 1,
                    3, 2, 1,
                },
                {
                    9, 8, 7,
                    9, 8, 7,
                    9, 8, 7,
                },
                {
                    6, 5, 4,
                    6, 5, 4,
                    6, 5, 4,
                },
                {
                    3, 2, 1,
                    3, 2, 1,
                    3, 2, 1,
                },
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(accu4_parallel_north_west)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    auto const flow_direction = lue::create_partitioned_array<FlowDirectionElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, lue::test::nw);
    auto const inflow = lue::create_partitioned_array<MaterialElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, 1);

    auto const outflow_we_got = lue::value_policies::accu4(flow_direction, inflow);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            lue::test::array_shape,
            lue::test::partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    9, 8, 7,
                    8, 8, 7,
                    7, 7, 7,
                },
                {
                    6, 5, 4,
                    6, 5, 4,
                    6, 5, 4,
                },
                {
                    3, 2, 1,
                    3, 2, 1,
                    3, 2, 1,
                },
                {
                    6, 6, 6,
                    5, 5, 5,
                    4, 4, 4,
                },
                {
                    6, 5, 4,
                    5, 5, 4,
                    4, 4, 4,
                },
                {
                    3, 2, 1,
                    3, 2, 1,
                    3, 2, 1,
                },
                {
                    3, 3, 3,
                    2, 2, 2,
                    1, 1, 1,
                },
                {
                    3, 3, 3,
                    2, 2, 2,
                    1, 1, 1,
                },
                {
                    3, 2, 1,
                    2, 2, 1,
                    1, 1, 1,
                },
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(accu4_parallel_north)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    auto const flow_direction = lue::create_partitioned_array<FlowDirectionElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, lue::test::n);
    auto const inflow = lue::create_partitioned_array<MaterialElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, 1);

    auto const outflow_we_got = lue::value_policies::accu4(flow_direction, inflow);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            lue::test::array_shape,
            lue::test::partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    9, 9, 9,
                    8, 8, 8,
                    7, 7, 7,
                },
                {
                    9, 9, 9,
                    8, 8, 8,
                    7, 7, 7,
                },
                {
                    9, 9, 9,
                    8, 8, 8,
                    7, 7, 7,
                },
                {
                    6, 6, 6,
                    5, 5, 5,
                    4, 4, 4,
                },
                {
                    6, 6, 6,
                    5, 5, 5,
                    4, 4, 4,
                },
                {
                    6, 6, 6,
                    5, 5, 5,
                    4, 4, 4,
                },
                {
                    3, 3, 3,
                    2, 2, 2,
                    1, 1, 1,
                },
                {
                    3, 3, 3,
                    2, 2, 2,
                    1, 1, 1,
                },
                {
                    3, 3, 3,
                    2, 2, 2,
                    1, 1, 1,
                },
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(parallel_north_east)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    auto const flow_direction = lue::create_partitioned_array<FlowDirectionElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, lue::test::ne);
    auto const inflow = lue::create_partitioned_array<MaterialElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, 1);

    auto const outflow_we_got = lue::value_policies::accu4(flow_direction, inflow);

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
                    7, 8, 8,
                    7, 7, 7,
                },
                {
                    1, 2, 3,
                    1, 2, 3,
                    1, 2, 3,
                },
                {
                    4, 5, 6,
                    4, 5, 5,
                    4, 4, 4,
                },
                {
                    6, 6, 6,
                    5, 5, 5,
                    4, 4, 4,
                },
                {
                    1, 2, 3,
                    1, 2, 2,
                    1, 1, 1,
                },
                {
                    3, 3, 3,
                    2, 2, 2,
                    1, 1, 1,
                },
                {
                    3, 3, 3,
                    2, 2, 2,
                    1, 1, 1,
                },
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(accu4_converge)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    auto const n = lue::test::n;
    auto const ne = lue::test::ne;
    auto const e = lue::test::e;
    auto const se = lue::test::se;
    auto const s = lue::test::s;
    auto const sw = lue::test::sw;
    auto const w = lue::test::w;
    auto const nw = lue::test::nw;
    auto const p = lue::test::p;

    auto const flow_direction =
        lue::test::create_partitioned_array<lue::PartitionedArray<FlowDirectionElement, 2>>(
            lue::test::array_shape,
            lue::test::partition_shape,
            {
                // clang-format off
                {
                    se, se, se,
                    se, se, se,
                    se, se, se,
                },
                {
                    s, s, s,
                    s, s, s,
                    s, s, s,
                },
                {
                    sw, sw, sw,
                    sw, sw, sw,
                    sw, sw, sw,
                },
                {
                    e, e, e,
                    e, e, e,
                    e, e, e,
                },
                {
                    se, s, sw,
                    e, p, w,
                    ne, n, nw,
                },
                {
                    w, w, w,
                    w, w, w,
                    w, w, w,
                },
                {
                    ne, ne, ne,
                    ne, ne, ne,
                    ne, ne, ne,
                },
                {
                    n, n, n,
                    n, n, n,
                    n, n, n,
                },
                {
                    nw, nw, nw,
                    nw, nw, nw,
                    nw, nw, nw,
                },
                // clang-format on
            });
    auto const inflow = lue::create_partitioned_array<MaterialElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, 1);

    auto const outflow_we_got = lue::value_policies::accu4(flow_direction, inflow);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            lue::test::array_shape,
            lue::test::partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    1, 1, 1,
                    1, 2, 2,
                    1, 2, 3,
                },
                {
                    1, 1, 1,
                    3, 2, 3,
                    6, 3, 6,
                },
                {
                    1, 1, 1,
                    2, 2, 1,
                    3, 2, 1,
                },
                {
                    1, 3, 6,
                    1, 2, 3,
                    1, 3, 6,
                },
                {
                    16, 4, 16,
                    4, 81, 4,
                    16, 4, 16,
                },
                {
                    6, 3, 1,
                    3, 2, 1,
                    6, 3, 1,
                },
                {
                    1, 2, 3,
                    1, 2, 2,
                    1, 1, 1,
                },
                {
                    6, 3, 6,
                    3, 2, 3,
                    1, 1, 1,
                },
                {
                    3, 2, 1,
                    2, 2, 1,
                    1, 1, 1,
                },
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(accu4_diverge)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    auto const n = lue::test::n;
    auto const ne = lue::test::ne;
    auto const e = lue::test::e;
    auto const se = lue::test::se;
    auto const s = lue::test::s;
    auto const sw = lue::test::sw;
    auto const w = lue::test::w;
    auto const nw = lue::test::nw;

    auto const flow_direction =
        lue::test::create_partitioned_array<lue::PartitionedArray<FlowDirectionElement, 2>>(
            lue::test::array_shape,
            lue::test::partition_shape,
            {
                // clang-format off
                {
                    nw, nw, nw,
                    nw, nw, nw,
                    nw, nw, nw,
                },
                {
                    n, n, n,
                    n, n, n,
                    n, n, n,
                },
                {
                    ne, ne, ne,
                    ne, ne, ne,
                    ne, ne, ne,
                },
                {
                    w, w, w,
                    w, w, w,
                    w, w, w,
                },
                {
                    nw, n, ne,
                    w, s, e,
                    sw, s, se,
                },
                {
                    e, e, e,
                    e, e, e,
                    e, e, e,
                },
                {
                    sw, sw, sw,
                    sw, sw, sw,
                    sw, sw, sw,
                },
                {
                    s, s, s,
                    s, s, s,
                    s, s, s,
                },
                {
                    se, se, se,
                    se, se, se,
                    se, se, se,
                },
                // clang-format on
            });
    auto const inflow = lue::create_partitioned_array<MaterialElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, 1);

    auto const outflow_we_got = lue::value_policies::accu4(flow_direction, inflow);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            lue::test::array_shape,
            lue::test::partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    4, 2, 1,
                    2, 3, 1,
                    1, 1, 2,
                },
                {
                    3, 4, 3,
                    2, 3, 2,
                    1, 2, 1,
                },
                {
                    1, 2, 4,
                    1, 3, 2,
                    2, 1, 1,
                },
                {
                    3, 2, 1,
                    4, 3, 2,
                    3, 2, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 2, 1,
                },
                {
                    1, 2, 3,
                    2, 3, 4,
                    1, 2, 3,
                },
                {
                    1, 1, 2,
                    2, 3, 1,
                    4, 2, 1,
                },
                {
                    1, 3, 1,
                    2, 4, 2,
                    3, 5, 3,
                },
                {
                    2, 1, 1,
                    1, 3, 2,
                    1, 2, 4,
                },
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(accu4_spiral_in_case)
{
    using MaterialElement = lue::FloatingPointElement<0>;

    auto const flow_direction = lue::test::spiral_in();
    auto const inflow = lue::test::ones<MaterialElement>();

    auto const outflow_we_got = lue::value_policies::accu4(flow_direction, inflow);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
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

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(spiral_out)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    auto const n = lue::test::n;
    auto const e = lue::test::e;
    auto const s = lue::test::s;
    auto const w = lue::test::w;

    auto const flow_direction =
        lue::test::create_partitioned_array<lue::PartitionedArray<FlowDirectionElement, 2>>(
            lue::test::array_shape,
            lue::test::partition_shape,
            {
                // clang-format off
                {
                    w, w, w,
                    s, w, w,
                    s, s, w,
                },
                {
                    w, w, w,
                    w, w, w,
                    w, w, w,
                },
                {
                    w, w, w,
                    w, w, n,
                    w, n, n,
                },
                {
                    s, s, s,
                    s, s, s,
                    s, s, s,
                },
                {
                    w, w, w,
                    s, w, n,
                    e, e, n,
                },
                {
                    n, n, n,
                    n, n, n,
                    n, n, n,
                },
                {
                    s, s, e,
                    s, e, e,
                    e, e, e,
                },
                {
                    e, e, e,
                    e, e, e,
                    e, e, e,
                },
                {
                    n, n, n,
                    e, n, n,
                    e, e, n,
                },
                // clang-format on
            });
    auto const inflow = lue::create_partitioned_array<MaterialElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, 1);

    auto const outflow_we_got = lue::value_policies::accu4(flow_direction, inflow);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            lue::test::array_shape,
            lue::test::partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    81, 80, 79,
                    50, 49, 48,
                    51, 26, 25,
                },
                {
                    78, 77, 76,
                    47, 46, 45,
                    24, 23, 22,
                },
                {
                    75, 74, 73,
                    44, 43, 72,
                    21, 42, 71,
                },
                {
                    52, 27, 10,
                    53, 28, 11,
                    54, 29, 12,
                },
                {
                    9, 8, 7,
                    2, 1, 6,
                    3, 4, 5,
                },
                {
                    20, 41, 70,
                    19, 40, 69,
                    18, 39, 68,
                },
                {
                    55, 30, 13,
                    56, 31, 32,
                    57, 58, 59,
                },
                {
                    14, 15, 16,
                    33, 34, 35,
                    60, 61, 62,
                },
                {
                    17, 38, 67,
                    36, 37, 66,
                    63, 64, 65,
                },
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(accu4_all_no_data_flow_direction)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    auto const flow_direction = lue::test::no_data<FlowDirectionElement>();
    auto const inflow = lue::test::ones<MaterialElement>();

    auto const outflow_we_got = lue::value_policies::accu4(flow_direction, inflow);

    auto const outflow_we_want = lue::test::filled(lue::policy::no_data_value<MaterialElement>);

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(all_no_data_material)
{
    using MaterialElement = lue::FloatingPointElement<0>;

    auto const e = lue::test::e;

    auto const flow_direction = lue::test::filled(e);
    auto const inflow = lue::test::no_data<MaterialElement>();

    auto const outflow_we_got = lue::value_policies::accu4(flow_direction, inflow);

    auto const outflow_we_want = lue::test::filled(lue::policy::no_data_value<MaterialElement>);

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(merging_streams_case_01)
{
    // No-data material in ridge cell. From there on, it must propagate
    // down to all downstream cells, in all downstream partitions.

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
            },
            // clang-format on
            // NOLINTEND
        });

    auto const outflow_we_got = lue::value_policies::accu4(flow_direction, inflow);

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
                    x, x, 1,
                },
                {
                    // 0, 1
                    x, 1, 1,
                    x, 1, 1,
                    x, 2, x,
                },
                {
                    // 0, 2
                    x, x, 1,
                    3, x, 1,
                    x, x, 1,
                },
                {
                    // 1, 0
                    x, x, 1,
                    x, x, 1,
                    x, x, 1,
                },
                {
                    // 1, 1
                    4, x, x,
                    x, x, 2,
                    2, x, 3,
                },
                {
                    // 1, 2
                    x, x, x,
                    1, x, x,
                    1, x, x,
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
                    x, x, 2,
                    4, 3, 2,
                },
                {
                    // 2, 2
                    1, x, x,
                    1, x, x,
                    1, x, x,
                },
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(back_and_forth)
{
    // Stream which runs from one partition to another and back and back
    // and back ...

    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    lue::ShapeT<lue::test::FlowDirectionArray> const array_shape{{6, 5}};
    lue::ShapeT<lue::test::FlowDirectionArray> const partition_shape{{3, 5}};

    auto const n = lue::test::n;
    auto const e = lue::test::e;
    auto const s = lue::test::s;
    auto const p = lue::test::p;

    auto const flow_direction =
        lue::test::create_partitioned_array<lue::PartitionedArray<FlowDirectionElement, 2>>(
            array_shape,
            partition_shape,
            {
                // clang-format off
                {
                    s, e, s, e, s,
                    s, n, s, n, s,
                    s, n, s, n, s,
                },
                {
                    s, n, s, n, s,
                    s, n, s, n, s,
                    e, n, e, n, p,
                },
                // clang-format on
            });
    MaterialElement const inflow = 1;

    auto outflow_we_got = lue::value_policies::accu4(flow_direction, inflow);


    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            array_shape,
            partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    1, 12, 13, 24, 25,
                    2, 11, 14, 23, 26,
                    3, 10, 15, 22, 27,
                },
                {
                    4, 9, 16, 21, 28,
                    5, 8, 17, 20, 29,
                    6, 7, 18, 19, 30,
                },
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
}


// TODO: Out of domain values: negative material is out of domain!


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


// BOOST_AUTO_TEST_CASE(accu_capacity)
// {
//     using FlowDirectionElement = std::uint8_t;
//     using FloatingPointElement = float;
//     using Policies =
//         lue::policy::accu_capacity::DefaultValuePolicies<FlowDirectionElement, FloatingPointElement>;
//     using Functor = lue::AccuCapacity<Policies>;
//
//     lue::PartitionedArray<FlowDirectionElement, 2> const flow_direction{};
//     [[maybe_unused]] lue::PartitionedArray<FloatingPointElement, 2> outflow{};
//     [[maybe_unused]] lue::PartitionedArray<FloatingPointElement, 2> residue{};
//
//     {
//         lue::PartitionedArray<FloatingPointElement, 2> const material{};
//         lue::PartitionedArray<FloatingPointElement, 2> const capacity{};
//
//         std::tie(outflow, residue) =
//             accumulating_router(Policies{}, Functor{}, flow_direction, material, capacity);
//     }
//
//     {
//         FloatingPointElement const material{};
//         FloatingPointElement const capacity{};
//
//         std::tie(outflow, residue) =
//             accumulating_router(Policies{}, Functor{}, flow_direction, material, capacity);
//     }
// }
//
//
// BOOST_AUTO_TEST_CASE(accu_trigger)
// {
//     using FlowDirectionElement = std::uint8_t;
//     using FloatingPointElement = float;
//     using Policies =
//         lue::policy::accu_trigger::DefaultValuePolicies<FlowDirectionElement, FloatingPointElement>;
//     using Functor = lue::AccuTrigger<Policies>;
//
//     lue::PartitionedArray<FlowDirectionElement, 2> const flow_direction{};
//     [[maybe_unused]] lue::PartitionedArray<FloatingPointElement, 2> outflow{};
//     [[maybe_unused]] lue::PartitionedArray<FloatingPointElement, 2> residue{};
//
//     {
//         lue::PartitionedArray<FloatingPointElement, 2> const material{};
//         lue::PartitionedArray<FloatingPointElement, 2> const trigger{};
//
//         std::tie(outflow, residue) =
//             accumulating_router(Policies{}, Functor{}, flow_direction, material, trigger);
//     }
//
//     {
//         FloatingPointElement const material{};
//         FloatingPointElement const trigger{};
//
//         std::tie(outflow, residue) =
//             accumulating_router(Policies{}, Functor{}, flow_direction, material, trigger);
//     }
// }


// BOOST_AUTO_TEST_CASE(kinematic_wave4_overloads)
// {
//     using FlowDirectionElement = lue::FlowDirectionElement;
//     using MaterialElement = lue::FloatingPointElement<0>;
//
//     lue::PartitionedArray<FlowDirectionElement, 2> const flow_direction{};
//     [[maybe_unused]] lue::PartitionedArray<MaterialElement, 2> outflow{};
//     [[maybe_unused]] lue::PartitionedArray<MaterialElement, 2> residue{};
//
//     // TODO: Which combinations make sense and which don't?
//     // TODO: Revisit all routing overloads: supporting scalars only makes sense if these arguments are
//     //       typically computed. Don't support them if that does not make sense.
//     lue::PartitionedArray<MaterialElement, 2> const current_outflow_raster{};
//     lue::PartitionedArray<MaterialElement, 2> const inflow_raster{};
//     lue::PartitionedArray<MaterialElement, 2> const alpha_raster{};
//     lue::PartitionedArray<MaterialElement, 2> const beta_raster{};
//     lue::PartitionedArray<MaterialElement, 2> const channel_length_raster{};
//
//     MaterialElement const inflow_value{};
//     MaterialElement const alpha_value{};
//     MaterialElement const beta_value{};
//     MaterialElement const time_step_duration_value{};
//     MaterialElement const channel_length_value{};
//
//     std::tie(outflow, residue) = lue::value_policies::kinematic_wave4(
//         flow_direction,
//         current_outflow_raster,
//         inflow_raster,
//         alpha_raster,
//         beta_raster,
//         time_step_duration_value,
//         channel_length_raster);
//
//     std::tie(outflow, residue) = lue::value_policies::kinematic_wave4(
//         flow_direction,
//         current_outflow_raster,
//         inflow_value,
//         alpha_value,
//         beta_value,
//         time_step_duration_value,
//         channel_length_value);
// }


// TODO: Make the tests below work


// namespace tt = boost::test_tools;
//
//
// BOOST_AUTO_TEST_CASE(pcraster_manual_example)
// {
//     // https://pcraster.geo.uu.nl/pcraster/4.4.1/documentation/pcraster_manual/sphinx/op_kinematic.html
//     auto flow_direction = lue::test::pcraster_example_flow_direction();
//
//     auto const array_shape{flow_direction.shape()};
//     auto const partition_shape{array_shape};
//
//     using Element = lue::FloatingPointElement<0>;
//
//     auto const current_discharge = lue::test::create_partitioned_array<lue::PartitionedArray<Element, 2>>(
//         array_shape,
//         partition_shape,
//         // clang-format off
//         {{
//             10, 10, 10, 10, 10,
//             10, 10, 10, 10, 10,
//             10, 10, 10, 10, 10,
//             10, 10, 10, 10, 50,
//             50, 50, 50, 50, 49,
//         }}  // clang-format on
//     );
//
//     auto const inflow = lue::create_partitioned_array<Element>(array_shape, partition_shape, 0);
//
//     Element const alpha = 1.5;
//     Element const beta = 0.6;
//     Element const time_step_duration = 15;
//
//     auto const channel_length = lue::create_partitioned_array<Element>(array_shape, partition_shape, 10);
//
//     auto const discharge_we_want = lue::test::create_partitioned_array<lue::PartitionedArray<Element, 2>>(
//         array_shape,
//         partition_shape,
//         // clang-format off
//         {{
//             2.32293,  2.32293,  2.32293,  2.32293, 2.32293,
//             4.00491,  4.00491,  5.76591,  4.00491, 2.32293,
//             5.27401,  9.81514,  8.40418,  4.00491, 2.32293,
//             2.32293, 21.22170,  9.68365,  9.60760, 7.19019,
//             7.19019, 57.27763, 17.74742, 12.90772, 7.09124,
//         }}  // clang-format on
//     );
//
//     auto const new_discharge = lue::value_policies::kinematic_wave(
//         flow_direction, current_discharge, inflow, alpha, beta, time_step_duration, channel_length);
//
//     lue::test::check_arrays_are_close(new_discharge, discharge_we_want);
// }
//
//
// BOOST_AUTO_TEST_CASE(zero_discharge_and_inflow)
// {
//     auto flow_direction = lue::test::pcraster_example_flow_direction();
//
//     auto const array_shape{flow_direction.shape()};
//     auto const partition_shape{array_shape};
//
//     using Element = lue::FloatingPointElement<0>;
//
//     auto const current_discharge = lue::create_partitioned_array<Element>(array_shape, partition_shape, 0);
//     auto const inflow = lue::create_partitioned_array<Element>(array_shape, partition_shape, 0);
//
//     Element const alpha = 1.5;
//     Element const beta = 0.6;
//     Element const time_step_duration = 15;
//
//     auto const channel_length = lue::create_partitioned_array<Element>(array_shape, partition_shape, 10);
//
//     auto const discharge_we_want = lue::create_partitioned_array<Element>(array_shape, partition_shape, 0);
//
//     auto const new_discharge = lue::value_policies::kinematic_wave(
//         flow_direction, current_discharge, inflow, alpha, beta, time_step_duration, channel_length);
//
//     lue::test::check_arrays_are_close(new_discharge, discharge_we_want);
// }
//
//
// BOOST_AUTO_TEST_CASE(non_zero_discharge_and_zero_inflow)
// {
//     auto flow_direction = lue::test::pcraster_example_flow_direction();
//
//     auto const array_shape{flow_direction.shape()};
//     auto const partition_shape{array_shape};
//
//     using Element = lue::FloatingPointElement<0>;
//
//     auto const current_discharge = lue::create_partitioned_array<Element>(array_shape, partition_shape, 1);
//     auto const inflow = lue::create_partitioned_array<Element>(array_shape, partition_shape, 0);
//
//     Element const alpha = 1.5;
//     Element const beta = 0.6;
//     Element const time_step_duration = 15;
//
//     auto const channel_length = lue::create_partitioned_array<Element>(array_shape, partition_shape, 10);
//
//     auto const new_discharge = lue::value_policies::kinematic_wave(
//         flow_direction, current_discharge, inflow, alpha, beta, time_step_duration, channel_length);
//
//     using namespace lue::value_policies;
//
//     BOOST_CHECK(lue::value_policies::all(new_discharge > Element{0}).future().get());
// }
//
//
// BOOST_AUTO_TEST_CASE(zero_discharge_and_non_zero_inflow)
// {
//     auto flow_direction = lue::test::pcraster_example_flow_direction();
//
//     auto const array_shape{flow_direction.shape()};
//     auto const partition_shape{array_shape};
//
//     using Element = lue::FloatingPointElement<0>;
//
//     auto const current_discharge = lue::create_partitioned_array<Element>(array_shape, partition_shape, 0);
//     auto const inflow = lue::create_partitioned_array<Element>(array_shape, partition_shape, 1);
//
//     Element const alpha = 1.5;
//     Element const beta = 0.6;
//     Element const time_step_duration = 15;
//
//     auto const channel_length = lue::create_partitioned_array<Element>(array_shape, partition_shape, 10);
//
//     auto const new_discharge = lue::value_policies::kinematic_wave(
//         flow_direction, current_discharge, inflow, alpha, beta, time_step_duration, channel_length);
//
//     using namespace lue::value_policies;
//
//     BOOST_CHECK(lue::value_policies::all(new_discharge > Element{0}).future().get());
// }
//
//
// // TODO Add tests for extreme inputs:
// // - What about "wrong" values for alpha, beta, time_step_duration?
//
//
// BOOST_AUTO_TEST_CASE(dry_cell)
// {
//     using FloatingPoint = lue::FloatingPointElement<0>;
//
//     {
//         FloatingPoint const new_discharge{lue::detail::iterate_to_new_discharge<FloatingPoint>(
//             0,     // upstream_discharge
//             0,     // current_discharge
//             0,     // lateral_inflow
//             1.5,   // alpha
//             0.6,   // beta
//             15,    // time_step_duration
//             10)};  // channel_length
//         FloatingPoint const discharge_we_want{0};
//
//         BOOST_TEST(new_discharge == discharge_we_want, tt::tolerance(1e-6));
//     }
//
//     {
//         FloatingPoint const new_discharge{lue::detail::iterate_to_new_discharge<FloatingPoint>(
//             1,     // upstream_discharge
//             0,     // current_discharge
//             -1,    // lateral_inflow
//             1.5,   // alpha
//             0.6,   // beta
//             15,    // time_step_duration
//             10)};  // channel_length
//         FloatingPoint const discharge_we_want{0};
//
//         BOOST_TEST(new_discharge == discharge_we_want, tt::tolerance(1e-6));
//     }
//
//     {
//         FloatingPoint const new_discharge{lue::detail::iterate_to_new_discharge<FloatingPoint>(
//             1,     // upstream_discharge
//             1,     // current_discharge
//             -2,    // lateral_inflow
//             1.5,   // alpha
//             0.6,   // beta
//             15,    // time_step_duration
//             10)};  // channel_length
//         FloatingPoint const discharge_we_want{0};
//
//         BOOST_TEST(new_discharge == discharge_we_want, tt::tolerance(1e-6));
//     }
//
//     {
//         FloatingPoint const new_discharge{lue::detail::iterate_to_new_discharge<FloatingPoint>(
//             0,     // upstream_discharge
//             1,     // current_discharge
//             -1,    // lateral_inflow
//             1.5,   // alpha
//             0.6,   // beta
//             15,    // time_step_duration
//             10)};  // channel_length
//         FloatingPoint const discharge_we_want{0};
//
//         BOOST_TEST(new_discharge == discharge_we_want, tt::tolerance(1e-6));
//     }
//
//     {
//         FloatingPoint const new_discharge{lue::detail::iterate_to_new_discharge<FloatingPoint>(
//             0,     // upstream_discharge
//             0,     // current_discharge
//             -1,    // lateral_inflow
//             1.5,   // alpha
//             0.6,   // beta
//             15,    // time_step_duration
//             10)};  // channel_length
//         FloatingPoint const discharge_we_want{0};
//
//         BOOST_TEST(new_discharge == discharge_we_want, tt::tolerance(1e-6));
//     }
// }
//
//
// // BOOST_AUTO_TEST_CASE(crashed_in_pcraster1)
// // {
// //     double const new_discharge{lue::detail::iterate_to_new_discharge<double>(
// //         0.000201343,   // upstream_discharge
// //         0.000115866,   // current_discharge
// //         -0.000290263,  // lateral_inflow
// //         1.73684,       // alpha
// //         0.6,           // beta
// //         15,            // time_step_duration
// //         10)};          // channel_length
// //     // /* epsilon */ 1E-12);  // epsilon
// //     double const discharge_we_want{0.000031450866300937};
// //
// //     BOOST_TEST(new_discharge == discharge_we_want, tt::tolerance(1e-6));
// // }
//
//
// BOOST_AUTO_TEST_CASE(crashed_in_pcraster2)
// {
//     if constexpr (lue::arithmetic_element_supported<double>)
//     {
//         // TODO Port to all float types
//         using FloatingPoint = double;
//
//         FloatingPoint const new_discharge{lue::detail::iterate_to_new_discharge<FloatingPoint>(
//             0,             // upstream_discharge
//             1.11659e-07,   // current_discharge
//             -1.32678e-05,  // lateral_inflow
//             1.6808,        // alpha
//             0.6,           // beta
//             15,            // time_step_duration
//             10)};          // channel_length
//
//         FloatingPoint const discharge_we_want{std::numeric_limits<FloatingPoint>::min()};
//
//         BOOST_TEST(new_discharge == discharge_we_want, tt::tolerance(1e-6));
//     }
// }
//
//
// BOOST_AUTO_TEST_CASE(random_input)
// {
//     using FloatingPoint = lue::FloatingPointElement<0>;
//
//     std::random_device random_device{};
//     std::default_random_engine random_number_engine(random_device());
//
//     std::uniform_real_distribution<FloatingPoint> discharge_distribution{0, 1000};
//     std::uniform_real_distribution<FloatingPoint> lateral_inflow_distribution{-1000, 1000};
//     std::uniform_real_distribution<FloatingPoint> alpha_distribution{0.5, 6.0};
//     std::uniform_real_distribution<FloatingPoint> beta_distribution{0.5, 2.0};
//     std::uniform_real_distribution<FloatingPoint> time_step_duration_distribution{1, 100};
//     std::uniform_real_distribution<FloatingPoint> channel_length_distribution{1, 100};
//
//     for (std::size_t i = 0; i < 10000; ++i)
//     {
//         FloatingPoint const upstream_discharge{discharge_distribution(random_number_engine)};
//         FloatingPoint const current_discharge{discharge_distribution(random_number_engine)};
//         FloatingPoint const alpha{alpha_distribution(random_number_engine)};
//         FloatingPoint const beta{beta_distribution(random_number_engine)};
//         FloatingPoint const time_step_duration{time_step_duration_distribution(random_number_engine)};
//         FloatingPoint const channel_length{channel_length_distribution(random_number_engine)};
//
//         FloatingPoint const lateral_inflow{
//             lateral_inflow_distribution(random_number_engine) / channel_length};
//
//         FloatingPoint new_discharge{-1};
//
//         BOOST_TEST_INFO(
//             std::format(
//                 "upstream_discharge: {}\n"
//                 "current_discharge: {}\n"
//                 "lateral_inflow: {}\n"
//                 "alpha: {}\n"
//                 "beta: {}\n"
//                 "time_step_duration: {}\n"
//                 "channel_length: {}\n",
//                 upstream_discharge,
//                 current_discharge,
//                 lateral_inflow,
//                 alpha,
//                 beta,
//                 time_step_duration,
//                 channel_length));
//
//         // This call should not throw an exception
//         new_discharge = lue::detail::iterate_to_new_discharge<FloatingPoint>(
//             upstream_discharge,
//             current_discharge,
//             lateral_inflow,
//             alpha,
//             beta,
//             time_step_duration,
//             channel_length);
//
//         BOOST_CHECK(new_discharge >= 0);
//     }
// }
