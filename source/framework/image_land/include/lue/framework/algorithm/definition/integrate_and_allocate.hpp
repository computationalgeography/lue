#pragma once
#include "lue/framework/algorithm/detail/localities.hpp"
#include "lue/framework/algorithm/image_land_operation_export.hpp"
#include "lue/framework/algorithm/integrate_and_allocate.hpp"
#include "lue/framework/algorithm/walk.hpp"
#include "lue/framework/core/component.hpp"
#include "lue/macro.hpp"
#include <hpx/include/components.hpp>
#include <hpx/serialization.hpp>
#include <hpx/synchronization/shared_mutex.hpp>
#include <set>


namespace lue {

    /*!
        @brief      Return whether two maps have the same keys, no less and no more
    */
    template<typename Key, typename Value1, typename Value2>
    [[nodiscard]] auto keys_are_equal(std::map<Key, Value1> const& map1, std::map<Key, Value2> const& map2)
        -> bool
    {
        bool result = false;

        if (map1.size() == map2.size())
        {
            auto it1 = map1.begin();
            auto it2 = map2.begin();

            for (; it1 != map1.end(); ++it1, ++it2)
            {
                if (it1->first != it2->first)
                {
                    break;
                }
            }

            result = it1 == map1.end();
        }

        return result;
    }

}  // namespace lue


namespace lue::detail::integrate_and_allocate {

    template<typename Element>
    using Array = PartitionedArray<Element, 2>;

    template<typename Element>
    using Partition = PartitionT<Array<Element>>;

    template<typename Element>
    using Partitions = PartitionsT<Array<Element>>;


    template<typename RouteID, typename ZoneElement, typename FloatingPointElement>
    class Data
    {

        public:

            using BooleanElement = bool;
            using ZonalProductionByCrop = std::map<ZoneElement, std::vector<FloatingPointElement>>;
            using ZonalDemandByCrop = std::map<ZoneElement, std::vector<FloatingPointElement>>;
            using ZonalDemandMet = std::map<ZoneElement, std::vector<BooleanElement>>;

            static_assert(std::is_same_v<RouteID, ZoneElement>);


            Data() = default;


            Data(
                ZonalDemandByCrop const& zonal_demand_by_crop,
                ZonalProductionByCrop const& zonal_production_by_crop):

                _zonal_demand_by_crop{zonal_demand_by_crop},
                _zonal_production_by_crop{zonal_production_by_crop},
                _zonal_demand_met{}

            {
                assert(std::size(_zonal_production_by_crop) == std::size(_zonal_demand_by_crop));

                assert(keys_are_equal(_zonal_demand_by_crop, _zonal_production_by_crop));

                // Per zone and per crop, whether or not demand is already met by the
                // production. These values are being updated during the visit. As soon as demand
                // is met, for a crop in a zone, things won't change for that crop and zone anymore.

                for (auto const& [zone, demand_by_crop] : _zonal_demand_by_crop)
                {
                    assert(_zonal_production_by_crop.find(zone) != _zonal_production_by_crop.end());

                    std::size_t const nr_crops{std::size(demand_by_crop)};

                    auto const& current_production_by_crop{_zonal_production_by_crop.at(zone)};

                    assert(std::size(current_production_by_crop) == nr_crops);

                    _zonal_demand_met[zone].resize(nr_crops, false);

                    for (std::size_t crop_idx = 0; crop_idx < nr_crops; ++crop_idx)
                    {
                        _zonal_demand_met[zone][crop_idx] = demand_by_crop <= current_production_by_crop;
                    }
                }

                assert(keys_are_equal(_zonal_demand_by_crop, _zonal_demand_met));
            }


            [[nodiscard]] auto demand(ZoneElement const zone, Index const crop_idx) -> FloatingPointElement
            {
                assert(_zonal_demand_by_crop.find(zone) != _zonal_demand_by_crop.end());

                auto const& demand{_zonal_demand_by_crop.at(zone)};
                auto const nr_crops = std::size(demand);

                assert(crop_idx < static_cast<Index>(nr_crops));

                return demand[nr_crops];
            }


            [[nodiscard]] auto production(ZoneElement const zone, Index const crop_idx)
                -> FloatingPointElement
            {
                assert(_zonal_production_by_crop.find(zone) != _zonal_production_by_crop.end());

                auto const& production{_zonal_production_by_crop.at(zone)};
                auto const nr_crops = std::size(production);

                assert(crop_idx < static_cast<Index>(nr_crops));

                return production[crop_idx];
            }


            auto set_production(
                ZoneElement const zone, Index const crop_idx, FloatingPointElement const production) -> void
            {
                _zonal_production_by_crop.at(zone)[crop_idx] = production;
            }


            [[nodiscard]] auto demand_met(ZoneElement const zone, Index const crop_idx) -> bool
            {
                assert(_zonal_demand_met.find(zone) != _zonal_demand_met.end());

                auto const& demand_met{_zonal_demand_met.at(zone)};
                auto const nr_crops = std::size(demand_met);

                assert(crop_idx < static_cast<Index>(nr_crops));

                return demand_met[nr_crops];
            }


            [[nodiscard]] auto keep_walking() const -> bool
            {
                return std::all_of(
                    _zonal_demand_met.begin(),
                    _zonal_demand_met.end(),
                    [](auto const& demand_met_pair) -> bool
                    {
                        return std::all_of(
                            demand_met_pair.second.begin(),
                            demand_met_pair.second.end(),
                            [](bool const met) { return met; });
                    });
            }

        private:

            friend class hpx::serialization::access;

            template<typename Archive>
            void serialize(Archive& archive, [[maybe_unused]] unsigned int const version)
            {
                // clang-format off
                archive & _zonal_demand_by_crop & _zonal_production_by_crop & _zonal_demand_met;
                // clang-format on
            }

            ZonalDemandByCrop _zonal_demand_by_crop;

            ZonalProductionByCrop _zonal_production_by_crop;

            ZonalDemandMet _zonal_demand_met;
    };


    template<typename Policies, Rank rank>
    class Walk;


    namespace server {

        template<typename Policies, Rank rank>
        class Walk: public hpx::components::component_base<Walk<Policies, rank>>
        {

            private:

                using ComponentServerBase = hpx::components::component_base<Walk<Policies, rank>>;
                using ComponentClient = integrate_and_allocate::Walk<Policies, rank>;

                using RouteID = policy::InputElementT<Policies, 0>;
                using ZoneElement = RouteID;
                using SDPFactorElement = policy::InputElementT<Policies, 1>;
                using YieldFactorElement = policy::InputElementT<Policies, 2>;
                using CropFractionElement = policy::InputElementT<Policies, 3>;
                using DemandElement = policy::InputElementT<Policies, 4>;
                using ProductionElement = policy::InputElementT<Policies, 5>;
                using IrrigatedCropFractionElement = policy::InputElementT<Policies, 6>;

