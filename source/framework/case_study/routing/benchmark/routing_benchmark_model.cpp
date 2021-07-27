#include "routing_benchmark_model.hpp"
#include "lue/framework/algorithm/value_policies/algorithm.hpp"
#include "lue/framework/algorithm/copy.hpp"
#include "lue/framework/io/read_into.hpp"
#include "lue/framework/benchmark/data_model.hpp"


namespace lue::benchmark {

    RoutingBenchmarkModel::RoutingBenchmarkModel(
        Environment const& /* environment */,
        Task const& task,
        std::size_t const max_tree_depth,
        std::string const& array_pathname,
        std::vector<Index> center_cell):

        BenchmarkModel<float, 2>{task, max_tree_depth},
        _array_pathname{array_pathname},
        _object_id{},
        _hyperslab{}

    {
        std::tie(_object_id, _hyperslab) = array_info(array_pathname, center_cell, this->array_shape());
    }


    void RoutingBenchmarkModel::do_preprocess()
    {
        using namespace lue::value_policies;
        auto const& array_shape{this->array_shape()};
        auto const& partition_shape{this->partition_shape()};

        // Time step duration: 1 h → 1 * 60 * 60 s
        time_step_duration = 1 * 60 * 60;

        // Read (subset) from array whose pathname is passed in
        // 1 byte / cell
        flow_direction =
            read<FlowDirectionElement, 2>(_array_pathname, _hyperslab, partition_shape, _object_id);

        // 4 byte / cell
        more_or_less_one = uniform<ScalarElement>(array_shape, partition_shape, 0.9f, 1.1f);

        // Infiltration rate: ~5 mm/h → 5 / 1000 m/s
        // 4 byte / cell
        infiltration_capacity = (time_step_duration * 5.0f * more_or_less_one) / 1000.0f;

        // 4 byte / cell
        auto& infiltration_storage = state();
        infiltration_storage = create_partitioned_array<ScalarElement>(array_shape, partition_shape, 0);

        // We need to wait for all stuff that needs to be ready
        // before the calculations that need to be measured start
        lue::wait_all(flow_direction);
        lue::wait_all(infiltration_capacity);
        lue::wait_all(more_or_less_one);
        lue::wait_all(infiltration_storage);

        // Store initial state:
        // 13 bytes per cell
    }


    void RoutingBenchmarkModel::do_initialize()
    {
        // using namespace lue::value_policies;
        // auto const& array_shape{this->array_shape()};
        // auto const& partition_shape{this->partition_shape()};
    }


    void RoutingBenchmarkModel::do_simulate(
        [[maybe_unused]] Count const time_step)
    {
        using namespace lue::value_policies;
        auto const& array_shape{this->array_shape()};
        auto const& partition_shape{this->partition_shape()};

        // This number determines the proporation of local operations vs the flow accumulation
        std::size_t const nr_iterations = 5;

        auto& infiltration_storage = state();

        // ---------------------------------------------------------------------
        // 'Read' precipitation: ~10 mm/h → 10 / 1000 m/s
        // - 3 * 4 b/c
        // - 3 local operations
        ScalarRaster precipitation =
            (10.0f + uniform<ScalarElement>(array_shape, partition_shape, -3, 3)) / 1000.0f;

        all(precipitation >= 0.0f).then(
            []([[maybe_unused]] auto&& f) { lue_hpx_assert(f.get()); });

        // Simulate the calculation of the actual infiltration capacity
        {
            // Infiltration capacity somehow depends on infiltation storage. Feedback variable.
            // - 2 * 4 b/c
            // - 2 local operations
            infiltration_capacity = infiltration_capacity * (infiltration_storage / infiltration_storage);

            // Add a number of local operations to update the infiltration capacity
            for(std::size_t n = 0; n < nr_iterations; ++n)
            {
                // - nr_iterations * 5 * 4 b/c
                // - nr_iterations * 5 local operations
                infiltration_capacity =
                    (infiltration_capacity * more_or_less_one + more_or_less_one - more_or_less_one) /
                    sqrt(more_or_less_one);
            }
        }

        all(infiltration_capacity >= 0.0f).then(
            []([[maybe_unused]] auto&& f) { lue_hpx_assert(f.get()); });

        // - 2 * 1 b/c (inflow counts)
        // - 2 * 4 b/c
        // - 1 flow direction operation
        auto [runoff, infiltrated_precipitation] =
            accu_threshold3(flow_direction, precipitation, infiltration_capacity);

        // Simulation the calculation of infiltration
        // Add a number of local operations to update the infiltrated precipitation
        for(std::size_t n = 0; n < nr_iterations; ++n)
        {
            // - nr_iterations * 5 * 4 b/c
            // - nr_iterations * 5 local operations
            infiltrated_precipitation =
                (infiltrated_precipitation * more_or_less_one + more_or_less_one - more_or_less_one) /
                sqrt(more_or_less_one);
        }

        all(infiltrated_precipitation >= 0.0f).then(
            []([[maybe_unused]] auto&& f) { lue_hpx_assert(f.get()); });
        all(infiltrated_precipitation >= infiltration_storage).then(
            []([[maybe_unused]] auto&& f) { lue_hpx_assert(f.get()); });

        // Update infiltration storage by adding new infiltrated precipitation and account for
        // losses by subtracting the same amount.
        // Actually, updates state().
        // - 4 b/c
        // - 2 local operations
        infiltration_storage = infiltration_storage + infiltrated_precipitation - infiltrated_precipitation;

        all(infiltration_storage >= 0.0f).then(
            []([[maybe_unused]] auto&& f) { lue_hpx_assert(f.get()); });

        // Per timestep:
        // - 7 local operations + (2 * nr_iterations * 5 local operations)
        //     → 7 + (nr_iterations * 10) local operations
        // - 1 flow direction operation
        //
        // - 8 * 4 b/c + (2 * nr_iterations * 5 * 4 b/c)
        //     → (8 + (nr_iterations * 10)) * 4 b/c
        // - 2 * 1 b/c
        //     → 2 b/c

        // nr_iterations == 5:
        // - 57 local operations + 1 flow direction operation
        // - 58 * 4 + 2 = 234 bytes per cell
    }

}  // namespace lue::benchmark
