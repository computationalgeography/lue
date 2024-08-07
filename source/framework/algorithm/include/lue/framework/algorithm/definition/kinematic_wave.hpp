#pragma once
#include "lue/framework/algorithm/definition/flow_accumulation3.hpp"
#include "lue/framework/algorithm/detail/verify_compatible.hpp"
#include "lue/framework/algorithm/kinematic_wave.hpp"
#include "lue/framework/algorithm/routing_operation_export.hpp"
#include "lue/macro.hpp"
#include <limits>
// #define BOOST_MATH_INSTRUMENT
#include <boost/math/tools/roots.hpp>
#include <fmt/format.h>
#include <cmath>


namespace lue {
    namespace detail {

        template<typename Float>
        class NonLinearKinematicWave
        {

            public:

                NonLinearKinematicWave(
                    Float const upstream_discharge,
                    Float const current_discharge,
                    Float const lateral_inflow,
                    Float const alpha,
                    Float const beta,
                    Float const time_step_duration,
                    Float const channel_length):

                    _upstream_discharge{upstream_discharge},
                    _current_discharge{current_discharge},
                    _lateral_inflow{lateral_inflow},
                    _alpha{alpha},
                    _beta{beta},
                    _alpha_beta{_alpha * _beta},
                    _time_step_duration{time_step_duration}

                {
                    lue_hpx_assert(_upstream_discharge >= Float{0});
                    lue_hpx_assert(_current_discharge >= Float{0});
                    lue_hpx_assert(_upstream_discharge + _current_discharge + _lateral_inflow > Float{0});
                    lue_hpx_assert(_alpha > Float{0});
                    lue_hpx_assert(_beta > Float{0});
                    lue_hpx_assert(_time_step_duration > Float{0});
                    lue_hpx_assert(channel_length > Float{0});

                    // Known terms, independent of new discharge
                    _time_step_duration_over_channel_length = _time_step_duration / channel_length;

                    _known_terms = _time_step_duration_over_channel_length * _upstream_discharge +
                                   _alpha * std::pow(_current_discharge, _beta) +
                                   _time_step_duration * _lateral_inflow;
                }


                /*!
                    @brief      Return a valid initial guess for the new discharge

                    Note that fq is only defined for discharges larger than zero. In case the initial guess
                    ends up being zero or negative, a small positive value is returned.
                */
                auto guess() const -> Float
                {
                    // Small, but not zero!
                    // TODO static Float const min_discharge{1e-30};
                    static Float const min_discharge{std::numeric_limits<Float>::min()};
                    Float discharge_guess{min_discharge};

                    // pow(0, -) is not defined
                    if ((_current_discharge + _upstream_discharge != Float{0}) || _beta >= Float{1})
                    {
                        Float const a_b_pq =
                            _alpha_beta *
                            std::pow((_current_discharge + _upstream_discharge) / Float{2}, _beta - Float{1});

                        lue_hpx_assert(!std::isnan(a_b_pq));

                        discharge_guess =
                            (_time_step_duration_over_channel_length * _upstream_discharge +
                             a_b_pq * _current_discharge + _time_step_duration * _lateral_inflow) /
                            (_time_step_duration_over_channel_length + a_b_pq);

                        lue_hpx_assert(!std::isnan(discharge_guess));

                        discharge_guess = std::max<Float>(discharge_guess, min_discharge);
                    }

                    lue_hpx_assert(discharge_guess > Float{0});

                    return discharge_guess;
                }


                auto operator()(Float const new_discharge) const -> std::pair<Float, Float>
                {
                    return std::make_pair(fq(new_discharge), dfq(new_discharge));
                }


                auto fq(Float const new_discharge) const -> Float
                {
                    lue_hpx_assert(new_discharge > Float{0});  // pow(0, -) is not defined

                    return _time_step_duration_over_channel_length * new_discharge +
                           _alpha * std::pow(new_discharge, _beta) - _known_terms;
                }


                auto dfq(Float const new_discharge) const -> Float
                {
                    lue_hpx_assert(new_discharge > Float{0});  // pow(0, -) is not defined

                    return _time_step_duration_over_channel_length +
                           _alpha_beta * std::pow(new_discharge, _beta - Float{1});
                }


            private:

                //! Updated / new discharge in the upstream cell
                Float _upstream_discharge;

                //! Current / previous discharge in the current cell
                Float _current_discharge;

                //! Lateral inflow
                Float _lateral_inflow;

                Float _alpha;

                //! Momentum coefficient / Boussinesq coefficient [1.01, 1.33] (Chow, p278)
                Float _beta;

                Float _alpha_beta;

                Float _time_step_duration;

                Float _time_step_duration_over_channel_length;

                Float _known_terms;
        };


        template<typename Float>
        Float iterate_to_new_discharge(
            Float const upstream_discharge,  // Summed discharge for cells draining into current cell
            Float const current_discharge,
            Float const lateral_inflow,
            Float const alpha,
            Float const beta,
            Float const time_step_duration,
            Float const channel_length)
        {
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

            Float new_discharge{0};

            if (upstream_discharge + current_discharge > 0 || lateral_inflow > 0)
            {
                // The cell receives water, from upstream and/or from an external source
                Float const inflow = lateral_inflow >= 0 ? lateral_inflow : Float{0};

                NonLinearKinematicWave<Float> kinematic_wave{
                    upstream_discharge,
                    current_discharge,
                    inflow,
                    alpha,
                    beta,
                    time_step_duration,
                    channel_length};

                Float const discharge_guess{kinematic_wave.guess()};

                // These brackets are crucial for obtaining a good performance
                Float const min_discharge{0};
                Float const max_discharge{std::numeric_limits<Float>::max()};
                int const digits = static_cast<int>(std::numeric_limits<Float>::digits * 0.6);

                // In general, 2-3 iterations are enough. In rare cases more are needed. The unit tests don't
                // seem to reach 8, so max 10 should be enough.
                std::uintmax_t const max_nr_iterations{10};
                std::uintmax_t actual_nr_iterations{max_nr_iterations};

                // https://www.boost.org/doc/libs/1_85_0/libs/math/doc/html/math_toolkit/roots_deriv.html
                // std::cout.precision(std::numeric_limits<Float>::digits10);
                new_discharge = boost::math::tools::newton_raphson_iterate(
                    kinematic_wave,
                    discharge_guess,
                    min_discharge,
                    max_discharge,
                    digits,
                    actual_nr_iterations);

                // if (actual_nr_iterations == max_nr_iterations)
                // {
                //     throw std::runtime_error(fmt::format(
                //         "Iterating to a solution took more iterations than expected (initial guess: {}, "
                //         "brackets: [{}, {}])",
                //         discharge_guess,
                //         min_discharge,
                //         max_discharge));
                // }
            }

            if (lateral_inflow < Float{0})
            {
                // Convert units: m³ / m / s → m³ / s
                Float const extraction{std::min(channel_length * std::abs(lateral_inflow), new_discharge)};

                new_discharge -= extraction;
            }

            lue_hpx_assert(new_discharge >= Float{0});

            return new_discharge;
        }


        template<typename Policies, Rank rank>
        class KinematicWaveCellAccumulator
        {

            public:

                using DomainPolicy = policy::DomainPolicyT<Policies>;
                using CurrentDischargeNoDataPolicy =
                    policy::InputNoDataPolicy2T<policy::InputPoliciesT<Policies, 1>>;
                using InflowNoDataPolicy = policy::InputNoDataPolicy2T<policy::InputPoliciesT<Policies, 2>>;
                using ChannelLengthNoDataPolicy =
                    policy::InputNoDataPolicy2T<policy::InputPoliciesT<Policies, 3>>;
                using NewDischargeNoDataPolicy =
                    policy::OutputNoDataPolicy2T<policy::OutputPoliciesT<Policies, 0>>;

                using MaterialElement = policy::ElementT<CurrentDischargeNoDataPolicy>;
                static_assert(std::is_same_v<policy::ElementT<InflowNoDataPolicy>, MaterialElement>);
                static_assert(std::is_same_v<policy::ElementT<ChannelLengthNoDataPolicy>, MaterialElement>);
                static_assert(std::is_same_v<policy::ElementT<NewDischargeNoDataPolicy>, MaterialElement>);

                using MaterialData = DataT<PartitionedArray<MaterialElement, rank>>;

                using Float = MaterialElement;

                static_assert(std::is_floating_point_v<Float>);