                static_assert(std::is_same_v<policy::OutputElementT<Policies, 0>, CropFractionElement>);
                static_assert(std::is_same_v<policy::OutputElementT<Policies, 1>, ProductionElement>);

            public:

                using RoutePartition = lue::SerialRoutePartition<RouteID, rank>;
                using SDPFactorPartition = ArrayPartition<SDPFactorElement, rank>;
                using SDPFactorPartitions = std::vector<SDPFactorPartition>;
                using YieldFactorPartition = ArrayPartition<YieldFactorElement, rank>;
                using YieldFactorPartitions = std::vector<YieldFactorPartition>;
                using CropFractionPartition = ArrayPartition<CropFractionElement, rank>;
                using CropFractionPartitions = std::vector<CropFractionPartition>;
                using IrrigatedCropFractionPartition = ArrayPartition<IrrigatedCropFractionElement, rank>;

                using ZonalProductions = std::map<ZoneElement, std::vector<ProductionElement>>;
                using ZonalProductionsF = hpx::future<ZonalProductions>;


                Walk(
                    Policies const& policies,
                    RoutePartition const& route_partition,
                    SDPFactorPartitions const& sdp_factor_per_crop_partitions,
                    YieldFactorPartitions const& yield_factor_per_crop_partitions,
                    CropFractionPartitions const& current_crop_fraction_per_crop_partitions,
                    IrrigatedCropFractionPartition const& irrigated_crop_fraction_partition):

                    ComponentServerBase{},
                    _policies{policies},
                    _crop_fraction_partitions_promises{},
                    _zonal_productions_per_crop_promise{},
                    _crop_fractions_per_crop_partitions{},  // Output
                    _zonal_productions_per_crop{},          // Output
                    _route_partition{route_partition},
                    _sdp_factor_per_crop_partitions{sdp_factor_per_crop_partitions},
                    _yield_factor_per_crop_partitions{yield_factor_per_crop_partitions},
                    _current_crop_fraction_per_crop_partitions{current_crop_fraction_per_crop_partitions},
                    _irrigated_crop_fraction_partition{irrigated_crop_fraction_partition},
                    _walk_mutex{},
                    _downstream_components{},
                    _fragment_idxs{}

                {
                    Count const nr_crops = std::size(_sdp_factor_per_crop_partitions);

                    _crop_fraction_partitions_promises.resize(nr_crops);

                    lue_hpx_assert(_route_partition.is_ready());
                    auto route_partition_ptr{ready_component_ptr(_route_partition)};

                    // Initialize output partitions with no-data
                    {
                        _crop_fractions_per_crop_partitions.resize(nr_crops);

                        auto const& ondp0 = std::get<0>(_policies.outputs_policies()).output_no_data_policy();
                        CropFractionElement crop_fraction_no_data_value;
                        ondp0.mark_no_data(crop_fraction_no_data_value);

                        using CropFractionPartitionServer = typename CropFractionPartition::Server;

                        for (Index crop_idx = 0; crop_idx < nr_crops; ++crop_idx)
                        {
                            _crop_fractions_per_crop_partitions[crop_idx] =
                                CropFractionPartition{hpx::new_<CropFractionPartitionServer>(
                                    hpx::find_here(),
                                    route_partition_ptr->offset(),
                                    route_partition_ptr->shape(),
                                    crop_fraction_no_data_value)};
                        }
                    }

                    std::set<RouteID> const route_ids{route_partition_ptr->route_ids()};

                    for (RouteID const route_id : route_ids)
                    {
                        // Initialize output productions for all zones and crops with zero
                        _zonal_productions_per_crop[route_id] = std::vector<ProductionElement>(nr_crops, 0);

                        // Initialize with idx. First fragment to visit has index
                        // zero. This index increases during the walk, until the last
                        // fragment has been handled, after which the index for this
                        // route_id can be removed. When all indices have been removed,
                        // the walk has finished.
                        _fragment_idxs[route_id] = 0;
                    }
                }


                std::tuple<CropFractionPartitions, ZonalProductionsF> results()
                {
                    // Connect the promises with the futures
                    // This function must be called exactly once

                    Count const nr_crops = std::size(_crop_fractions_per_crop_partitions);

                    CropFractionPartitions crop_fraction_partitions(nr_crops);

                    for (Index crop_idx = 0; crop_idx < nr_crops; ++crop_idx)
                    {
                        crop_fraction_partitions[crop_idx] =
                            CropFractionPartition{_crop_fraction_partitions_promises[crop_idx].get_future()};
                    }

                    ZonalProductionsF zonal_productions_per_crop_f{
                        _zonal_productions_per_crop_promise.get_future()};

                    // If the routes partition contains no fragments, the results
                    // can be marked as finished immediately
                    if (_fragment_idxs.empty())
                    {
                        finish_results();
                    }

                    return std::make_tuple(
                        std::move(crop_fraction_partitions), std::move(zonal_productions_per_crop_f));
                }


                void set_downstream_components(std::map<hpx::id_type, ComponentClient>&& components)
                {
                    _downstream_components = components;
                }


