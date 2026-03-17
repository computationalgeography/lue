#pragma once
#include "lue/framework/partitioned_array_decl.hpp"
#include "lue/framework/serial_route.hpp"
#include <hpx/include/components.hpp>
#include <map>
#include <vector>


namespace lue {
    namespace detail {

    }  // namespace detail


    template<
        template<typename, typename...> typename Collection,
        typename... Ts,
        template<typename> typename Future,
        typename Element>
    hpx::future<Collection<Element>> when_all_get(Collection<Future<Element>, Ts...>&& futures)
    {
        // Given a collection of futures to elements, wait for all futures to become ready
        // and return a collection with the elements originally pointed to by the futures.

        return hpx::when_all(futures.begin(), futures.end())
            .then(
                hpx::unwrapping(
                    [](std::vector<Future<Element>>&& futures)
                    {
                        Collection<Element> elements(std::size(futures));

                        std::transform(
                            futures.begin(),
                            futures.end(),
                            elements.begin(),
                            [](Future<Element>& future) { return future.get(); });

                        return elements;
                    }));
    }


    template<template<typename> typename Future, typename Element, Rank rank>
    hpx::future<Array<Element, rank>> when_all_get(Array<Future<Element>, rank>&& array_of_futures)
    {
        return hpx::when_all(array_of_futures.begin(), array_of_futures.end())
            .then(
                hpx::unwrapping(
                    [shape = array_of_futures.shape()](std::vector<Future<Element>>&& futures)
                    {
                        Array<Element, rank> elements{shape};

                        std::transform(
                            futures.begin(),
                            futures.end(),
                            elements.begin(),
                            [](Future<Element>& future) { return future.get(); });

                        return elements;
                    }));
    }