                KinematicWaveCellAccumulator(
                    Policies const& policies,
                    MaterialData const& current_discharge,
                    MaterialData const& inflow,
                    Float const alpha,
                    Float const beta,
                    Float const time_step_duration,
                    MaterialData const& channel_length,
                    MaterialData& new_discharge):

                    _dp{policies.domain_policy()},
                    _indp_current_discharge{std::get<1>(policies.inputs_policies()).input_no_data_policy()},
                    _indp_inflow{std::get<2>(policies.inputs_policies()).input_no_data_policy()},
                    _indp_channel_length{std::get<3>(policies.inputs_policies()).input_no_data_policy()},
                    _ondp_new_discharge{std::get<0>(policies.outputs_policies()).output_no_data_policy()},

                    _current_discharge{current_discharge},
                    _inflow{inflow},
                    _alpha{alpha},
                    _beta{beta},
                    _time_step_duration{time_step_duration},
                    _channel_length{channel_length},
                    _new_discharge{new_discharge}

                {
                }


                void accumulate_external_inflow(Index const idx0, Index const idx1)
                {
                    MaterialElement const current_discharge{_current_discharge(idx0, idx1)};
                    MaterialElement const inflow{_inflow(idx0, idx1)};
                    MaterialElement const channel_length{_channel_length(idx0, idx1)};

                    MaterialElement& new_discharge{_new_discharge(idx0, idx1)};

                    if (!_ondp_new_discharge.is_no_data(new_discharge))
                    {
                        if (_indp_current_discharge.is_no_data(current_discharge) ||
                            _indp_inflow.is_no_data(inflow) ||
                            _indp_channel_length.is_no_data(channel_length) ||
                            !_dp.within_domain(current_discharge, inflow, channel_length))
                        {
                            _ondp_new_discharge.mark_no_data(new_discharge);
                        }
                        else
                        {
                            // All information is available now to calculate the new discharge
                            // for the current cell
                            new_discharge = iterate_to_new_discharge(
                                new_discharge,
                                current_discharge,
                                inflow,
                                _alpha,
                                _beta,
                                _time_step_duration,
                                channel_length);
                        }
                    }
                }


                void accumulate_downstream(
                    Index const idx0_from, Index const idx1_from, Index const idx0_to, Index const idx1_to)
                {
                    // The discharge for an upstream cell is ready. Accumulate it in the new discharge
                    // of the downstream cell.
                    MaterialElement const new_discharge_from{_new_discharge(idx0_from, idx1_from)};

                    MaterialElement& new_discharge_to{_new_discharge(idx0_to, idx1_to)};

                    if (!_ondp_new_discharge.is_no_data(new_discharge_to))
                    {
                        if (_ondp_new_discharge.is_no_data(new_discharge_from))
                        {
                            _ondp_new_discharge.mark_no_data(new_discharge_to);
                        }
                        else
                        {
                            lue_hpx_assert(new_discharge_from >= 0);

                            new_discharge_to += new_discharge_from;
                        }
                    }
                }


                void accumulate_downstream(
                    MaterialElement const& new_discharge_from, Index const idx0_to, Index const idx1_to)
                {
                    // The result for the upstream cell is ready
                    MaterialElement& new_discharge_to{_new_discharge(idx0_to, idx1_to)};

                    if (!_ondp_new_discharge.is_no_data(new_discharge_to))
                    {
                        if (_ondp_new_discharge.is_no_data(new_discharge_from))
                        {
                            _ondp_new_discharge.mark_no_data(new_discharge_to);
                        }
                        else
                        {
                            new_discharge_to += new_discharge_from;
                        }
                    }
                }


                MaterialElement const& outflow(Index const idx0, Index const idx1) const
                {
                    return _new_discharge(idx0, idx1);
                }


            private:

                DomainPolicy _dp;

                CurrentDischargeNoDataPolicy _indp_current_discharge;

                InflowNoDataPolicy _indp_inflow;

                ChannelLengthNoDataPolicy _indp_channel_length;

                NewDischargeNoDataPolicy _ondp_new_discharge;

                MaterialData const& _current_discharge;

                MaterialData const& _inflow;

                Float _alpha;

                Float _beta;

                Float _time_step_duration;

                MaterialData const& _channel_length;