                void walk(RouteID const route_id, Data<RouteID, ZoneElement, ProductionElement>&& data)
                {
                    // This function may be called by multiple threads, for different
                    // routes, or for the same route but for different fragments. In
                    // the latter case, these calls will be ordered, with the first
                    // call handling an upstream fragment and subsequent calls handling
                    // downstream fragments.

                    std::shared_lock read_lock{_walk_mutex, std::defer_lock};
                    std::unique_lock write_lock{_walk_mutex, std::defer_lock};

                    // First, do stuff, *using* state veriables. Don't change stuff that
                    // is shared between threads. Changing different cells in a raster
                    // is fine, just don't change the same values.
                    read_lock.lock();

                    auto route_partition_server_ptr{ready_component_ptr(_route_partition)};
                    auto& route_partition_server{*route_partition_server_ptr};

                    lue_hpx_assert(
                        route_partition_server.route_ids().find(route_id) !=
                        route_partition_server.route_ids().end());

                    lue_hpx_assert(!_fragment_idxs.empty());
                    lue_hpx_assert(_fragment_idxs.find(route_id) != _fragment_idxs.end());

                    Index& fragment_idx{_fragment_idxs.at(route_id)};

                    auto const& route_fragments{route_partition_server.route_fragments(route_id)};

                    lue_hpx_assert(fragment_idx < static_cast<Size>(std::size(route_fragments)));

                    auto const& route_fragment{route_fragments[fragment_idx]};

                    auto const& cell_idxs{route_fragment.cell_idxs()};

                    // For as long as needed, iterate over each cell in the current
                    // fragment. For each cell:
                    // - Calculate output values
                    // - Assign output values to corresponding cell in the output partitions

                    Count const nr_crops = std::size(_crop_fractions_per_crop_partitions);

                    // SDP factor -----
                    std::vector<std::shared_ptr<typename SDPFactorPartition::Server>>
                        sdp_factor_per_crop_partition_server_ptrs(std::size(_sdp_factor_per_crop_partitions));
                    std::transform(
                        _sdp_factor_per_crop_partitions.begin(),
                        _sdp_factor_per_crop_partitions.end(),
                        sdp_factor_per_crop_partition_server_ptrs.begin(),
                        [](auto const& partition_client) { return ready_component_ptr(partition_client); });

                    std::vector<typename SDPFactorPartition::Data> sdp_factor_per_crop_partition_data(
                        std::size(_sdp_factor_per_crop_partitions));
                    std::transform(
                        sdp_factor_per_crop_partition_server_ptrs.begin(),
                        sdp_factor_per_crop_partition_server_ptrs.end(),
                        sdp_factor_per_crop_partition_data.begin(),
                        [](auto const& partition_server_ptr) { return partition_server_ptr->data(); });
                    auto const& sdp_factor_per_crop_partition_indp =
                        std::get<1>(_policies.inputs_policies()).input_no_data_policy();

                    // Yield factor -----
                    std::vector<std::shared_ptr<typename YieldFactorPartition::Server>>
                        yield_factor_per_crop_partition_server_ptrs(
                            std::size(_yield_factor_per_crop_partitions));
                    std::transform(
                        _yield_factor_per_crop_partitions.begin(),
                        _yield_factor_per_crop_partitions.end(),
                        yield_factor_per_crop_partition_server_ptrs.begin(),
                        [](auto const& partition_client) { return ready_component_ptr(partition_client); });

                    std::vector<typename YieldFactorPartition::Data> yield_factor_per_crop_partition_data(
                        std::size(_yield_factor_per_crop_partitions));
                    std::transform(
                        yield_factor_per_crop_partition_server_ptrs.begin(),
                        yield_factor_per_crop_partition_server_ptrs.end(),
                        yield_factor_per_crop_partition_data.begin(),
                        [](auto const& partition_server_ptr) { return partition_server_ptr->data(); });
                    auto const& yield_factor_per_crop_partition_indp =
                        std::get<2>(_policies.inputs_policies()).input_no_data_policy();

                    // Crop fraction -----
                    std::vector<std::shared_ptr<typename CropFractionPartition::Server>>
                        current_crop_fraction_per_crop_partition_server_ptrs(
                            std::size(_current_crop_fraction_per_crop_partitions));
                    std::transform(
                        _current_crop_fraction_per_crop_partitions.begin(),
                        _current_crop_fraction_per_crop_partitions.end(),
                        current_crop_fraction_per_crop_partition_server_ptrs.begin(),
                        [](auto const& partition_client) { return ready_component_ptr(partition_client); });

                    std::vector<typename CropFractionPartition::Data>
                        current_crop_fraction_per_crop_partition_data(
                            std::size(_current_crop_fraction_per_crop_partitions));
                    std::transform(
                        current_crop_fraction_per_crop_partition_server_ptrs.begin(),
                        current_crop_fraction_per_crop_partition_server_ptrs.end(),
                        current_crop_fraction_per_crop_partition_data.begin(),
                        [](auto const& partition_server_ptr) { return partition_server_ptr->data(); });
                    auto const& current_crop_fraction_per_crop_partition_indp =
                        std::get<3>(_policies.inputs_policies()).input_no_data_policy();

                    // Irrigated crop fraction -----
                    auto irrigated_crop_fraction_partition_server_ptr{
                        ready_component_ptr(_irrigated_crop_fraction_partition)};
                    auto& irrigated_crop_fraction_partition_server{
                        *irrigated_crop_fraction_partition_server_ptr};
                    auto irrigated_crop_fraction_partition_data{
                        irrigated_crop_fraction_partition_server.data()};
                    auto const& irrigated_crop_fraction_partition_indp =
                        std::get<6>(_policies.inputs_policies()).input_no_data_policy();

                    // Crop fraction per crop partition (output) -----
                    std::vector<std::shared_ptr<typename CropFractionPartition::Server>>
                        crop_fraction_per_crop_partition_server_ptrs(
                            std::size(_crop_fractions_per_crop_partitions));
                    std::transform(
                        _crop_fractions_per_crop_partitions.begin(),
                        _crop_fractions_per_crop_partitions.end(),
                        crop_fraction_per_crop_partition_server_ptrs.begin(),
                        [](auto const& partition_client) { return ready_component_ptr(partition_client); });

                    std::vector<typename CropFractionPartition::Data> crop_fraction_per_crop_partition_data(
                        std::size(_crop_fractions_per_crop_partitions));
                    std::transform(
                        crop_fraction_per_crop_partition_server_ptrs.begin(),
                        crop_fraction_per_crop_partition_server_ptrs.end(),
                        crop_fraction_per_crop_partition_data.begin(),
                        [](auto const& partition_server_ptr) { return partition_server_ptr->data(); });

                    // Within a single cell:
                    // Region cell is in is route_id, current_crop_fractions_per_crop is the list
                    // of crop fractions for the current cell, yield_factors_per_crop is the list
                    // of yield factors (1 per crop) in the current cell, irrigated_crop_fraction is the
                    // fraction of the current cell taken up by irrigated crops. sdp_factors_per_crop is the
                    // list of sdp factors in the current cell. EPS is a threshold value (eg 1e-6)

                    // A. Correct current fractions in case they are non-zero
                    // B. Allocate extra fractions, in case current fraction does not allocate
                    //    the whole cell (sum of all crop fractions < 1)
                    // C. Correct extra fractions


                    // 1. Demand is met: reset fractions to zero
                    // 2. Demand not met, but gets met in the current cel. Likely needs adjusting because it
                    // overshoots.
                    // 3. Demand is not met. Do nothing to current fraction.

                    std::vector<CropFractionElement> extra_crop_fractions_per_crop(nr_crops, 0);
                    std::vector<CropFractionElement> current_crop_fractions_per_crop(nr_crops);
                    std::vector<SDPFactorElement> sdp_factors_per_crop(nr_crops);
                    std::vector<YieldFactorElement> yield_factors_per_crop(nr_crops);
                    std::vector<DemandElement> remaining_demand_per_crop(nr_crops);

                    // Magic number. Pass this index in?
                    Index const grass_idx{0};

                    // return new_fracs_flat, zonal_productions_per_crop
                    for (std::size_t idx = 0; idx < cell_idxs.size() && data.keep_walking();
                         ++idx)  // , ++data)
                    {
                        Index const cell_idx{cell_idxs[idx]};

                        if (std::any_of(
                                sdp_factor_per_crop_partition_data.begin(),
                                sdp_factor_per_crop_partition_data.end(),
                                [&indp = sdp_factor_per_crop_partition_indp, cell_idx](auto const& data)
                                { return indp.is_no_data(data, cell_idx); }) ||
                            std::any_of(
                                yield_factor_per_crop_partition_data.begin(),
                                yield_factor_per_crop_partition_data.end(),
                                [&indp = yield_factor_per_crop_partition_indp, cell_idx](auto const& data)
                                { return indp.is_no_data(data, cell_idx); }) ||
                            std::any_of(
                                current_crop_fraction_per_crop_partition_data.begin(),
                                current_crop_fraction_per_crop_partition_data.end(),
                                [&indp = current_crop_fraction_per_crop_partition_indp,
                                 cell_idx](auto const& data) { return indp.is_no_data(data, cell_idx); }) ||
                            irrigated_crop_fraction_partition_indp.is_no_data(
                                irrigated_crop_fraction_partition_data, cell_idx))
                        {
                            // Skip this cell. Easiest for now, but also correct?
                            break;
                        }


                        for (Index crop_idx = 0; crop_idx < nr_crops; ++crop_idx)
                        {
                            current_crop_fractions_per_crop[crop_idx] =
                                current_crop_fraction_per_crop_partition_data[crop_idx][cell_idx];
                            sdp_factors_per_crop[crop_idx] =
                                sdp_factor_per_crop_partition_data[crop_idx][cell_idx];
                            yield_factors_per_crop[crop_idx] =
                                yield_factor_per_crop_partition_data[crop_idx][cell_idx];
                        }

                        // adjust demand surpassers function, 1st call (includes grass)
                        for (Index crop_idx = 0; crop_idx < nr_crops; ++crop_idx)
                        {
                            // if demand already met, set frac to 0
                            if (data.demand_met(route_id, crop_idx))
                            {
                                current_crop_fractions_per_crop[crop_idx] = 0.0;
                            }
                            else
                            {
                                // now considering case where current cell may make prod>dem
                                ProductionElement temp_zonal_production =
                                    data.production(route_id, crop_idx) +
                                    current_crop_fractions_per_crop[crop_idx] *
                                        yield_factors_per_crop[crop_idx];

                                bool temp_zonal_demand_met =
                                    data.demand(route_id, crop_idx) <= temp_zonal_production;

                                if (temp_zonal_demand_met && current_crop_fractions_per_crop[crop_idx] > 0)
                                {
                                    if (yield_factors_per_crop[crop_idx] > 0)
                                    {
                                        current_crop_fractions_per_crop[crop_idx] =
                                            (data.demand(route_id, crop_idx) -
                                             data.production(route_id, crop_idx)) /
                                            yield_factors_per_crop[crop_idx];
                                    }
                                    else
                                    {
                                        current_crop_fractions_per_crop[crop_idx] = 0.0;
                                    }
                                }
                            }
                        }

                        // sum of current_crop_fractions_per_crop
                        CropFractionElement f_sum = 0;

                        // compute projected yields, update reg prod and sum of fracs
                        for (Index crop_idx = 0; crop_idx < nr_crops; ++crop_idx)
                        {
                            YieldFactorElement yield =
                                current_crop_fractions_per_crop[crop_idx] * yield_factors_per_crop[crop_idx];
                            data.set_production(
                                route_id, crop_idx, data.production(route_id, crop_idx) + yield);
                            f_sum += current_crop_fractions_per_crop[crop_idx];
                        }

                        // add irrigated fraction to f_sum to ensure normalisation
                        f_sum += irrigated_crop_fraction_partition_data[cell_idx];

                        // compute remaining demand (list of same length as current_crop_fractions_per_crop)
                        // and sdp
                        SDPFactorElement sdp = 0.0;

                        for (Index crop_idx = 0; crop_idx < nr_crops; ++crop_idx)
                        {
                            if (data.demand_met(route_id, crop_idx))
                            {
                                remaining_demand_per_crop[crop_idx] = 0.0;
                            }
                            else
                            {
                                remaining_demand_per_crop[crop_idx] =
                                    data.demand(route_id, crop_idx) - data.production(route_id, crop_idx);
                            }

                            sdp += remaining_demand_per_crop[crop_idx] * sdp_factors_per_crop[crop_idx];
                        }

                        // allocate (rest of) new crop_idx fractions, except for grass
                        // NB: extra_crop_fractions_per_crop is a list of same length as
                        // current_crop_fractions_per_crop
                        if (sdp > 0)
                        {
                            for (Index crop_idx = 0; crop_idx < nr_crops; ++crop_idx)
                            {
                                if (crop_idx != grass_idx)
                                {
                                    extra_crop_fractions_per_crop[crop_idx] =
                                        ((1 - f_sum) * sdp_factors_per_crop[crop_idx] / sdp) *
                                        remaining_demand_per_crop[crop_idx];
                                }
                            }
                        }

                        // BRW: at this point need to double check extra_crop_fractions_per_crop aren't NaN

                        // Now make sure demand not surpassed by extra_crop_fractions_per_crop
                        // adjust demand surpassers function, 2nd call (excluding grass)
                        for (Index crop_idx = 0; crop_idx < nr_crops; ++crop_idx)
                        {
                            if (crop_idx != grass_idx)
                            {
                                if (data.demand_met(route_id, crop_idx))
                                {
                                    extra_crop_fractions_per_crop[crop_idx] = 0.0;
                                }
                                else
                                {
                                    // now considering case where current cell may make prod > dem
                                    ProductionElement temp_zonal_production =
                                        data.production(route_id, crop_idx) +
                                        extra_crop_fractions_per_crop[crop_idx] *
                                            yield_factors_per_crop[crop_idx];
                                    bool temp_zonal_demand_met =
                                        data.demand(route_id, crop_idx) <= temp_zonal_production;

                                    if (temp_zonal_demand_met && extra_crop_fractions_per_crop[crop_idx] > 0)
                                    {
                                        if (yield_factors_per_crop[crop_idx] > 0)
                                        {
                                            extra_crop_fractions_per_crop[crop_idx] =
                                                (data.demand(route_id, crop_idx) -
                                                 data.production(route_id, crop_idx)) /
                                                yield_factors_per_crop[crop_idx];
                                        }
                                        else
                                        {
                                            extra_crop_fractions_per_crop[crop_idx] = 0.0;
                                        }
                                    }
                                }
                            }
                        }

                        // update f_sum to include extra_crop_fractions_per_crop; update data.production

                        for (Index crop_idx = 0; crop_idx < nr_crops; ++crop_idx)
                        {
                            ProductionElement extra_yield =
                                extra_crop_fractions_per_crop[crop_idx] * yield_factors_per_crop[crop_idx];
                            data.set_production(
                                route_id, crop_idx, data.production(route_id, crop_idx) + extra_yield);
                            f_sum += extra_crop_fractions_per_crop[crop_idx];
                        }


                        // fill any remaining space with grass
                        if (f_sum < 1)
                        {
                            extra_crop_fractions_per_crop[grass_idx] = 1 - f_sum;
                        }

                        for (Index crop_idx = 0; crop_idx < nr_crops; ++crop_idx)
                        {
                            crop_fraction_per_crop_partition_data[crop_idx][cell_idx] =
                                current_crop_fractions_per_crop[crop_idx] +
                                extra_crop_fractions_per_crop[crop_idx];
                        }
                    }

                    read_lock.unlock();
                    write_lock.lock();
                    ++fragment_idx;

                    // TODO refactor with integrate.hpp
                    if (!route_fragment.is_last())
                    {
                        if (data.keep_walking())
                        {
                            // Continue the walk in a downstream partition
                            lue_hpx_assert(route_fragment.next_fragment_location().valid());
                            lue_hpx_assert(route_fragment.next_fragment_location().is_ready());

                            hpx::id_type const downstream_route_partition_id{
                                route_fragment.next_fragment_location().get()};

                            lue_hpx_assert(
                                _downstream_components.find(downstream_route_partition_id) !=
                                _downstream_components.end());

                            _downstream_components.at(downstream_route_partition_id)
                                .walk(route_id, std::move(data));
                        }
                        else
                        {
                            lue_hpx_assert(route_fragment.next_fragment_location().valid());
                            lue_hpx_assert(route_fragment.next_fragment_location().is_ready());

                            hpx::id_type const downstream_route_partition_id{
                                route_fragment.next_fragment_location().get()};

                            lue_hpx_assert(
                                _downstream_components.find(downstream_route_partition_id) !=
                                _downstream_components.end());

                            _downstream_components.at(downstream_route_partition_id)
                                .skip_walking_route_fragments(route_id);
                        }
                    }

                    lue_hpx_assert(
                        (route_fragment.is_last() &&
                         fragment_idx == static_cast<Size>(std::size(route_fragments))) ||
                        !route_fragment.is_last());

                    if (fragment_idx == static_cast<Size>(std::size(route_fragments)))
                    {
                        // This was the last fragment of the route that is located in
                        // this partition. If it was the last fragment at all, we can / must
                        // finish the partition.
                        _fragment_idxs.erase(route_id);

                        if (_fragment_idxs.empty())
                        {
                            finish_results();
                        }
                    }
                    // /TODO refactor
                }


