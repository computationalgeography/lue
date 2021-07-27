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


        /// // Read (subset) from array whose pathname is passed in
        /// // 1 byte / cell
        /// flow_direction =
        ///     read<FlowDirectionElement, 2>(_array_pathname, _hyperslab, partition_shape, _object_id);

        /// // We need to wait for all stuff that needs to be ready
        /// // before the calculations that need to be measured start
        /// lue::wait_all(flow_direction);

        /// // state() is waited for automatically

        /// // Initial state:
        /// // - Memory usage: 1 * 1 bytes / cell
        /// // - local operation: 0
        /// // - flow direction operations: 0


    void RoutingBenchmarkModel::do_initialize()
    {
        using namespace lue::value_policies;
        auto const& array_shape{this->array_shape()};
        auto const& partition_shape{this->partition_shape()};

    }

        /// // Interception rate: ~2 mm/h → 2 / 1000 m/s
        /// // - 3 * 4 b/c
        /// // - 3 local operations
        /// interception_rate = (2.0f + uniform<ScalarElement>(array_shape, partition_shape, -0.5, 0.5)) / 1000.0f;

        /// // How much can be potentially intercepted
        /// // - 1 * 4 b/c
        /// // - 1 local operation
        /// interception_capacity = time_step_duration * interception_rate;

        /// // How much is actually intercepted
        /// // - 1 * 4 b/c
        /// // - 1 local operation
        /// interception_storage = create_partitioned_array<ScalarElement>(array_shape, partition_shape, 0);

        /// // Infiltration rate: ~5 mm/h → 5 / 1000 m/s
        /// // - 3 * 4 b/c
        /// // - 3 local operation
        /// infiltration_rate = (5.0f + uniform<ScalarElement>(array_shape, partition_shape, -2, 2)) / 1000.0f;

        /// // How much can potentially infiltrate
        /// // - 1 * 4 b/c
        /// // - 1 local operation
        /// infiltration_capacity = time_step_duration * infiltration_rate;

        /// // - 1 * 4 b/c
        /// // - 1 local operation
        /// auto& infiltration_storage = state();
        /// infiltration_storage = create_partitioned_array<ScalarElement>(array_shape, partition_shape, 0);

        /// // Initial state:
        /// // - Memory usage: 10 * 4 bytes / cell
        /// // - local operation: 10
        /// // - flow direction operations: 0


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






        /// // ---------------------------------------------------------------------
        /// // Update state of interception component
        /// // How much water can be stored in the interception storage,
        /// // given that there might already be water present
        /// // - 1 * 4 b/c
        /// // - 1 local operation
        /// ScalarRaster actual_interception_capacity = interception_capacity - interception_storage;

        /// // TODO add more operations to update actual_interception_capacity

        /// // Fill interception storage with as much water as possible
        /// // - 1 * 1 b/c
        /// // - 1 * 4 b/c
        /// // - 2 local operations
        /// ScalarRaster intercepted_precipitation =
        ///     where(actual_interception_capacity < precipitation, actual_interception_capacity, precipitation);

        /// // - 1 * 4 b/c
        /// // - 1 local operation
        /// interception_storage = interception_storage + intercepted_precipitation;
        /// // - 1 * 4 b/c
        /// // - 1 local operation
        /// precipitation = precipitation - intercepted_precipitation;

        /// all(interception_storage >= 0.0f).then(
        ///     []([[maybe_unused]] auto&& f) { lue_hpx_assert(f.get()); });
        /// all(interception_storage < interception_capacity).then(
        ///     []([[maybe_unused]] auto&& f) { lue_hpx_assert(f.get()); });


        /// // ---------------------------------------------------------------------
        /// // Update state of infiltration component
        /// // - 1 * 4 b/c
        /// // - 1 local operation
        /// auto& infiltration_storage = state();
        /// ScalarRaster actual_infiltration_capacity = infiltration_capacity - infiltration_storage;

        /// // TODO add more operations to update actual_infiltration_capacity
        /// // - evapotranspiration, forest floor interception, ...

        /// // Redistribute the water that reaches the soil laterally,
        /// // taking the infiltration capacity into account.

        /// // - 2 * 1 b/c (inflow counts)
        /// // - 2 * 4 b/c
        /// // - 1 flow direction operation
        /// auto [runoff, infiltrated_precipitation] =
        ///     accu_threshold3(flow_direction, precipitation, actual_infiltration_capacity);

        /// // - 1 * 4 b/c
        /// // - 1 local operation
        /// infiltration_storage = infiltration_storage + infiltrated_precipitation;

        /// // TODO add more operations to update infiltration_storage
        /// // - seapage, ...

        /// all(infiltration_storage >= 0.0f).then(
        ///     []([[maybe_unused]] auto&& f) { lue_hpx_assert(f.get()); });
        /// all(infiltration_storage < infiltration_capacity).then(
        ///     []([[maybe_unused]] auto&& f) { lue_hpx_assert(f.get()); });


        /// // Per time step:
        /// // - memory usage: 2 * 1 byte / cell + 11 * 4 bytes / cell
        /// // - local operation: 10
        /// // - flow direction operations: 1


        /// // Overall:
        /// // initial:
        /// // - Memory usage: 1 bytes / cell + 40 bytes / cell -> 41 bytes / cell
        /// // - local operation: 10
        /// // simulate:
        /// // - memory usage: 2 byte / cell + 44 bytes / cell → 46 bytes / cell
        /// // - local operation: 10
        /// // - flow direction operations: 1


        /// // Memory usage (lower limit):
        /// // nr_rows * nr_cols * (41 + nr_time_steps_in_flight * 46)

        /// // Move to some doc in paper repo:
        /// // python $LUE/../paper_2021_routing/source/benchmark/array_shapes.py eejit 41 46 5







        /// feedback, maar range moet gelijk blijven
        /// 
        /// potentiele_infiltratie = subsurface_water(actuele_infiltratie)
        /// 
        /// resultaat altijd ongeveer 5, maar wel afhankelijk van actuele infi
        /// 
        /// Nog iets anders doen met de precipitation of wat dan ook ernaast doen.





        /// // ---------------------------------------------------------------------
        /// // Given a certain amount of precipitation, this model calculates
        /// // the amount of water that is stored in the soil at each
        /// // time step (== state).

        /// // External input of water. This changes each time step.
        /// ScalarRaster precipitation = uniform<ScalarElement>(array_shape, partition_shape, 0, 100);

        /// // -------------------
        /// // Calculate the amount of precipitation that reaches the top
        /// // of the soil. Some of the precipitation is intercepted by
        /// // plants and the forest floor.
        /// // -------------------

        /// // The amount of precipitation intercepted by the canopy is a
        /// // fraction, dependent on the vegetation kind, subtracted
        /// // by the amount intercepted during the previous time step.
        /// canopy_interception = (fraction_canopy_interception * precipitation) - canopy_interception;  // feedback

        /// // Some of this intercepted water is transpired into the air.
        /// // transpiration = fraction_transpiration * canopy_interception
        /// // canopy_interception -= transpiration

        /// // Some of the remaining intercepted water leaves the cannopy
        /// // as stem_flow or throughfall.
        /// // stem_flow = fraction_stemflow * canopy_interception
        /// // throughfall = fraction_throughfall * canopy_interception
        /// // canopy_interception -= stem_flow + throughfall

        /// // precipitation -= canopy_interception

        /// // Something similar for the forest floor.
        /// // forest_floor_interception = (fraction_forest_floor_interception * precipitation) - forest_floor_interception // feedback
        /// // evaporation = fraction_evaporation * forest_floor_interception
        /// // forest_floor_interception -= evaporation

        /// // precipitation -= forest_floor_interception


        /// // -------------------
        /// // Redistributed the water reaching the soil laterally, based on
        /// // the flow direction field and the infiltration capacity of
        /// // the soil.
        /// // -------------------

        /// // Amount of water for which there is space in the soil. This
        /// // depends on pore space and the amount of it already occupied
        /// // by water from the previous time step.
        /// // porosity = ... // initial
        /// // pore_space = ... // initial
        /// // dry_height = pore_space - wet_height;  // feedback

        /// // Given the duration of a time step, the infiltration capacity
        /// // determines how much of the dry_height can be filled.
        /// // dry_height = min(dry_height, infiltration_capacity * time_step_duration)

        /// // Redistribute the water reaching the soil laterally, based on
        /// // the flow direction field and the capacity of the soil to take
        /// // up more water.
        /// // auto [runoff, infiltration] = op::accu_threshold3(flow_direction, precipitation, dry_height);

        /// // Update the amount of water in the soil by the amount actually
        /// // infiltrated. The rest runs off as runoff and eventually
        /// // stream flow.
        /// // wet_height += infiltration

        /// // Update the amount of water in the soil by the amount
        /// // percolating to the ground water.
        /// // wet_height -= fraction_percolation * wet_height;

        /// // state() = std::move(wet_height);






        // // // Infiltration capacity in dry conditions. TODO Move to initialize()
        // // ScalarRaster infiltration_capacity = op::uniform<ScalarElement>(array_shape, partition_shape, 0, 50);

        // /// // Actual infiltration capacity, based on current conditions.
        // /// // TODO
        // /// // This is a rate!
        // /// // ScalarRaster actual_infiltration_capacity = op::subtract(infiltration_capacity, state());

        // // Amount of water for which there is space in the soil. This
        // // depends on porosity and amount of water already in the soil.
        // // porosity = ... // initial
        // // height = ... // initial
        // // dry_height = height - wet_height;

        // // Redistribute the inflow of water based on the flow direction
        // // field and the potential for infiltration
        // auto [runoff, infiltration] = op::accu_threshold3(flow_direction, inflow, dry_height);


        // // -------------------
        // // Update upper soil layer's hydrological state
        // // -------------------

        // // Given the inflow of water infiltrating from above, move water
        // // to lower layers, out of the upper soil layer.

        // // Use infiltration_capacity (rate) again?

        // // wet_height += infiltration
        // // wet_height -= ... TODO use infiltration capacity to move some of the water away

        /// state() = std::move(wet_height);


        // ---------------------------------------------------------------------

        // output idle time


    void RoutingBenchmarkModel::do_terminate()
    {
        // Wait for the results of the model to be available. Only then
        // the stopwacht should stop.
        // lue::wait_all(interception_storage);
        // lue::wait_all(infiltration_storage);
    }

}  // namespace lue::benchmark
