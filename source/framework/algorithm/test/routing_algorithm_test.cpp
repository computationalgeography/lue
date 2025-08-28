#define BOOST_TEST_MODULE lue framework algorithm routing_algorithm

#include "lue/framework/algorithm/definition/flow_accumulation3.hpp"
#include "lue/framework/algorithm/detail/communicator_array.hpp"
#include "lue/framework/algorithm/detail/inflow_count3.hpp"
#include "lue/framework/algorithm/policy.hpp"

// #include "flow_accumulation.hpp"
// #include "lue/framework/algorithm/definition/upstream.hpp"

#include "lue/framework/test/hpx_unit_test.hpp"
// #include "lue/framework.hpp"

#include "lue/framework/partitioned_array.hpp"
#include "lue/concept.hpp"

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

    // TODO: Get rid of all the [[maybe_unused]]

    template<typename Functor>
    using ResultT = typename Functor::Result;

    template<typename Functor>
    using MaterialT = typename Functor::Material;

    // template<typename Functor, typename... Arguments>
    // using ActionT = typename Functor::Action<Arguments...>;

    template<typename Functor>
    using ActionResultT = typename Functor::ActionResult;

    template<typename Functor>
    using IntraPartitionStreamCellsResult = typename Functor::IntraPartitionStreamCellsResult;

    template<typename Functor>
    using InterPartitionStreamCellsResult = typename Functor::InterPartitionStreamCellsResult;


    namespace detail {

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


        template<typename Element, Rank rank>
        class ArgumentTraits<PartitionedArray<Element, rank>>
        {
            public:

                using PassedArgument = PartitionT<PartitionedArray<Element, rank>>;
        };


        template<Arithmetic Element>
        class ArgumentTraits<Element>
        {
            public:

                using PassedArgument = Element;
        };


        template<typename Argument>
        using PassedArgumentT = ArgumentTraits<Argument>::PassedArgument;


        template<typename Element, Rank rank>
        auto pass_argument(PartitionedArray<Element, rank> const& array, Index const partition_idx)
            -> PartitionT<PartitionedArray<Element, rank>>
        {
            return array.partitions()[partition_idx];
        }


        template<typename Element>
        auto pass_argument(Element const value, [[maybe_unused]] Index const partition_idx) -> Element
        {
            return value;
        }


        template<typename Policies, typename Functor, typename... Arguments>
        auto solve_intra_partition_stream_cells(
            [[maybe_unused]] Policies const& policies, [[maybe_unused]] Functor const& functor)
            -> IntraPartitionStreamCellsResult<Functor>
        {
            // auto [material_data_f, inflow_count_data_f, output_cells_idxs_f] =
            //     solve_intra_partition_stream_cells();
            // TODO: Implement, calling functor code
            // hier verder

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

            return IntraPartitionStreamCellsResult<Functor>{};
        }


        template<typename Policies, typename Functor, typename... Arguments>
        auto solve_inter_partition_stream_cells(
            [[maybe_unused]] Policies const& policies, [[maybe_unused]] Functor const& functor)
            -> InterPartitionStreamCellsResult<Functor>
        {
            // material_data_f = solve_inter_partition_stream_cells();
            // TODO: Implement, calling functor code
            // hier verder

            /*
                - Once the flow direction partition, external inflow material partition, threshold partition,
               NIDP partition and collections of partition output cells are ready:
                    - For each cell with an NIDP of zero:
                        - Start the flow accumulation procedure untill a junction cell, sink cell, or
               partition output cell is reached.
                        - If a partition output cell is reached, send location and outflow to the input cell
               in a neighbouring partition using the associated channel in the material communicator. If this
               was the last output cell associated with the neighbouring partition, close the channel.
                    - Immediately return futures to the outflow partition data, residue partition data, and
               the updated NIDP counts.

                - Once the results of the intra-partition stream cell calculations are ready:
                    - Asynchronously create tasks that will each monitor a channel for incoming material sent
               from tasks handling neighbouring partitions. Once material is received:
                        - Start the flow accumulation procedure until a junction cell, sink cell, or partition
               output cell is reached.
                        - If a partition output cell is reached, send location and outflow to the input cell
               in a neighbouring partition using the associated channel in the material communicator.
                        - Stop when all partition input cells have received a value.
            */

            return InterPartitionStreamCellsResult<Functor>{};
        }


        template<typename Policies, typename Functor, typename... Arguments>
        auto accumulate_partition(
            [[maybe_unused]] Policies const& policies,
            [[maybe_unused]] Functor const& functor,
            [[maybe_unused]] ArrayPartition<policy::InputElementT<Policies, 0>, 2> const&
                flow_direction_partition,
            [[maybe_unused]] Arguments const&... arguments,
            [[maybe_unused]] InflowCountCommunicator<2> inflow_count_communicator,
            [[maybe_unused]] MaterialCommunicator<MaterialT<Functor>, 2> material_communicator)
            -> ActionResultT<Functor>
        {
            auto [inflow_count_partition, input_cells_idxs_f, output_cells_idxs_f] = inflow_count3<Policies>(
                policies, flow_direction_partition, std::move(inflow_count_communicator));

            using FlowDirectionElement = policy::InputElementT<Policies, 0>;
            using FlowDirectionPartition = ArrayPartition<FlowDirectionElement, 2>;

            if constexpr (!TupleLike<ResultT<Functor>>)
            {
                // Result is a single partition

                auto [material_data_f, inflow_count_data_f, output_cells_idxs_f] =
                    solve_intra_partition_stream_cells(policies, functor);
                material_data_f = solve_inter_partition_stream_cells(policies, functor);

                // Once the future of the result of the inter-partition stream calculations has become
                // ready, return the result partition
                using MaterialPartition = PartitionT<ResultT<Functor>>;
                using MaterialData = DataT<MaterialPartition>;

                auto material_partition_f = hpx::dataflow(
                    hpx::launch::async,

                    [](FlowDirectionPartition const& flow_direction_partition,
                       hpx::future<MaterialData>&& material_data_f)
                    {
                        using Server = typename MaterialPartition::Server;

                        Offset const partition_offset{
                            ready_component_ptr(flow_direction_partition)->offset()};

                        return MaterialPartition{
                            hpx::new_<Server>(hpx::find_here(), partition_offset, material_data_f.get())};
                    },

                    flow_direction_partition,
                    std::move(material_data_f));

                return MaterialPartition{std::move(material_partition_f)};
            }
            else
            {
                // Result is a tuple of partitions

                auto [outflow_data_f, residue_data_f, inflow_count_data_f, output_cells_idxs_f] =
                    solve_intra_partition_stream_cells(policies, functor);
                std::tie(outflow_data_f, residue_data_f) =
                    hpx::split_future(solve_inter_partition_stream_cells(policies, functor));

                // Once the futures of the results of the inter-partition stream calculations have become
                // ready, return the result partitions
                using OutflowPartition = PartitionT<std::tuple_element_t<0, ResultT<Functor>>>;
                using ResiduePartition = PartitionT<std::tuple_element_t<1, ResultT<Functor>>>;
                using OutflowData = DataT<OutflowPartition>;
                using ResidueData = DataT<ResiduePartition>;

                auto [outflow_partition_f, residue_partition_f] = hpx::split_future(
                    hpx::dataflow(
                        hpx::launch::async,

                        [](FlowDirectionPartition const& flow_direction_partition,
                           hpx::future<OutflowData>&& outflow_data_f,
                           hpx::future<ResidueData>&& residue_data_f)
                        {
                            AnnotateFunction const annotation{
                                "accu_threshold: partition: create_result_partitions"};
                            using OutflowServer = typename OutflowPartition::Server;
                            using ResidueServer = typename ResiduePartition::Server;

                            Offset const partition_offset{
                                ready_component_ptr(flow_direction_partition)->offset()};

                            return hpx::make_tuple(
                                OutflowPartition{hpx::new_<OutflowServer>(
                                    hpx::find_here(), partition_offset, outflow_data_f.get())},
                                ResiduePartition{hpx::new_<ResidueServer>(
                                    hpx::find_here(), partition_offset, residue_data_f.get())});
                        },

                        flow_direction_partition,
                        std::move(outflow_data_f),
                        std::move(residue_data_f)));

                return hpx::make_tuple(
                    OutflowPartition{std::move(outflow_partition_f)},
                    ResiduePartition{std::move(residue_partition_f)});
            }
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


    template<typename Policies, typename Functor, typename... Arguments>
    auto accumulating_router(
        [[maybe_unused]] Policies const& policies,
        [[maybe_unused]] Functor const& functor,
        [[maybe_unused]] PartitionedArray<policy::InputElementT<Policies, 0>, 2> const& flow_direction,
        [[maybe_unused]] Arguments const&... arguments) -> ResultT<Functor>
    {
        Rank const rank{2};

        Localities<rank> localities{flow_direction.localities()};

        auto [inflow_count_communicators, material_communicators] =
            detail::create_communicators<Functor>(localities);

        auto const& shape_in_partitions{flow_direction.partitions().shape()};
        Count const nr_partitions{nr_elements(shape_in_partitions)};

        using Action = detail::AccumulateAction<Policies, Functor, detail::PassedArgumentT<Arguments>...>;
        Action action{};

        if constexpr (!TupleLike<ResultT<Functor>>)
        {
            // Result is a single partitioned array

            using Array = ResultT<Functor>;
            using Partitions = PartitionsT<Array>;

            Partitions result_partitions{shape_in_partitions};

            for (Index partition_idx = 0; partition_idx < nr_partitions; ++partition_idx)
            {
                result_partitions[partition_idx] = hpx::async(
                    action,
                    localities[partition_idx],
                    policies,
                    functor,
                    flow_direction.partitions()[partition_idx],
                    detail::pass_argument(arguments, partition_idx)...,
                    inflow_count_communicators[partition_idx],
                    material_communicators[partition_idx]);
            }

            detail::keep_communicators_alive(
                result_partitions, std::move(inflow_count_communicators), std::move(material_communicators));

            return Array{flow_direction.shape(), std::move(localities), std::move(result_partitions)};
        }
        else
        {
            // Result is a tuple of partitioned arrays

            using Arrays = ResultT<Functor>;
            // TODO: Generalize
            static_assert(std::tuple_size_v<Arrays> == 2);

            using Array0 = std::tuple_element_t<0, Arrays>;
            using Array1 = std::tuple_element_t<1, Arrays>;
            using Partitions0 = PartitionsT<Array0>;
            using Partitions1 = PartitionsT<Array1>;
            using Partitions = std::tuple<Partitions0, Partitions1>;

            Partitions result_partitions{};

            std::get<0>(result_partitions) = Partitions0{shape_in_partitions};
            std::get<1>(result_partitions) = Partitions1{shape_in_partitions};

            for (Index partition_idx = 0; partition_idx < nr_partitions; ++partition_idx)
            {
                std::tie(
                    std::get<0>(result_partitions)[partition_idx],
                    std::get<1>(result_partitions)[partition_idx]) =
                    hpx::split_future(
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
                std::get<0>(result_partitions),
                std::move(inflow_count_communicators),
                std::move(material_communicators));

            // TODO: One array can grab the localities
            return std::make_tuple(
                Array0{
                    flow_direction.shape(),
                    Localities<rank>{localities},
                    std::move(std::get<0>(result_partitions))},
                Array1{
                    flow_direction.shape(),
                    Localities<rank>{localities},
                    std::move(std::get<1>(result_partitions))});
        }
    }


    template<typename Policies>
    class AccumulatingRouterFunctor
    {
        public:

            using CountElement = SmallestIntegralElement;
            using InflowCountPartition = ArrayPartition<CountElement, 2>;
            using InflowCountData = DataT<InflowCountPartition>;
            using CellsIdxs = std::vector<std::array<Index, 2>>;
    };


    template<typename Policies>
    class AccuInfo: public AccumulatingRouterFunctor<Policies>
    {
        public:

            using typename AccumulatingRouterFunctor<Policies>::InflowCountData;
            using typename AccumulatingRouterFunctor<Policies>::CellsIdxs;

            static constexpr char const* name{"accu_info"};

            using Material = policy::OutputElementT<Policies, 0>;

            using Result = PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;

            using ActionResult = PartitionT<Result>;

            using MaterialPartition = ArrayPartition<Material, 2>;
            using MaterialData = DataT<MaterialPartition>;

            using IntraPartitionStreamCellsResult = std::tuple<
                hpx::future<MaterialData>,
                hpx::future<InflowCountData>,
                hpx::future<std::array<CellsIdxs, detail::nr_neighbours<2>()>>>;

            using InterPartitionStreamCellsResult = hpx::future<MaterialData>;
    };


    template<typename Policies>
    class AccuFlux: public AccumulatingRouterFunctor<Policies>
    {
        public:

            using typename AccumulatingRouterFunctor<Policies>::InflowCountData;
            using typename AccumulatingRouterFunctor<Policies>::CellsIdxs;

            static constexpr char const* name{"accu_flux"};

            using Material = policy::InputElementT<Policies, 1>;

            using Result = PartitionedArray<policy::OutputElementT<Policies, 0>, 2>;

            using ActionResult = PartitionT<Result>;

            using MaterialPartition = ArrayPartition<Material, 2>;
            using MaterialData = DataT<MaterialPartition>;

            using IntraPartitionStreamCellsResult = std::tuple<
                hpx::future<MaterialData>,
                hpx::future<InflowCountData>,
                hpx::future<std::array<CellsIdxs, detail::nr_neighbours<2>()>>>;

            using InterPartitionStreamCellsResult = hpx::future<MaterialData>;
    };


    template<typename Policies>
    class AccuThreshold: public AccumulatingRouterFunctor<Policies>
    {
        public:

            using typename AccumulatingRouterFunctor<Policies>::InflowCountData;
            using typename AccumulatingRouterFunctor<Policies>::CellsIdxs;

            static constexpr char const* name{"accu_threshold"};

            using Material = policy::InputElementT<Policies, 1>;

            using Result = std::tuple<
                PartitionedArray<policy::OutputElementT<Policies, 0>, 2>,
                PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>;

            using ActionResult = std::tuple<
                PartitionT<PartitionedArray<policy::OutputElementT<Policies, 0>, 2>>,
                PartitionT<PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>>;

            using MaterialPartition = ArrayPartition<Material, 2>;
            using MaterialData = DataT<MaterialPartition>;

            using IntraPartitionStreamCellsResult = std::tuple<
                hpx::future<MaterialData>,
                hpx::future<MaterialData>,
                hpx::future<InflowCountData>,
                hpx::future<std::array<CellsIdxs, detail::nr_neighbours<2>()>>>;

            using InterPartitionStreamCellsResult = hpx::future<std::tuple<MaterialData, MaterialData>>;
    };


    template<typename Policies>
    class AccuFraction: public AccumulatingRouterFunctor<Policies>
    {
        public:

            using typename AccumulatingRouterFunctor<Policies>::InflowCountData;
            using typename AccumulatingRouterFunctor<Policies>::CellsIdxs;

            static constexpr char const* name{"accu_fraction"};

            using Material = policy::InputElementT<Policies, 1>;

            using Result = std::tuple<
                PartitionedArray<policy::OutputElementT<Policies, 0>, 2>,
                PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>;

            using ActionResult = std::tuple<
                PartitionT<PartitionedArray<policy::OutputElementT<Policies, 0>, 2>>,
                PartitionT<PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>>;

            using MaterialPartition = ArrayPartition<Material, 2>;
            using MaterialData = DataT<MaterialPartition>;

            using IntraPartitionStreamCellsResult = std::tuple<
                hpx::future<MaterialData>,
                hpx::future<MaterialData>,
                hpx::future<InflowCountData>,
                hpx::future<std::array<CellsIdxs, detail::nr_neighbours<2>()>>>;

            using InterPartitionStreamCellsResult = hpx::future<std::tuple<MaterialData, MaterialData>>;
    };


    template<typename Policies>
    class AccuCapacity: public AccumulatingRouterFunctor<Policies>
    {
        public:

            using typename AccumulatingRouterFunctor<Policies>::InflowCountData;
            using typename AccumulatingRouterFunctor<Policies>::CellsIdxs;

            static constexpr char const* name{"accu_capacity"};

            using Material = policy::InputElementT<Policies, 1>;

            using Result = std::tuple<
                PartitionedArray<policy::OutputElementT<Policies, 0>, 2>,
                PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>;

            using ActionResult = std::tuple<
                PartitionT<PartitionedArray<policy::OutputElementT<Policies, 0>, 2>>,
                PartitionT<PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>>;

            using MaterialPartition = ArrayPartition<Material, 2>;
            using MaterialData = DataT<MaterialPartition>;

            using IntraPartitionStreamCellsResult = std::tuple<
                hpx::future<MaterialData>,
                hpx::future<MaterialData>,
                hpx::future<InflowCountData>,
                hpx::future<std::array<CellsIdxs, detail::nr_neighbours<2>()>>>;

            using InterPartitionStreamCellsResult = hpx::future<std::tuple<MaterialData, MaterialData>>;
    };


    template<typename Policies>
    class AccuTrigger: public AccumulatingRouterFunctor<Policies>
    {
        public:

            using typename AccumulatingRouterFunctor<Policies>::InflowCountData;
            using typename AccumulatingRouterFunctor<Policies>::CellsIdxs;

            static constexpr char const* name{"accu_trigger"};

            using Material = policy::InputElementT<Policies, 1>;

            using Result = std::tuple<
                PartitionedArray<policy::OutputElementT<Policies, 0>, 2>,
                PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>;

            using ActionResult = std::tuple<
                PartitionT<PartitionedArray<policy::OutputElementT<Policies, 0>, 2>>,
                PartitionT<PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>>;

            using MaterialPartition = ArrayPartition<Material, 2>;
            using MaterialData = DataT<MaterialPartition>;

            using IntraPartitionStreamCellsResult = std::tuple<
                hpx::future<MaterialData>,
                hpx::future<MaterialData>,
                hpx::future<InflowCountData>,
                hpx::future<std::array<CellsIdxs, detail::nr_neighbours<2>()>>>;

            using InterPartitionStreamCellsResult = hpx::future<std::tuple<MaterialData, MaterialData>>;
    };


    template<typename Policies>
    class KinematicWave: public AccumulatingRouterFunctor<Policies>
    {
        public:

            using typename AccumulatingRouterFunctor<Policies>::InflowCountData;
            using typename AccumulatingRouterFunctor<Policies>::CellsIdxs;

            static constexpr char const* name{"kinematic_wave"};

            using Material = policy::InputElementT<Policies, 1>;

            using Result = std::tuple<
                PartitionedArray<policy::OutputElementT<Policies, 0>, 2>,
                PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>;

            using ActionResult = std::tuple<
                PartitionT<PartitionedArray<policy::OutputElementT<Policies, 0>, 2>>,
                PartitionT<PartitionedArray<policy::OutputElementT<Policies, 1>, 2>>>;

            using MaterialPartition = ArrayPartition<Material, 2>;
            using MaterialData = DataT<MaterialPartition>;

            using IntraPartitionStreamCellsResult = std::tuple<
                hpx::future<MaterialData>,
                hpx::future<MaterialData>,
                hpx::future<InflowCountData>,
                hpx::future<std::array<CellsIdxs, detail::nr_neighbours<2>()>>>;

            using InterPartitionStreamCellsResult = hpx::future<std::tuple<MaterialData, MaterialData>>;
    };


    namespace policy::accu_info {

        // TODO: Check policies
        template<std::integral FlowDirection, std::integral Class>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<FlowDirection>,
            OutputElements<Class>,
            InputElements<FlowDirection>>;

    }  // namespace policy::accu_info


    namespace policy::accu_flux {

        // TODO: Check policies
        template<std::integral FlowDirection, std::floating_point Material>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<FlowDirection, Material>,
            OutputElements<Material>,
            InputElements<FlowDirection, Material>>;

    }  // namespace policy::accu_flux


    namespace policy::accu_threshold {

        // TODO: Check policies
        template<std::integral FlowDirection, std::floating_point Material>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<FlowDirection, Material, Material>,
            OutputElements<Material, Material>,
            InputElements<FlowDirection, Material, Material>>;

    }  // namespace policy::accu_threshold


    namespace policy::accu_fraction {

        // TODO: Check policies
        template<std::integral FlowDirection, std::floating_point FloatingPointElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<FlowDirection, FloatingPointElement, FloatingPointElement>,
            OutputElements<FloatingPointElement, FloatingPointElement>,
            InputElements<FlowDirection, FloatingPointElement, FloatingPointElement>>;

    }  // namespace policy::accu_fraction


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


    namespace policy::kinematic_wave {

        // TODO: Check policies
        template<
            std::integral FlowDirection,
            std::floating_point FloatingPointElement,
            std::unsigned_integral CountElement>
        using DefaultValuePolicies = policy::DefaultValuePolicies<
            AllValuesWithinDomain<
                FlowDirection,
                FloatingPointElement,
                FloatingPointElement,
                FloatingPointElement,
                FloatingPointElement,
                CountElement,
                CountElement,
                FloatingPointElement>,
            OutputElements<FloatingPointElement, FloatingPointElement>,
            InputElements<
                FlowDirection,
                FloatingPointElement,
                FloatingPointElement,
                FloatingPointElement,
                FloatingPointElement,
                CountElement,
                CountElement,
                FloatingPointElement>>;

    }  // namespace policy::kinematic_wave

}  // namespace lue


BOOST_AUTO_TEST_CASE(accu_info)
{
    using FlowDirectionElement = std::uint8_t;
    using ClassElement = std::int8_t;
    using Policies = lue::policy::accu_info::DefaultValuePolicies<FlowDirectionElement, ClassElement>;
    using Functor = lue::AccuInfo<Policies>;

    lue::PartitionedArray<FlowDirectionElement, 2> const flow_direction{};

    [[maybe_unused]] lue::PartitionedArray<ClassElement, 2> class_ =
        accumulating_router(Policies{}, Functor{}, flow_direction);
}


BOOST_AUTO_TEST_CASE(accu_flux)
{
    using FlowDirectionElement = std::uint8_t;
    using MaterialElement = float;
    using Policies = lue::policy::accu_flux::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;
    using Functor = lue::AccuFlux<Policies>;

    lue::PartitionedArray<FlowDirectionElement, 2> const flow_direction{};
    [[maybe_unused]] lue::PartitionedArray<MaterialElement, 2> outflow{};

    {
        lue::PartitionedArray<MaterialElement, 2> const material{};

        outflow = accumulating_router(Policies{}, Functor{}, flow_direction, material);
    }

    {
        MaterialElement const material{};

        outflow = accumulating_router(Policies{}, Functor{}, flow_direction, material);
    }
}


BOOST_AUTO_TEST_CASE(accu_threshold)
{
    using FlowDirectionElement = std::uint8_t;
    using MaterialElement = float;
    using Policies = lue::policy::accu_threshold::DefaultValuePolicies<FlowDirectionElement, MaterialElement>;
    using Functor = lue::AccuThreshold<Policies>;

    lue::PartitionedArray<FlowDirectionElement, 2> const flow_direction{};
    [[maybe_unused]] lue::PartitionedArray<MaterialElement, 2> outflow{};
    [[maybe_unused]] lue::PartitionedArray<MaterialElement, 2> residue{};

    {
        lue::PartitionedArray<MaterialElement, 2> const material{};
        lue::PartitionedArray<MaterialElement, 2> const threshold{};

        std::tie(outflow, residue) =
            accumulating_router(Policies{}, Functor{}, flow_direction, material, threshold);
    }

    {
        MaterialElement const material{};
        MaterialElement const threshold{};

        std::tie(outflow, residue) =
            accumulating_router(Policies{}, Functor{}, flow_direction, material, threshold);
    }
}


BOOST_AUTO_TEST_CASE(accu_fraction)
{
    using FlowDirectionElement = std::uint8_t;
    using FloatingPointElement = float;
    using Policies =
        lue::policy::accu_fraction::DefaultValuePolicies<FlowDirectionElement, FloatingPointElement>;
    using Functor = lue::AccuFraction<Policies>;

    lue::PartitionedArray<FlowDirectionElement, 2> const flow_direction{};
    [[maybe_unused]] lue::PartitionedArray<FloatingPointElement, 2> outflow{};
    [[maybe_unused]] lue::PartitionedArray<FloatingPointElement, 2> residue{};

    {
        lue::PartitionedArray<FloatingPointElement, 2> const material{};
        lue::PartitionedArray<FloatingPointElement, 2> const fraction{};

        std::tie(outflow, residue) =
            accumulating_router(Policies{}, Functor{}, flow_direction, material, fraction);
    }

    {
        FloatingPointElement const material{};
        FloatingPointElement const fraction{};

        std::tie(outflow, residue) =
            accumulating_router(Policies{}, Functor{}, flow_direction, material, fraction);
    }
}


BOOST_AUTO_TEST_CASE(accu_capacity)
{
    using FlowDirectionElement = std::uint8_t;
    using FloatingPointElement = float;
    using Policies =
        lue::policy::accu_capacity::DefaultValuePolicies<FlowDirectionElement, FloatingPointElement>;
    using Functor = lue::AccuCapacity<Policies>;

    lue::PartitionedArray<FlowDirectionElement, 2> const flow_direction{};
    [[maybe_unused]] lue::PartitionedArray<FloatingPointElement, 2> outflow{};
    [[maybe_unused]] lue::PartitionedArray<FloatingPointElement, 2> residue{};

    {
        lue::PartitionedArray<FloatingPointElement, 2> const material{};
        lue::PartitionedArray<FloatingPointElement, 2> const capacity{};

        std::tie(outflow, residue) =
            accumulating_router(Policies{}, Functor{}, flow_direction, material, capacity);
    }

    {
        FloatingPointElement const material{};
        FloatingPointElement const capacity{};

        std::tie(outflow, residue) =
            accumulating_router(Policies{}, Functor{}, flow_direction, material, capacity);
    }
}


BOOST_AUTO_TEST_CASE(accu_trigger)
{
    using FlowDirectionElement = std::uint8_t;
    using FloatingPointElement = float;
    using Policies =
        lue::policy::accu_trigger::DefaultValuePolicies<FlowDirectionElement, FloatingPointElement>;
    using Functor = lue::AccuTrigger<Policies>;

    lue::PartitionedArray<FlowDirectionElement, 2> const flow_direction{};
    [[maybe_unused]] lue::PartitionedArray<FloatingPointElement, 2> outflow{};
    [[maybe_unused]] lue::PartitionedArray<FloatingPointElement, 2> residue{};

    {
        lue::PartitionedArray<FloatingPointElement, 2> const material{};
        lue::PartitionedArray<FloatingPointElement, 2> const trigger{};

        std::tie(outflow, residue) =
            accumulating_router(Policies{}, Functor{}, flow_direction, material, trigger);
    }

    {
        FloatingPointElement const material{};
        FloatingPointElement const trigger{};

        std::tie(outflow, residue) =
            accumulating_router(Policies{}, Functor{}, flow_direction, material, trigger);
    }
}


BOOST_AUTO_TEST_CASE(kinematic_wave)
{
    using FlowDirectionElement = std::uint8_t;
    using FloatingPointElement = float;
    using CountElement = std::uint32_t;
    using Policies = lue::policy::kinematic_wave::
        DefaultValuePolicies<FlowDirectionElement, FloatingPointElement, CountElement>;
    using Functor = lue::KinematicWave<Policies>;

    lue::PartitionedArray<FlowDirectionElement, 2> const flow_direction{};
    lue::PartitionedArray<FloatingPointElement, 2> const current_state{};
    lue::PartitionedArray<FloatingPointElement, 2> const lateral_inflow{};
    CountElement const nr_time_slices{};
    CountElement const time_step_duration{};

    [[maybe_unused]] lue::PartitionedArray<FloatingPointElement, 2> outflow{};
    [[maybe_unused]] lue::PartitionedArray<FloatingPointElement, 2> residue{};

    {
        lue::PartitionedArray<FloatingPointElement, 2> const alpha{};
        lue::PartitionedArray<FloatingPointElement, 2> const beta{};
        lue::PartitionedArray<FloatingPointElement, 2> const channel_length{};

        std::tie(outflow, residue) = accumulating_router(
            Policies{},
            Functor{},
            flow_direction,
            current_state,
            lateral_inflow,
            alpha,
            beta,
            nr_time_slices,
            time_step_duration,
            channel_length);
    }

    {
        FloatingPointElement const alpha{};
        FloatingPointElement const beta{};
        FloatingPointElement const channel_length{};

        std::tie(outflow, residue) = accumulating_router(
            Policies{},
            Functor{},
            flow_direction,
            current_state,
            lateral_inflow,
            alpha,
            beta,
            nr_time_slices,
            time_step_duration,
            channel_length);
    }
}