                // TODO refactor with integrate.hpp
                void skip_walking_route_fragments(RouteID const route_id)
                {
                    // Skip walking route fragments for the route with the ID passed in.
                    // If this results in an empty _fragment_idxs collection, then
                    // apparently we have handled all fragments of all routes, and the
                    // output partition can be finished.

                    // This function is called from another component. Since we will
                    // be changing the state of the current component, we need to obtain
                    // a write lock.

                    std::shared_lock read_lock{_walk_mutex, std::defer_lock};
                    std::unique_lock write_lock{_walk_mutex, std::defer_lock};

                    read_lock.lock();

                    lue_hpx_assert(!_fragment_idxs.empty());
                    lue_hpx_assert(_fragment_idxs.find(route_id) != _fragment_idxs.end());

                    Index& fragment_idx{_fragment_idxs.at(route_id)};

                    auto const& route_fragments{
                        ready_component_ptr(_route_partition)->route_fragments(route_id)};
                    lue_hpx_assert(fragment_idx < static_cast<Size>(std::size(route_fragments)));

                    auto const& route_fragment{route_fragments[fragment_idx]};

                    if (!route_fragment.is_last())
                    {
                        lue_hpx_assert(route_fragment.next_fragment_location().valid());
                        lue_hpx_assert(route_fragment.next_fragment_location().is_ready());

                        hpx::id_type const downstream_route_partition_id{
                            route_fragment.next_fragment_location().get()};

                        lue_hpx_assert(
                            _downstream_components.find(downstream_route_partition_id) !=
                            _downstream_components.end());

                        _downstream_components.at(downstream_route_partition_id)
                            .skip_walking_route_fragments(route_id);
                    }

                    read_lock.unlock();
                    write_lock.lock();

                    ++fragment_idx;

                    if (fragment_idx == static_cast<Size>(std::size(route_fragments)))
                    {
                        _fragment_idxs.erase(route_id);

                        if (_fragment_idxs.empty())
                        {
                            finish_results();
                        }
                    }
                }


