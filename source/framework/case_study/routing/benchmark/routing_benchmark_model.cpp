#include "routing_benchmark_model.hpp"
#include "lue/framework/algorithm/value_policies/algorithm.hpp"
#include "lue/framework/algorithm/copy.hpp"
#include "lue/framework/io/read_into.hpp"
#include "lue/framework/benchmark/data_model.hpp"


namespace lue::benchmark {
    namespace detail {

        /// class Model:
        ///     public RoutingModelBase
        /// {

        ///     public:

        ///         explicit Model(ScalarRasterPtr const& state_ptr):

        ///             RoutingModelBase{state_ptr}

        ///         {
        ///         }

        ///         Model(Model const&)=default;

        ///         Model(Model&&)=default;

        ///         ~Model()=default;

        ///         Model& operator=(Model const&)=default;

        ///         Model& operator=(Model&&)=default;


        ///     private:

        ///         // BooleanRaster initial_fire() const final
        ///         // {
        ///         //     return uniform(state(), ScalarElement{0.0}, ScalarElement{1.0}) < ScalarElement{1e-5};
        ///         // }

        /// };

    }  // namespace detail


    RoutingBenchmarkModel::RoutingBenchmarkModel(
        Environment const& /* environment */,
        Task const& task,
        std::size_t const max_tree_depth,
        std::string const& array_pathname,
        std::vector<Index> center_cell):

        BenchmarkModel<float, 2>{task, max_tree_depth},
        // _model{std::make_unique<detail::Model>(state_ptr())},
        _array_pathname{array_pathname},
        _object_id{},
        _hyperslab{}

    {
        std::tie(_object_id, _hyperslab) = array_info(array_pathname, center_cell, this->array_shape());
    }


    RoutingBenchmarkModel::~RoutingBenchmarkModel()
    {
        // Not =default because then the forward declared detail::Model icw
        // std::unique_ptr won't compile.
    }


    void RoutingBenchmarkModel::do_preprocess()
    {
        // namespace op = lue::value_policies;
        using namespace lue::value_policies;

        auto const& array_shape{this->array_shape()};
        auto const& partition_shape{this->partition_shape()};

        // Read (subset) from array whose pathname is passed in
        flow_direction = read<FlowDirectionElement, 2>(
            _array_pathname, _hyperslab, partition_shape, _object_id);

        // Interception rate: ~2 mm/h → 2 / 1000 m/s
        interception_rate =
            (2.0f + uniform<ScalarElement>(array_shape, partition_shape, -0.5, 0.5)) / 1000.0f;
        interception_storage =
            create_partitioned_array<ScalarElement>(array_shape, partition_shape, 0);

        // Infiltration rate: ~5 mm/h → 5 / 1000 m/s
        infiltration_rate =
            (5.0f + uniform<ScalarElement>(array_shape, partition_shape, -2, 2)) / 1000.0f;

        auto& infiltration_storage = state();
        infiltration_storage =
            create_partitioned_array<ScalarElement>(array_shape, partition_shape, 0);

        // We need to wait for all stuff that needs to be ready
        // before the calculations that need to be measured start.
        // state() is waited for automatically.
        lue::wait_all(flow_direction);
        lue::wait_all(interception_rate);
        lue::wait_all(interception_storage);
        lue::wait_all(infiltration_rate);
        // lue::wait_all(infiltration_storage);
    }


    void RoutingBenchmarkModel::do_initialize()
    {
        // Call functions of the specialization to initialize the modelled
        // state. Different specializations do this differently.
    }


    void RoutingBenchmarkModel::do_simulate(
        [[maybe_unused]] Count const time_step)
    {
        // Call a mix of modelling operations that is similar to the
        // mix found in the PyCatch model

        using ScalarRaster = PartitionedArray<ScalarElement, 2>;

        auto const& array_shape{this->array_shape()};
        auto const& partition_shape{this->partition_shape()};

        // Pull operations and operators that use the default value
        // policies from their namespace
        using namespace lue::value_policies;


        // Time step duration: 1 h → 1 * 60 * 60 s
        float const duration = 1 * 60 * 60;  // TODO initial


        // ---------------------------------------------------------------------
        // 'Read' precipitation: ~10 mm/h → 10 / 1000 m/s
        ScalarRaster precipitation =
            (10.0f + uniform<ScalarElement>(array_shape, partition_shape, -3, 3)) / 1000.0f;

        all(precipitation >= 0.0f).then(
            []([[maybe_unused]] auto&& f) { lue_hpx_assert(f.get()); });


        // ---------------------------------------------------------------------
        // Update state of interception component
        ScalarRaster interception_capacity = duration * interception_rate; // TODO initial
        ScalarRaster actual_interception_capacity = interception_capacity - interception_storage;

        // TODO add more operations to update actual_interception_capacity

        // Fill interception storage with as much water as possible.
        ScalarRaster intercepted_precipitation =
            where(actual_interception_capacity < precipitation, actual_interception_capacity, precipitation);
        interception_storage = interception_storage + intercepted_precipitation;
        precipitation = precipitation - intercepted_precipitation;

        all(interception_storage >= 0.0f).then(
            []([[maybe_unused]] auto&& f) { lue_hpx_assert(f.get()); });
        all(interception_storage < interception_capacity).then(
            []([[maybe_unused]] auto&& f) { lue_hpx_assert(f.get()); });


        // ---------------------------------------------------------------------
        // Update state of infiltration component
        auto& infiltration_storage = state();
        ScalarRaster infiltration_capacity = duration * infiltration_rate;  // TODO initial
        ScalarRaster actual_infiltration_capacity = infiltration_capacity - infiltration_storage;

        // TODO add more operations to update actual_infiltration_capacity
        // - evapotranspiration, forest floor interception, ...

        // Redistribute the water that reaches the soil laterally,
        // taking the infiltration capacity into account.

        auto [runoff, infiltrated_precipitation] =
            accu_threshold3(flow_direction, precipitation, actual_infiltration_capacity);

        infiltration_storage = infiltration_storage + infiltrated_precipitation;

        // TODO add more operations to update infiltration_storage
        // - seapage, ...

        all(infiltration_storage >= 0.0f).then(
            []([[maybe_unused]] auto&& f) { lue_hpx_assert(f.get()); });
        all(infiltration_storage < infiltration_capacity).then(
            []([[maybe_unused]] auto&& f) { lue_hpx_assert(f.get()); });




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


    }


    void RoutingBenchmarkModel::do_terminate()
    {
        // Wait for the results of the model to be available. Only then
        // the stopwacht should stop.
        // lue::wait_all(interception_storage);
        // lue::wait_all(infiltration_storage);
    }

}  // namespace lue::benchmark