                MaterialData& _new_discharge;
        };


        template<typename Policies, typename FlowDirectionElement, typename Element, Rank rank>
        ArrayPartition<Element, rank> kinematic_wave_partition(
            Policies const& policies,
            ArrayPartition<FlowDirectionElement, rank> const& flow_direction_partition,
            ArrayPartition<Element, rank> const& discharge_partition,
            ArrayPartition<Element, rank> const& inflow_partition,
            Element const alpha,
            Element const beta,
            Element const time_step_duration,
            ArrayPartition<Element, rank> const& channel_length_partition,
            InflowCountCommunicator<rank> inflow_count_communicator,
            MaterialCommunicator<Element, rank> discharge_communicator)
        {
            using FlowDirectionPartition = ArrayPartition<FlowDirectionElement, rank>;
            using FlowDirectionData = DataT<FlowDirectionPartition>;

            using Offset = OffsetT<FlowDirectionPartition>;

            using MaterialElement = Element;
            using MaterialPartition = ArrayPartition<MaterialElement, rank>;
            using MaterialData = DataT<MaterialPartition>;

            using LengthElement = Element;
            using LengthPartition = ArrayPartition<LengthElement, rank>;
            using LengthData = DataT<LengthPartition>;

            using CountElement = std::uint8_t;
            using InflowCountPartition = ArrayPartition<CountElement, rank>;
            using InflowCountData = DataT<InflowCountPartition>;

            using CellsIdxs = std::vector<std::array<Index, rank>>;


            auto [inflow_count_partition, input_cells_idxs_f, output_cells_idxs_f] = inflow_count3<Policies>(
                policies, flow_direction_partition, std::move(inflow_count_communicator));


            // Solve intra-partition stream cells
            hpx::future<MaterialData> new_discharge_data_f;
            hpx::future<InflowCountData> inflow_count_data_f;
            {
                // Once the input partitions and inflow_count_data are ready, spawn a task
                // that will calculate the kinematic wave for the intra-partition stream
                // cells. Whenever discharge is known for the border of the partition, it is sent to
                // the corresponding task managing the neighbouring partition.
                hpx::tie(new_discharge_data_f, inflow_count_data_f, output_cells_idxs_f) =
                    hpx::split_future(hpx::dataflow(
                        hpx::launch::async,

                        [policies, alpha, beta, time_step_duration, discharge_communicator](
                            FlowDirectionPartition const& flow_direction_partition,
                            MaterialPartition const& discharge_partition,
                            MaterialPartition const& inflow_partition,
                            LengthPartition const& channel_length_partition,
                            InflowCountPartition const& inflow_count_partition,
                            hpx::future<std::array<CellsIdxs, nr_neighbours<rank>()>>&&
                                output_cells_idxs_f) mutable
                        {
                            AnnotateFunction annotation{"intra_partition_stream_kinematic_wave"};

                            auto const flow_direction_partition_ptr{
                                ready_component_ptr(flow_direction_partition)};
                            FlowDirectionData const& flow_direction_data{
                                flow_direction_partition_ptr->data()};

                            auto const discharge_partition_ptr{ready_component_ptr(discharge_partition)};
                            MaterialData const& current_discharge_data{discharge_partition_ptr->data()};

                            auto const inflow_partition_ptr{ready_component_ptr(inflow_partition)};
                            MaterialData const& inflow_data{inflow_partition_ptr->data()};

                            auto const channel_length_partition_ptr{
                                ready_component_ptr(channel_length_partition)};
                            LengthData const& channel_length_data{channel_length_partition_ptr->data()};

                            auto const inflow_count_partition_ptr{
                                ready_component_ptr(inflow_count_partition)};
                            InflowCountData const& inflow_count_data{inflow_count_partition_ptr->data()};

                            auto const& partition_shape{inflow_count_data.shape()};

                            DataT<MaterialPartition> new_discharge_data{partition_shape, 0};

                            auto const [nr_elements0, nr_elements1] = partition_shape;

                            auto const& indp_flow_direction =
                                std::get<0>(policies.inputs_policies()).input_no_data_policy();
                            auto const& ondp_new_discharge =
                                std::get<0>(policies.outputs_policies()).output_no_data_policy();

                            // We need to copy inflow counts:
                            // - inflow counts are used to select ridge cells
                            // - downstream processing updates inflow counts
                            InflowCountData inflow_count_data_copy{deep_copy(inflow_count_data)};

                            using CellAccumulator = KinematicWaveCellAccumulator<Policies, rank>;
                            using Communicator = MaterialCommunicator<MaterialElement, rank>;

                            CellAccumulator cell_accumulator{
                                policies,
                                current_discharge_data,
                                inflow_data,
                                alpha,
                                beta,
                                time_step_duration,
                                channel_length_data,
                                new_discharge_data};
                            auto output_cells_idxs{output_cells_idxs_f.get()};
                            Accumulator3<CellAccumulator, Communicator> accumulator{
                                std::move(cell_accumulator), discharge_communicator, output_cells_idxs};

                            for (Index idx0 = 0; idx0 < nr_elements0; ++idx0)
                            {
                                for (Index idx1 = 0; idx1 < nr_elements1; ++idx1)
                                {
                                    if (indp_flow_direction.is_no_data(flow_direction_data, idx0, idx1))
                                    {
                                        // Skip cells for which we don't have a flow-direction
                                        ondp_new_discharge.mark_no_data(new_discharge_data, idx0, idx1);
                                    }
                                    else if (inflow_count_data(idx0, idx1) == 0)
                                    {
                                        accumulate3(
                                            accumulator,
                                            idx0,
                                            idx1,
                                            flow_direction_data,
                                            inflow_count_data_copy);
                                    }
                                }
                            }

                            return hpx::make_tuple(
                                std::move(new_discharge_data),
                                std::move(inflow_count_data_copy),
                                std::move(output_cells_idxs));
                        },

                        flow_direction_partition,
                        discharge_partition,
                        inflow_partition,
                        channel_length_partition,
                        inflow_count_partition,
                        std::move(output_cells_idxs_f)));
            }

            // Solve inter-partition stream cells
            {
                // Once the futures of the results of the intra-partition stream calculations
                // have become ready, spawn a task per neighbouring receive channel, each of which
                // must monitor incoming discharge values and perform the kinematic wave calculation
                // once they do. Each of these tasks must stop once all relevant partition input
                // cells (given the direction of the neighbour), have been received.
                new_discharge_data_f = hpx::dataflow(
                    hpx::launch::async,

                    [policies,
                     alpha,
                     beta,
                     time_step_duration,
                     discharge_communicator = std::move(discharge_communicator)](
                        FlowDirectionPartition const& flow_direction_partition,
                        MaterialPartition const& current_discharge_partition,
                        MaterialPartition const& inflow_partition,
                        LengthPartition const& channel_length_partition,
                        hpx::future<InflowCountData>&& inflow_count_data_f,
                        hpx::shared_future<std::array<CellsIdxs, nr_neighbours<rank>()>> const&
                            input_cells_idxs_f,
                        hpx::future<std::array<CellsIdxs, nr_neighbours<rank>()>>&& output_cells_idxs_f,
                        hpx::future<MaterialData>&& new_discharge_data_f) mutable
                    {
                        AnnotateFunction annotation{"inter_partition_stream_kinematic_wave"};

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
                        // It must also synchronize access
                        // to the accumulate call so no two
                        // threads call it at the same time
                        //
                        // Calling the function must
                        // - lock access to the data
                        // - accumulate material
                        // - release the lock

                        auto const flow_direction_partition_ptr{
                            ready_component_ptr(flow_direction_partition)};
                        FlowDirectionData const& flow_direction_data{flow_direction_partition_ptr->data()};
                        auto const& partition_shape{flow_direction_data.shape()};

                        auto const current_discharge_partition_ptr{
                            ready_component_ptr(current_discharge_partition)};
                        MaterialData const& current_discharge_data{current_discharge_partition_ptr->data()};

                        auto const inflow_partition_ptr{ready_component_ptr(inflow_partition)};
                        MaterialData const& inflow_data{inflow_partition_ptr->data()};

                        auto const channel_length_partition_ptr{
                            ready_component_ptr(channel_length_partition)};
                        MaterialData const& channel_length_data{channel_length_partition_ptr->data()};

                        InflowCountData inflow_count_data{inflow_count_data_f.get()};
                        MaterialData new_discharge_data{new_discharge_data_f.get()};

                        using CellAccumulator = KinematicWaveCellAccumulator<Policies, rank>;
                        using Communicator = MaterialCommunicator<MaterialElement, rank>;

                        CellAccumulator cell_accumulator{
                            policies,
                            current_discharge_data,
                            inflow_data,
                            alpha,
                            beta,
                            time_step_duration,
                            channel_length_data,
                            new_discharge_data};
                        auto output_cells_idxs{output_cells_idxs_f.get()};
                        Accumulator3<CellAccumulator, Communicator> accumulator{
                            std::move(cell_accumulator), discharge_communicator, output_cells_idxs};

                        hpx::mutex accu_mutex;

                        auto accumulate =
                            [&accu_mutex, &accumulator, &flow_direction_data, &inflow_count_data](
                                std::array<Index, rank> const& cell_idxs, MaterialElement const value) mutable
                        {
                            auto [idx0, idx1] = cell_idxs;

                            // Prevent multiple threads
                            // from touching this data at the
                            // same time
                            std::scoped_lock lock{accu_mutex};

                            lue_hpx_assert(inflow_count_data(idx0, idx1) >= 1);

                            accumulator.enter_at_partition_input(value, idx0, idx1);

                            --inflow_count_data(idx0, idx1);

                            // Note that multiple streams
                            // from other partitions can join
                            // in a single partition input cell. Only
                            // start an accumulation if this is
                            // the last one.
                            if (inflow_count_data(idx0, idx1) == 0)
                            {
                                detail::accumulate3(
                                    accumulator, idx0, idx1, flow_direction_data, inflow_count_data);
                            }
                        };
                        using Accumulate = decltype(accumulate);

                        std::array<CellsIdxs, nr_neighbours<rank>()> input_cells_idxs{
                            input_cells_idxs_f.get()};

                        std::vector<hpx::future<void>> results{};
                        results.reserve(nr_neighbours<rank>());

                        auto const [extent0, extent1] = partition_shape;

                        {
                            CellsIdxs cells_idxs{input_cells_idxs[accu::Direction::north]};

                            if (!cells_idxs.empty())
                            {
                                RowIdxConverter north_idx_converter{};
                                results.push_back(hpx::async(
                                    monitor_material_inputs<
                                        MaterialElement,
                                        decltype(north_idx_converter),
                                        Accumulate,
                                        rank>,
                                    std::move(cells_idxs),
                                    discharge_communicator.receive_channel(accu::Direction::north),
                                    north_idx_converter,
                                    accumulate));
                            }
                        }

                        {
                            CellsIdxs cells_idxs{input_cells_idxs[accu::Direction::south]};

                            if (!cells_idxs.empty())
                            {
                                RowIdxConverter south_idx_converter{extent0 - 1};
                                results.push_back(hpx::async(
                                    monitor_material_inputs<
                                        MaterialElement,
                                        decltype(south_idx_converter),
                                        Accumulate,
                                        rank>,
                                    std::move(cells_idxs),
                                    discharge_communicator.receive_channel(accu::Direction::south),
                                    south_idx_converter,
                                    accumulate));
                            }
                        }

                        {
                            CellsIdxs cells_idxs{input_cells_idxs[accu::Direction::west]};

                            if (!cells_idxs.empty())
                            {
                                ColIdxConverter west_idx_converter{};
                                results.push_back(hpx::async(
                                    monitor_material_inputs<
                                        MaterialElement,
                                        decltype(west_idx_converter),
                                        Accumulate,
                                        rank>,
                                    std::move(cells_idxs),
                                    discharge_communicator.receive_channel(accu::Direction::west),
                                    west_idx_converter,
                                    accumulate));
                            }
                        }

                        {
                            CellsIdxs cells_idxs{input_cells_idxs[accu::Direction::east]};

                            if (!cells_idxs.empty())
                            {
                                ColIdxConverter east_idx_converter{extent1 - 1};
                                results.push_back(hpx::async(
                                    monitor_material_inputs<
                                        MaterialElement,
                                        decltype(east_idx_converter),
                                        Accumulate,
                                        rank>,
                                    std::move(cells_idxs),
                                    discharge_communicator.receive_channel(accu::Direction::east),
                                    east_idx_converter,
                                    accumulate));
                            }
                        }

                        {
                            CellsIdxs cells_idxs{input_cells_idxs[accu::Direction::north_west]};

                            if (!cells_idxs.empty())
                            {
                                CornerIdxConverter north_west_idx_converter{};
                                results.push_back(hpx::async(
                                    monitor_material_inputs<
                                        MaterialElement,
                                        decltype(north_west_idx_converter),
                                        Accumulate,
                                        rank>,
                                    std::move(cells_idxs),
                                    discharge_communicator.receive_channel(accu::Direction::north_west),
                                    north_west_idx_converter,
                                    accumulate));
                            }
                        }

                        {
                            CellsIdxs cells_idxs{input_cells_idxs[accu::Direction::north_east]};

                            if (!cells_idxs.empty())
                            {
                                CornerIdxConverter north_east_idx_converter{0, extent1 - 1};
                                results.push_back(hpx::async(
                                    monitor_material_inputs<
                                        MaterialElement,
                                        decltype(north_east_idx_converter),
                                        Accumulate,
                                        rank>,
                                    std::move(cells_idxs),
                                    discharge_communicator.receive_channel(accu::Direction::north_east),
                                    north_east_idx_converter,
                                    accumulate));
                            }
                        }

                        {
                            CellsIdxs cells_idxs{input_cells_idxs[accu::Direction::south_east]};

                            if (!cells_idxs.empty())
                            {
                                CornerIdxConverter south_east_idx_converter{extent0 - 1, extent1 - 1};
                                results.push_back(hpx::async(
                                    monitor_material_inputs<
                                        MaterialElement,
                                        decltype(south_east_idx_converter),
                                        Accumulate,
                                        rank>,
                                    std::move(cells_idxs),
                                    discharge_communicator.receive_channel(accu::Direction::south_east),
                                    south_east_idx_converter,
                                    accumulate));
                            }
                        }

                        {
                            CellsIdxs cells_idxs{input_cells_idxs[accu::Direction::south_west]};

                            if (!cells_idxs.empty())
                            {
                                CornerIdxConverter south_west_idx_converter{extent0 - 1, 0};
                                results.push_back(hpx::async(
                                    monitor_material_inputs<
                                        MaterialElement,
                                        decltype(south_west_idx_converter),
                                        Accumulate,
                                        rank>,
                                    std::move(cells_idxs),
                                    discharge_communicator.receive_channel(accu::Direction::south_west),
                                    south_west_idx_converter,
                                    accumulate));
                            }
                        }

                        // As a side effect of this wait,
                        // the partitions whose data is used in
                        // the above monitor_material threads
                        // do not go out of scope
                        hpx::wait_all(results);
                        lue_hpx_assert(all_are_valid(results));
                        lue_hpx_assert(all_are_ready(results));

                        // All output idxs must have been solved by now
                        // TODO Unit test failed this test here once
                        lue_hpx_assert(std::all_of(
                            output_cells_idxs.begin(),
                            output_cells_idxs.end(),
                            [](auto const& idxs) { return idxs.empty(); }));

                        // TODO Assert all inflow counts are zero
                        // TODO Assert all sender channels are closed

                        return new_discharge_data;
                    },


                    flow_direction_partition,
                    discharge_partition,
                    inflow_partition,
                    channel_length_partition,
                    inflow_count_data_f,
                    std::move(input_cells_idxs_f),
                    std::move(output_cells_idxs_f),
                    new_discharge_data_f);
            }

            // Once the futures of the results of the
            // inter-partition stream calculations have become ready,
            // return the result partitions.
            hpx::future<MaterialPartition> new_discharge_partition_f = hpx::dataflow(
                hpx::launch::async,

                [](FlowDirectionPartition const& flow_direction_partition,
                   hpx::future<MaterialData>&& new_discharge_data_f)
                {
                    AnnotateFunction annotation{"create_result_partitions_kinematic_wave"};

                    using Server = typename MaterialPartition::Server;

                    Offset const partition_offset{ready_component_ptr(flow_direction_partition)->offset()};

                    return MaterialPartition{
                        hpx::new_<Server>(hpx::find_here(), partition_offset, new_discharge_data_f.get())};
                },

                flow_direction_partition,
                std::move(new_discharge_data_f));

            return MaterialPartition{std::move(new_discharge_partition_f)};
        }