                HPX_DEFINE_COMPONENT_ACTION(Walk, results, ResultsAction);

                HPX_DEFINE_COMPONENT_ACTION(Walk, set_downstream_components, SetDownstreamComponentsAction);

                HPX_DEFINE_COMPONENT_ACTION(Walk, walk, WalkAction);

                HPX_DEFINE_COMPONENT_ACTION(
                    Walk, skip_walking_route_fragments, SkipWalkingRouteFragmentsAction);

            private:

                void finish_results()
                {
                    // This function must be called exactly once

                    Count const nr_crops = std::size(_crop_fractions_per_crop_partitions);

                    for (Count crop_idx = 0; crop_idx < nr_crops; ++crop_idx)
                    {
                        lue_hpx_assert(_crop_fractions_per_crop_partitions[crop_idx].valid());
                        lue_hpx_assert(_crop_fractions_per_crop_partitions[crop_idx].is_ready());

                        _crop_fraction_partitions_promises[crop_idx].set_value(
                            _crop_fractions_per_crop_partitions[crop_idx].get_id());
                    }

                    _zonal_productions_per_crop_promise.set_value(std::move(_zonal_productions_per_crop));
                    _zonal_productions_per_crop.clear();

                    lue_hpx_assert(_fragment_idxs.empty());

                    _downstream_components.clear();
                }

                Policies _policies;

                //! Per crop a promise to a partition
                mutable std::vector<hpx::promise<hpx::id_type>> _crop_fraction_partitions_promises;

                //! A promise to the collection of productions per region
                mutable hpx::promise<ZonalProductions> _zonal_productions_per_crop_promise{};

                CropFractionPartitions _crop_fractions_per_crop_partitions;  // Output

                ZonalProductions _zonal_productions_per_crop;  // Output

                RoutePartition _route_partition;

                SDPFactorPartitions _sdp_factor_per_crop_partitions;

                YieldFactorPartitions _yield_factor_per_crop_partitions;

                CropFractionPartitions _current_crop_fraction_per_crop_partitions;

                IrrigatedCropFractionPartition _irrigated_crop_fraction_partition;

                hpx::shared_mutex _walk_mutex;

                //! Component client by routes partition ID
                std::map<hpx::id_type, ComponentClient> _downstream_components;

                //! Route fragment index by route ID
                std::map<RouteID, Index> _fragment_idxs;
        };

    }  // namespace server


    template<typename Policies, Rank rank>
    class Walk: public hpx::components::client_base<Walk<Policies, rank>, server::Walk<Policies, rank>>
    {