    /*!
        @brief      Initiate a visit of all cells along all routes
        @param      route Routes to visit the cells of
        @param      components Components that will do the actual visiting
        @param      data State to use while visiting the cells
        @return     A future that becomes ready once the visiting is done
    */
    template<typename RouteID, Rank rank, typename Component, typename Data>
    hpx::future<void> walk(
        SerialRoute<RouteID, rank> const& route, Array<Component, rank>& components, hpx::future<Data>&& data)
    {
        // For each route:
        // - Visit all cells until some condition is false
        //     - Pass in a copy of the data passed in

        // Algorithm:
        // - Wait for all components to be ready
        // - Create a map of route partition ID → walk component
        // - Per partition create a map with downstream route partition IDs → walk component
        // - Pass partition-specific map to each walk component
        // - Initiate each route's walk by calling visit on the walk component containing the
        //   route's start fragment. The visit must continue from there on, for as long as needed

        using Route = SerialRoute<RouteID, rank>;
        using RoutePartition = PartitionT2<Route>;
        using DownstreamRoutePartitionIDs = std::vector<hpx::shared_future<hpx::id_type>>;

        auto const& route_partitions{route.partitions()};
        Count const nr_partitions{lue::nr_elements(route_partitions)};

        std::vector<hpx::future<DownstreamRoutePartitionIDs>> downstream_route_partitions_ids_ff(
            nr_partitions);

        // Per partition, obtain a future to the collection of downstream route partition IDs
        for (Index partition_idx = 0; partition_idx < nr_partitions; ++partition_idx)
        {
            downstream_route_partitions_ids_ff[partition_idx] = hpx::dataflow(
                hpx::launch::async,
                [](RoutePartition const& route_partition)
                { return route_partition.remote_route_fragment_locations(); },
                route_partitions[partition_idx]);
        }

        // What we have now:
        // - route_partitions → Futures to the ID of route partition component servers
        // - components → Futures to the ID of component servers
        // - downstream_route_partitions_ids_ff → Futures to collections of futures to downstream
        //   route partition IDs

        // What we want to have next:
        // - A map of {route_partition_id, component}
        //     → Since the component already knows about the route partition ID, we can create
        //       this map as soon as the components are ready
        // - Per partition, a map of {downstream_route_partition_id, component}
        //     - For this, we need all downstream route partition IDs (wait on
        //       downstream_route_partitions_ids_ff and wait on the futures in there)
        //     - We also need the {route_partition_id, component} map created in the first step

        hpx::shared_future<std::map<hpx::id_type, Component>> component_by_route_partition{hpx::dataflow(
            hpx::launch::async,
            hpx::unwrapping(
                [](std::vector<RoutePartition>&& route_partitions, std::vector<Component>&& components)
                {
                    lue_hpx_assert(std::size(route_partitions) == std::size(components));

                    std::map<hpx::id_type, Component> result{};
                    Count const nr_partitions{static_cast<Count>(std::size(route_partitions))};

                    for (Index partition_idx = 0; partition_idx < nr_partitions; ++partition_idx)
                    {
                        result[route_partitions[partition_idx].get_id()] = components[partition_idx];
                    }

                    return result;
                }),
            hpx::when_all(route_partitions.begin(), route_partitions.end()),
            hpx::when_all(components.begin(), components.end()))};

        hpx::future<std::vector<DownstreamRoutePartitionIDs>> down1{
            when_all_get(std::move(downstream_route_partitions_ids_ff))};

        hpx::future<Array<std::vector<hpx::id_type>, rank>> down2{down1.then(
            hpx::unwrapping(
                [shape_in_partitions =
                     route_partitions.shape()](std::vector<DownstreamRoutePartitionIDs>&& down1)
                {
                    Array<hpx::future<std::vector<hpx::id_type>>, rank> result{shape_in_partitions};

                    std::transform(
                        down1.begin(),
                        down1.end(),
                        result.begin(),
                        [](DownstreamRoutePartitionIDs& down) { return when_all_get(std::move(down)); });

                    return when_all_get(std::move(result));
                }))};

        hpx::future<void> components_configured{hpx::dataflow(
            hpx::launch::async,
            hpx::unwrapping(
                [](std::vector<Component>&& components,
                   std::map<hpx::id_type, Component> const& component_by_route_partition,
                   Array<std::vector<hpx::id_type>, rank>&& downstream_partition_ids)
                {
                    lue_hpx_assert(
                        static_cast<Count>(std::size(components)) ==
                        nr_elements(downstream_partition_ids.shape()));

                    Count const nr_partitions{static_cast<Count>(std::size(components))};
                    std::vector<hpx::future<void>> components_configured(nr_partitions);

                    for (Index partition_idx = 0; partition_idx < nr_partitions; ++partition_idx)
                    {
                        std::map<hpx::id_type, Component> component_map{};

                        for (hpx::id_type const& downstream_partition_id :
                             downstream_partition_ids[partition_idx])
                        {
                            lue_hpx_assert(
                                component_by_route_partition.find(downstream_partition_id) !=
                                component_by_route_partition.end());

                            component_map[downstream_partition_id] =
                                component_by_route_partition.at(downstream_partition_id);
                        }

                        components_configured[partition_idx] =
                            components[partition_idx].set_downstream_components(std::move(component_map));
                    }

                    return hpx::future<void>{
                        hpx::when_all(components_configured.begin(), components_configured.end())};
                }),
            hpx::when_all(components.begin(), components.end()),
            component_by_route_partition,
            down2)};

        using RouteStarts = typename Route::Starts;

        hpx::future<void> walks_started{hpx::dataflow(
            hpx::launch::async,
            []([[maybe_unused]] hpx::future<void>&& components_configured_f,
               hpx::shared_future<RouteStarts> const& route_starts_f,
               hpx::future<Data>&& data_f,
               hpx::shared_future<std::map<hpx::id_type, Component>>&& component_by_route_partition_f)
            {
                RouteStarts const& route_starts{route_starts_f.get()};
                Data data{data_f.get()};
                std::map<hpx::id_type, Component> const& component_by_route_partition{
                    component_by_route_partition_f.get()};

                std::vector<hpx::future<void>> walk_started{};
                walk_started.reserve(std::size(route_starts));

                // Iterate over all route starts
                // - Obtain the component handling the start fragment of the route
                // - Start the walk for the given route ID

                for (auto const& [route_id, fragment_location] : route_starts)
                {
                    lue_hpx_assert(fragment_location.valid());
                    lue_hpx_assert(fragment_location.is_ready());

                    hpx::id_type const route_partition_id{fragment_location.get()};

                    lue_hpx_assert(
                        component_by_route_partition.find(route_partition_id) !=
                        component_by_route_partition.end());

                    Component const& component{component_by_route_partition.at(route_partition_id)};

                    walk_started.push_back(component.walk(route_id, data));
                }

                return hpx::future<void>{hpx::when_all(walk_started.begin(), walk_started.end())};
            },
            components_configured,
            route.starts(),
            data,
            component_by_route_partition)};

        // Note: this future will be finished when all walks have started. Whether or not walks
        // have finished is something else. Use it to start showing a progress bar, for
        // example. Or just let it go out of scope.
        return walks_started;
    }


    /*!
        @overload
    */
    template<typename RouteID, Rank rank, typename Component, typename Data>
    hpx::future<void> walk(
        SerialRoute<RouteID, rank> const& route, Array<Component, rank>& components, Data&& data)
    {
        return walk(route, components, hpx::make_ready_future<Data>(std::forward<Data>(data)));
    }

}  // namespace lue