        template<typename Policies, typename FlowDirectionElement, typename Element, Rank rank>
        struct KinematicWaveAction:
            hpx::actions::make_action<
                decltype(&kinematic_wave_partition<Policies, FlowDirectionElement, Element, rank>),
                &kinematic_wave_partition<Policies, FlowDirectionElement, Element, rank>,
                KinematicWaveAction<Policies, FlowDirectionElement, Element, rank>>::type
        {
        };

    }  // namespace detail


    template<typename Policies, typename FlowDirectionElement, typename Element, Rank rank>
    PartitionedArray<Element, rank> kinematic_wave(
        Policies const& policies,
        PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
        PartitionedArray<Element, rank> const& current_discharge,
        PartitionedArray<Element, rank> const& inflow,
        Element const alpha,
        Element const beta,
        Element const time_step_duration,
        PartitionedArray<Element, rank> const& channel_length)
    {
        using Material = Element;
        using MaterialArray = PartitionedArray<Material, rank>;
        using MaterialPartitions = PartitionsT<MaterialArray>;

        detail::verify_compatible(flow_direction, current_discharge, inflow, channel_length);

        auto const& shape_in_partitions{flow_direction.partitions().shape()};
        Localities<rank> const& localities{flow_direction.localities()};


        // Create communicators used in solving the kinematic wave
        using InflowCountCommunicator = detail::InflowCountCommunicator<rank>;
        using InflowCountCommunicatorArray = detail::CommunicatorArray<InflowCountCommunicator, rank>;
        using MaterialCommunicator = detail::MaterialCommunicator<Material, rank>;
        using MaterialCommunicatorArray = detail::CommunicatorArray<MaterialCommunicator, rank>;

        InflowCountCommunicatorArray inflow_count_communicators{
            "/lue/kinematic_wave/inflow_count/", localities};
        MaterialCommunicatorArray discharge_communicators{"/lue/kinematic_wave/", localities};


        // For each partition, spawn a task that will solve the kinematic wave equation for the partition
        MaterialPartitions new_discharge_partitions{shape_in_partitions};
        detail::KinematicWaveAction<Policies, FlowDirectionElement, Element, rank> action{};
        Count const nr_partitions{nr_elements(shape_in_partitions)};

        for (Index p = 0; p < nr_partitions; ++p)
        {
            new_discharge_partitions[p] = hpx::async(
                action,
                localities[p],
                policies,
                flow_direction.partitions()[p],
                current_discharge.partitions()[p],
                inflow.partitions()[p],
                alpha,
                beta,
                time_step_duration,
                channel_length.partitions()[p],
                inflow_count_communicators[p],
                discharge_communicators[p]);
        }


        // The communicators used by the tasks calculating the kinematic wave result must be kept
        // alive until these results are ready. We do this by attaching a continuation, passing in
        // the communicators. Once the results are ready, these communicators go out of scope. Once
        // they do, free up AGAS resources. Note that component clients are reference-counted.
        hpx::when_all(
            new_discharge_partitions.begin(),
            new_discharge_partitions.end(),
            [inflow_count_communicators = std::move(inflow_count_communicators),
             discharge_communicators =
                 std::move(discharge_communicators)]([[maybe_unused]] auto&& partitions) mutable
            {
                auto f1{inflow_count_communicators.unregister()};
                auto f2{discharge_communicators.unregister()};

                hpx::wait_all(f1, f2);
            });


        return MaterialArray{flow_direction.shape(), localities, std::move(new_discharge_partitions)};
    }

}  // namespace lue


#define LUE_INSTANTIATE_KINEMATIC_WAVE(Policies, FlowDirectionElement, Element)                              \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT PartitionedArray<Element, 2>                                       \
    kinematic_wave<ArgumentType<void(Policies)>, FlowDirectionElement, Element, 2>(                          \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<FlowDirectionElement, 2> const&,                                                    \
        PartitionedArray<Element, 2> const&,                                                                 \
        PartitionedArray<Element, 2> const&,                                                                 \
        Element const,                                                                                       \
        Element const,                                                                                       \
        Element const,                                                                                       \
        PartitionedArray<Element, 2> const&);