        private:

            using ComponentServer = server::Walk<Policies, rank>;
            using ComponentClientBase = hpx::components::client_base<Walk<Policies, rank>, ComponentServer>;
            using CropFractionPartitions = typename ComponentServer::CropFractionPartitions;
            using ZonalProductionsF = typename ComponentServer::ZonalProductionsF;

        public:

            using RouteID = policy::InputElementT<Policies, 0>;
            using ZoneElement = RouteID;
            using ProductionElement = policy::InputElementT<Policies, 2>;


            Walk():

                ComponentClientBase{}

            {
            }


            Walk(hpx::id_type const& component_id):

                ComponentClientBase{component_id}

            {
            }


            Walk(hpx::future<hpx::id_type>&& component_id):

                ComponentClientBase{std::move(component_id)}

            {
            }


            hpx::future<std::tuple<CropFractionPartitions, ZonalProductionsF>> results() const
            {
                lue_hpx_assert(this->is_ready());
                lue_hpx_assert(this->get_id());

                typename ComponentServer::ResultsAction action;

                return hpx::async(action, this->get_id());
            }


            hpx::future<void> set_downstream_components(std::map<hpx::id_type, Walk>&& components)
            {
                lue_hpx_assert(this->is_ready());
                lue_hpx_assert(this->get_id());

                typename ComponentServer::SetDownstreamComponentsAction action;

                return hpx::async(action, this->get_id(), std::move(components));
            }


            hpx::future<void> walk(
                RouteID const route_id, Data<RouteID, ZoneElement, ProductionElement> data) const
            {
                lue_hpx_assert(this->is_ready());
                lue_hpx_assert(this->get_id());

                typename ComponentServer::WalkAction action;

                return hpx::async(action, this->get_id(), route_id, std::move(data));
            }


            hpx::future<void> skip_walking_route_fragments(RouteID const route_id) const
            {
                lue_hpx_assert(this->is_ready());
                lue_hpx_assert(this->get_id());

                typename ComponentServer::SkipWalkingRouteFragmentsAction action;

                return hpx::async(action, this->get_id(), route_id);
            }
    };

}  // namespace lue::detail::integrate_and_allocate


namespace lue {

    template<typename Policies, Rank rank>
    auto integrate_and_allocate(
        Policies const& policies,
        SerialRoute<policy::InputElementT<Policies, 0>, rank> const& routes,
        std::vector<
            std::reference_wrapper<PartitionedArray<policy::InputElementT<Policies, 1>, rank> const>> const&
            sdp_factors_per_crop,
        std::vector<
            std::reference_wrapper<PartitionedArray<policy::InputElementT<Policies, 2>, rank> const>> const&
            yield_factors_per_crop,
        std::vector<
            std::reference_wrapper<PartitionedArray<policy::InputElementT<Policies, 3>, rank> const>> const&
            current_crop_fractions_per_crop,
        hpx::shared_future<std::map<
            policy::InputElementT<Policies, 0>,
            std::vector<policy::InputElementT<Policies, 4>>>> const& zonal_demands_per_crop,
        hpx::shared_future<std::map<
            policy::InputElementT<Policies, 0>,
            std::vector<policy::InputElementT<Policies, 5>>>> const& current_zonal_production_per_crop,
        PartitionedArray<policy::InputElementT<Policies, 6>, rank> const& irrigated_crop_fractions)
        -> std::tuple<
            std::vector<PartitionedArray<policy::OutputElementT<Policies, 0>, rank>>,
            hpx::shared_future<std::map<
                policy::InputElementT<Policies, 0>,
                std::vector<policy::OutputElementT<Policies, 1>>>>>
    {
        using RouteID = policy::InputElementT<Policies, 0>;
        using ZoneElement = RouteID;

        using SDPFactorElement = policy::InputElementT<Policies, 1>;
        using YieldFactorElement = policy::InputElementT<Policies, 2>;
        using CropFractionElement = policy::InputElementT<Policies, 3>;
        using DemandElement = policy::InputElementT<Policies, 4>;
        using ProductionElement = policy::InputElementT<Policies, 5>;

        static_assert(std::is_integral_v<RouteID>);
        static_assert(std::is_floating_point_v<SDPFactorElement>);
        static_assert(std::is_floating_point_v<YieldFactorElement>);
        static_assert(std::is_floating_point_v<CropFractionElement>);
        static_assert(std::is_floating_point_v<DemandElement>);
        static_assert(std::is_floating_point_v<ProductionElement>);

        static_assert(std::is_same_v<CropFractionElement, policy::InputElementT<Policies, 6>>);
        static_assert(std::is_same_v<CropFractionElement, policy::OutputElementT<Policies, 0>>);
        static_assert(std::is_same_v<ProductionElement, policy::OutputElementT<Policies, 1>>);

        static_assert(std::is_floating_point_v<ProductionElement>);

        using Route = SerialRoute<RouteID, rank>;
        using RoutePartition = PartitionT2<Route>;
        using RoutePartitions = PartitionsT2<Route>;

        using Shape = ShapeT<Route>;

        using SDPFactorArray = PartitionedArray<SDPFactorElement, rank>;
        using SDPFactorPartition = PartitionT<SDPFactorArray>;
        using SDPFactorPartitionArray = std::vector<SDPFactorPartition>;

        using YieldFactorArray = PartitionedArray<YieldFactorElement, rank>;
        using YieldFactorPartition = PartitionT<YieldFactorArray>;
        using YieldFactorPartitionArray = std::vector<YieldFactorPartition>;

        using CropFractionArray = PartitionedArray<CropFractionElement, rank>;
        using CropFractionPartition = PartitionT<CropFractionArray>;
        using CropFractionPartitions = PartitionsT<CropFractionArray>;
        using CropFractionPartitionArray = std::vector<CropFractionPartition>;
        using CropFractionPartitionsArray = std::vector<CropFractionPartitions>;
        using CropFractionArrays = std::vector<PartitionedArray<CropFractionElement, rank>>;

        namespace iaa = detail::integrate_and_allocate;

        Count const nr_crops{static_cast<Count>(std::size(current_crop_fractions_per_crop))};

        if (static_cast<Count>(std::size(sdp_factors_per_crop)) != nr_crops ||
            static_cast<Count>(std::size(yield_factors_per_crop)) != nr_crops ||
            static_cast<Count>(std::size(current_crop_fractions_per_crop)) != nr_crops)
        {
            throw std::runtime_error(
                "Length of collections differ: collections of sdp factors, yield factors, "
                "current crop fractions must have the same size");
        }

        using WalkComponentClient = detail::integrate_and_allocate::Walk<Policies, rank>;
        using WalkComponentServer = detail::integrate_and_allocate::server::Walk<Policies, rank>;

        RoutePartitions const& route_partitions{routes.partitions()};

        Array<hpx::future<hpx::id_type>, rank> locality_fs{detail::localities(route_partitions)};

        Shape const& shape_in_partitions{route_partitions.shape()};
        Count const nr_partitions{nr_elements(shape_in_partitions)};

        Array<WalkComponentClient, rank> walk_components{route_partitions.shape()};
        {
            SDPFactorPartitionArray sdp_factors(nr_crops);
            YieldFactorPartitionArray yield_factors(nr_crops);
            CropFractionPartitionArray current_crop_fractions(nr_crops);

            for (Index idx = 0; idx < nr_partitions; ++idx)
            {
                std::transform(
                    sdp_factors_per_crop.begin(),
                    sdp_factors_per_crop.end(),
                    sdp_factors.begin(),
                    [idx](SDPFactorArray const& array) { return array.partitions()[idx]; });
                std::transform(
                    yield_factors_per_crop.begin(),
                    yield_factors_per_crop.end(),
                    yield_factors.begin(),
                    [idx](YieldFactorArray const& array) { return array.partitions()[idx]; });
                std::transform(
                    current_crop_fractions_per_crop.begin(),
                    current_crop_fractions_per_crop.end(),
                    current_crop_fractions.begin(),
                    [idx](CropFractionArray const& array) { return array.partitions()[idx]; });

                walk_components[idx] = WalkComponentClient{hpx::dataflow(
                    hpx::launch::async,
                    [policies](
                        RoutePartition const& route_partition,
                        hpx::future<SDPFactorPartitionArray>&& sdp_factor_partition_array_fs,
                        hpx::future<YieldFactorPartitionArray>&& yield_factor_partition_array_fs,
                        hpx::future<CropFractionPartitionArray>&& current_crop_fractions_partition_array_fs,
                        CropFractionPartition const& current_crop_fraction_partition)
                    {
                        return hpx::new_<WalkComponentServer>(
                            hpx::colocated(route_partition.get_id()),
                            policies,
                            route_partition,
                            sdp_factor_partition_array_fs.get(),
                            yield_factor_partition_array_fs.get(),
                            current_crop_fractions_partition_array_fs.get(),
                            current_crop_fraction_partition);
                    },
                    route_partitions[idx],
                    hpx::when_all(sdp_factors),
                    hpx::when_all(yield_factors),
                    hpx::when_all(current_crop_fractions),
                    irrigated_crop_fractions.partitions()[idx])};
            }
        }

        // Per crop a collection of (futures to) array partitions, containing crop fractions
        CropFractionPartitionsArray crop_fractions_per_crop_partitions_array(nr_crops);

        // Per partition a future to a dictionary containing per zone a collection of crop productions
        std::vector<hpx::shared_future<std::map<ZoneElement, std::vector<ProductionElement>>>>
            zonal_productions_per_crop_per_partition(nr_partitions);

        {
            for (Index crop_idx = 0; crop_idx < nr_crops; ++crop_idx)
            {
                crop_fractions_per_crop_partitions_array[crop_idx] =
                    CropFractionPartitions{shape_in_partitions};
            }

            for (Index partition_idx = 0; partition_idx < nr_partitions; ++partition_idx)
            {
                // - Once walk component is ready
                //     - Get collection of result partitions. For each result array, for each crop a
                //     partition.
                // - Assign the partitions to the result arrays

                // future<vector<partition1>>, future<map<Zone, vector<Production>>>
                auto [crop_fractions_per_crop_partition_array_f, zonal_productions_per_crop_f] =
                    hpx::split_future(walk_components[partition_idx].then(hpx::unwrapping(
                        [](WalkComponentClient const& component)
                        {
                            return hpx::dataflow(
                                hpx::launch::async,
                                hpx::unwrapping(
                                    [](auto&& results)
                                    {
                                        auto crop_fractions_partition{std::get<0>(results)};
                                        auto zonal_productions_per_crop_f{std::get<1>(results).get()};

                                        return std::make_tuple(
                                            std::move(crop_fractions_partition),
                                            std::move(zonal_productions_per_crop_f));
                                    }),
                                component.results());
                        })));

                // shared_future<vector<partition1>>
                auto crop_fractions_per_crop_partition_array_sf =
                    crop_fractions_per_crop_partition_array_f.share();

                for (Index crop_idx = 0; crop_idx < nr_crops; ++crop_idx)
                {
                    crop_fractions_per_crop_partitions_array[crop_idx][partition_idx] =
                        crop_fractions_per_crop_partition_array_sf.then(
                            [crop_idx](auto const& crop_fractions_per_crop_partition_array_sf)
                            { return crop_fractions_per_crop_partition_array_sf.get()[crop_idx]; });
                }

                zonal_productions_per_crop_per_partition[partition_idx] =
                    zonal_productions_per_crop_f.share();
            }
        }

        hpx::future<iaa::Data<RouteID, ZoneElement, ProductionElement>> data_f = hpx::dataflow(
            hpx::launch::async,
            hpx::unwrapping(
                []([[maybe_unused]] auto const& route_starts,
                   auto const& zonal_demands,
                   auto const& current_zonal_productions_per_crop)
                {
                    // TODO For some reason throwing an exception here hangs the processes...
                    // if(
                    //     !keys_are_equal(route_starts, zonal_demands) ||
                    //     !keys_are_equal(route_starts, current_zonal_productions_per_crop))
                    // {
                    //     throw std::runtime_error(
                    //         "Route-id's differ from zones: zonal demands and current zonal production per
                    //         crop " "must contain information for all routes");
                    // }

                    // if(!keys_are_equal(zonal_demands, current_zonal_productions_per_crop))
                    // {
                    //     throw std::runtime_error("Zones differ: zonal demands and current zonal production
                    //     per crop must contain information for the same zones");
                    // }

                    return iaa::Data<RouteID, ZoneElement, ProductionElement>{
                        zonal_demands, current_zonal_productions_per_crop};
                }),
            routes.starts(),
            zonal_demands_per_crop,
            current_zonal_production_per_crop);

        // Do whatever it takes to end up with ready output partitions
        walk(routes, walk_components, std::move(data_f));

        // Keep the components alive for as long as the results are not ready
        {
            std::vector<hpx::future<void>> partitions_ready_fs{};
            partitions_ready_fs.reserve(nr_crops);

            for (Index crop_idx = 0; crop_idx < nr_crops; ++crop_idx)
            {
                partitions_ready_fs.push_back(hpx::when_all(
                    crop_fractions_per_crop_partitions_array[crop_idx].begin(),
                    crop_fractions_per_crop_partitions_array[crop_idx].end()));
            }

            auto crop_fractions_ready_f = hpx::when_all(partitions_ready_fs);
            auto zonal_crop_productions_ready_f = hpx::when_all(zonal_productions_per_crop_per_partition);

            hpx::when_all(crop_fractions_ready_f, zonal_crop_productions_ready_f)
                .then([components = std::move(walk_components)](auto&&) { HPX_UNUSED(components); });
        }

        // TODO Get rid of this wait and localities stuff
        Array<hpx::id_type, rank> localities{locality_fs.shape()};
        {
            hpx::wait_all(locality_fs.begin(), locality_fs.end());
            std::transform(
                locality_fs.begin(),
                locality_fs.end(),
                localities.begin(),
                [](hpx::future<hpx::id_type>& locality_f) { return locality_f.get(); });
        }

        CropFractionArrays crop_fraction_arrays(nr_crops);

        for (Index crop_idx = 0; crop_idx < nr_crops; ++crop_idx)
        {
            crop_fraction_arrays[crop_idx] = CropFractionArray{
                routes.shape(), localities, std::move(crop_fractions_per_crop_partitions_array[crop_idx])};
        }

        // Once the zonal crop productions for all partitions are available, we can aggregate
        // them into the zonal crop production collection for the whole array

        auto zonal_productions_per_crop_per_partition_f =
            hpx::when_all(zonal_productions_per_crop_per_partition)
                .then(hpx::unwrapping(
                    [](auto&& zonal_productions_per_crop_per_partition)
                    {
                        std::map<ZoneElement, std::vector<ProductionElement>> result{};

                        // hpx::shared_future<std::map<ZoneElement, std::vector<ProductionElement>>>
                        for (auto const& zonal_productions_per_crop_f :
                             zonal_productions_per_crop_per_partition)
                        {
                            std::map<ZoneElement, std::vector<ProductionElement>> const&
                                zonal_productions_per_crop{zonal_productions_per_crop_f.get()};

                            for (auto const& [zone, partition_productions] : zonal_productions_per_crop)
                            {
                                auto it = result.find(zone);

                                if (it == result.end())
                                {
                                    // First time we see this zone
                                    result[zone] = partition_productions;
                                }
                                else
                                {
                                    // Result already contains information about this zone.
                                    auto& result_productions{it->second};
                                    auto const nr_crops{std::size(result_productions)};

                                    // We assume that for all partitions we get information about the
                                    // production for all crops.
                                    assert(std::size(partition_productions) == nr_crops);

                                    for (std::size_t crop_idx = 0; crop_idx < nr_crops; ++crop_idx)
                                    {
                                        result_productions[crop_idx] += partition_productions[crop_idx];
                                    }
                                }
                            }
                        }

                        return result;
                    }));

        return std::make_tuple(
            std::move(crop_fraction_arrays), zonal_productions_per_crop_per_partition_f.share());
    }

}  // namespace lue


#define LUE_REGISTER_INTEGRATE_AND_ALLOCATE_ACTION_DECLARATIONS(Policies, rank, unique)                      \
                                                                                                             \
    namespace lue::detail {                                                                                  \
        using IntegrateAndAllocateWalkServer_##unique =                                                      \
            lue::detail::integrate_and_allocate::server::Walk<Policies, rank>;                               \
    }                                                                                                        \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::IntegrateAndAllocateWalkServer_##unique::ResultsAction,                                 \
        IntegrateAndAllocateWalkServerResultsAction_##unique)                                                \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::IntegrateAndAllocateWalkServer_##unique::SetDownstreamComponentsAction,                 \
        IntegrateAndAllocateWalkServerSetRemoteComponentsAction_##unique)                                    \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::IntegrateAndAllocateWalkServer_##unique::WalkAction,                                    \
        IntegrateAndAllocateWalkServerWalkAction_##unique)                                                   \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::IntegrateAndAllocateWalkServer_##unique::SkipWalkingRouteFragmentsAction,               \
        IntegrateAndAllocateWalkServerSkipWalkingRouteFragmentsAction_##unique)


#define LUE_REGISTER_INTEGRATE_AND_ALLOCATE_ACTIONS(Policies, rank, unique)                                  \
                                                                                                             \
    namespace lue::detail {                                                                                  \
        using IntegrateAndAllocateWalkServerComponent_##unique =                                             \
            hpx::components::component<lue::detail::integrate_and_allocate::server::Walk<Policies, rank>>;   \
    }                                                                                                        \
                                                                                                             \
    HPX_REGISTER_COMPONENT(                                                                                  \
        lue::detail::IntegrateAndAllocateWalkServerComponent_##unique, IntegrateЅerverComponent_##unique)    \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::IntegrateAndAllocateWalkServer_##unique::ResultsAction,                                 \
        IntegrateAndAllocateWalkServerResultsAction_##unique)                                                \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::IntegrateAndAllocateWalkServer_##unique::SetDownstreamComponentsAction,                 \
        IntegrateAndAllocateWalkServerSetRemoteComponentsAction_##unique)                                    \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::IntegrateAndAllocateWalkServer_##unique::WalkAction,                                    \
        IntegrateAndAllocateWalkServerWalkAction_##unique)                                                   \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::IntegrateAndAllocateWalkServer_##unique::SkipWalkingRouteFragmentsAction,               \
        IntegrateAndAllocateWalkServerSkipWalkingRouteFragmentsAction_##unique)


#define LUE_INSTANTIATE_INTEGRATE_AND_ALLOCATE(Policies, rank)                                               \
                                                                                                             \
    template LUE_IMAGE_LAND_OPERATION_EXPORT std::tuple<                                                     \
        std::vector<PartitionedArray<policy::OutputElementT<Policies, 0>, rank>>,                            \
        hpx::shared_future<                                                                                  \
            std::map<policy::InputElementT<Policies, 0>, std::vector<policy::OutputElementT<Policies, 1>>>>> \
    integrate_and_allocate<ArgumentType<void(Policies)>, rank>(                                              \
        ArgumentType<void(Policies)> const&,                                                                 \
        SerialRoute<policy::InputElementT<Policies, 0>, rank> const&,                                        \
        std::vector<std::reference_wrapper<                                                                  \
            PartitionedArray<policy::InputElementT<Policies, 1>, rank> const>> const&,                       \
        std::vector<std::reference_wrapper<                                                                  \
            PartitionedArray<policy::InputElementT<Policies, 2>, rank> const>> const&,                       \
        std::vector<std::reference_wrapper<                                                                  \
            PartitionedArray<policy::InputElementT<Policies, 3>, rank> const>> const&,                       \
        hpx::shared_future<std::map<                                                                         \
            policy::InputElementT<Policies, 0>,                                                              \
            std::vector<policy::InputElementT<Policies, 4>>>> const&,                                        \
        hpx::shared_future<std::map<                                                                         \
            policy::InputElementT<Policies, 0>,                                                              \
            std::vector<policy::InputElementT<Policies, 5>>>> const&,                                        \
        PartitionedArray<policy::InputElementT<Policies, 6>, rank> const&)
