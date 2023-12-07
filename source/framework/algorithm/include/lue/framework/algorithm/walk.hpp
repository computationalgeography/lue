#pragma once
#include "lue/framework/partitioned_array.hpp"
#include "lue/framework/serial_route.hpp"
#include <hpx/include/components.hpp>
#include <map>
#include <vector>


namespace lue {
    namespace detail {

    }  // namespace detail


    template<template<typename> typename Collection, template<typename> typename Future, typename Element>
    hpx::future<Collection<Element>> when_all_get(Collection<Future<Element>>&& futures)
    {
        // Given a collection of futures to elements, wait for all futures to become ready
        // and return a collection with the elements originally pointed to by the futures.

        return hpx::when_all(futures.begin(), futures.end())
            .then(hpx::unwrapping(
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
            .then(hpx::unwrapping(
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


    template<Rank rank, typename Component, typename Data>
    hpx::future<void> walk(SerialRoute<rank> const& route, Array<Component, rank>& components, Data&& data)
    {
        // For each route:
        // - Visit all cells untill some condition is false
        //     - Pass in a copy of the data passed in

        // Algorithm:
        // - Wait for all components to be ready
        // - Create a map of route partition ID → walk component
        // - Per partition create a map with downstream route partition IDs → walk component
        // - Pass partition-specific map to walk component
        // - Initiate each route's walk by calling visit on the walk component containing the
        //   route's start fragment. The visit must continue from there on, for as long as some
        //   condition is true.

        using Route = SerialRoute<rank>;
        // using RouteID = typename Route::RouteID;
        using RoutePartition = PartitionT2<Route>;
        using DownstreamRoutePartitionIDs = std::vector<hpx::shared_future<hpx::id_type>>;

        auto const& route_partitions{route.partitions()};
        Count const nr_partitions{lue::nr_elements(route_partitions)};

        std::vector<hpx::future<DownstreamRoutePartitionIDs>> downstream_route_partitions_ids_ff(
            nr_partitions);

        // Per partition, obtain a future to the collection of downstream route partition IDs
        for (Index p = 0; p < nr_partitions; ++p)
        {
            downstream_route_partitions_ids_ff[p] = hpx::dataflow(
                hpx::launch::async,
                [](RoutePartition const& route_partition)
                { return route_partition.remote_route_fragment_locations(); },
                route_partitions[p]);
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

                    for (Index p = 0; p < nr_partitions; ++p)
                    {
                        result[route_partitions[p].get_id()] = components[p];
                    }

                    return result;
                }),
            hpx::when_all(route_partitions.begin(), route_partitions.end()),
            hpx::when_all(components.begin(), components.end()))};

        hpx::future<std::vector<DownstreamRoutePartitionIDs>> down1{
            when_all_get(std::move(downstream_route_partitions_ids_ff))};

        hpx::future<Array<std::vector<hpx::id_type>, rank>> down2{down1.then(hpx::unwrapping(
            [shape_in_partitions = route_partitions.shape()](std::vector<DownstreamRoutePartitionIDs>&& down1)
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
                [](
                    /// std::vector<RoutePartition>&& route_partitions,
                    std::vector<Component>&& components,
                    std::map<hpx::id_type, Component> const& component_by_route_partition,
                    Array<std::vector<hpx::id_type>, rank>&& downstream_partition_ids)
                {
                    lue_hpx_assert(
                        static_cast<Count>(std::size(components)) ==
                        nr_elements(downstream_partition_ids.shape()));

                    Count const nr_partitions{static_cast<Count>(std::size(components))};
                    std::vector<hpx::future<void>> components_configured(nr_partitions);

                    for (Index p = 0; p < nr_partitions; ++p)
                    {


                        /// std::map<hpx::id_type, std::vector<Component>> component_map{};

                        /// for(hpx::id_type const& downstream_partition_id: downstream_partition_ids[p])
                        /// {
                        ///     lue_hpx_assert(component_by_route_partition.find(downstream_partition_id) !=
                        ///     component_by_route_partition.end());

                        ///     component_map[route_partitions[p].get_id()].push_back(component_by_route_partition.at(downstream_partition_id));
                        /// }

                        /// components_configured[p] =
                        /// components[p].set_downstream_components(std::move(component_map));


                        std::map<hpx::id_type, Component> component_map{};

                        for (hpx::id_type const& downstream_partition_id : downstream_partition_ids[p])
                        {
                            lue_hpx_assert(
                                component_by_route_partition.find(downstream_partition_id) !=
                                component_by_route_partition.end());

                            component_map[downstream_partition_id] =
                                component_by_route_partition.at(downstream_partition_id);
                        }

                        components_configured[p] =
                            components[p].set_downstream_components(std::move(component_map));
                    }

                    return hpx::future<void>{
                        hpx::when_all(components_configured.begin(), components_configured.end())};
                }),
            /// hpx::when_all(route_partitions.begin(), route_partitions.end()),
            hpx::when_all(components.begin(), components.end()),
            component_by_route_partition,
            down2)};

        using RouteStarts = typename Route::Starts;

        hpx::future<void> walks_started{hpx::dataflow(
            hpx::launch::async,
            [data = std::move(data)](
                [[maybe_unused]] hpx::future<void>&& components_configured_f,
                hpx::shared_future<RouteStarts> const& route_starts_f,
                hpx::shared_future<std::map<hpx::id_type, Component>>&& component_by_route_partition_f)
            {
                RouteStarts const& route_starts{route_starts_f.get()};
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
            component_by_route_partition)};

        // Note: this future will be finished when all walks have started. Whether or not walks
        // have finished is something else. Use it to start showing a progress bar, for
        // example. Or just let it go out of scope.
        return walks_started;
    }

}  // namespace lue


/// namespace lue {
///     namespace detail {
///
///         using RouteID = Index;
///
///         using RoutePartitionID = hpx::id_type;
///
///         using WalkComponentID = hpx::id_type;
///
///         using PartitionIdx = Index;
///
///         using RoutePartitionData = std::tuple<WalkComponentID, PartitionIdx>;
///
///         using RoutePartitionMap = std::map<RoutePartitionID, RoutePartitionData>;
///
///         template<Rank rank, typename Functor>
///         class Walk;
///
///
///         namespace server {
///
///             template<Rank rank, typename Functor>
///             class Walk: public hpx::components::component_base<Walk<rank, Functor>>
///             {
///
///                 private:
///
///                     using ComponentServerBase = hpx::components::component_base<Walk<rank, Functor>>;
///                     using WalkClient = detail::Walk<rank, Functor>;
///
///
///                 public:
///
///                     Walk(Functor const& functor):
///
///                         ComponentServerBase{},
///                         _functor{functor},
///                         _downstream_components{}
///
///                     {
///                     }
///
///
///                     Walk(Functor&& functor):
///
///                         ComponentServerBase{},
///                         _functor{std::move(functor)},
///                         _downstream_components{}
///
///                     {
///                     }
///
///
///                     void set_downstream_components(std::map<hpx::id_type, std::vector<WalkClient>>&&
///                     components)
///                     {
///                         _downstream_components = components;
///                     }
///
///
///                     void walk(RouteID const route_id, Functor const& functor)
///                     {
///                         // TODO
///                     }
///
///
///                     HPX_DEFINE_COMPONENT_ACTION(Walk, set_downstream_components,
///                     SetDownstreamComponentsAction);
///
///                     HPX_DEFINE_COMPONENT_ACTION(Walk, set_downstream_components, WalkAction);
///
///                 private:
///
///                     Functor _functor;
///
///                     std::map<hpx::id_type, std::vector<WalkClient>> _downstream_components;
///
///             };
///
///         }  // namespace server
///
///
///         template<Rank rank, typename Functor>
///         class Walk: public hpx::components::client_base<Walk<rank, Functor>, server::Walk<rank, Functor>>
///
///         {
///
///             private:
///
///                 using ComponentServer = server::Walk<rank, Functor>;
///                 using ComponentClientBase =
///                     hpx::components::client_base<Walk<rank, Functor>, ComponentServer>;
///
///             public:
///
///                 Walk():
///
///                     ComponentClientBase{}
///
///                 {
///                 }
///
///
///                 Walk(hpx::id_type const component_id):
///
///                     ComponentClientBase{component_id}
///
///                 {
///                 }
///
///
///                 Walk(hpx::future<hpx::id_type>&& component_id):
///
///                     ComponentClientBase{std::move(component_id)}
///
///                 {
///                 }
///
///
///                 Walk(Walk const&) = default;
///
///                 Walk(Walk&&) = default;
///
///                 ~Walk() = default;
///
///                 Walk& operator=(Walk const&) = default;
///
///                 Walk& operator=(Walk&&) = default;
///
///
///                 hpx::future<void> set_downstream_components(std::map<hpx::id_type, std::vector<Walk>>&&
///                 components)
///                 {
///                     lue_hpx_assert(this->is_ready());
///                     lue_hpx_assert(this->get_id());
///
///                     typename ComponentServer::SetDownstreamComponentsAction action;
///
///                     return hpx::async(action, this->get_id(), std::move(components));
///                 }
///
///
///                 hpx::future<void> walk(RouteID const route_id, Functor const& functor)
///                 {
///                     lue_hpx_assert(this->is_ready());
///                     lue_hpx_assert(this->get_id());
///
///                     typename ComponentServer::WalkAction action;
///
///                     return hpx::async(action, this->get_id(), functor);
///                 }
///
///         };
///
///     }  // namespace detail


/// /*!
///     @brief      Return a future to an array with localities to the components passed in
/// */
/// template<
///     typename Component,
///     Rank rank>
/// hpx::future<Array<hpx::id_type, rank>> localities(
///     Array<Component, rank> const& components)
/// {
///     using LocalityFutures = Array<hpx::future<hpx::id_type>, rank>;
///     using Localities = Array<hpx::id_type, rank>;

///     auto const shape{components.shape()};

///     LocalityFutures localities_f{shape};

///     Count const nr_components{lue::nr_elements(components)};

///     for(Index idx = 0; idx < nr_components; ++idx)
///     {
///         localities_f[idx] = hpx::async(hpx::launch::async,
///             [](Component const& component)
///             {
///                 return hpx::get_colocation_id(component.get_id());
///             },
///             components[idx]);
///     }

///     return hpx::when_all(localities_f.begin(), localities_f.end()).then(
///         [shape](auto&& localities_ff)
///         {
///             auto localities_f{localities_ff.get()};
///             Localities localities{shape};

///             std::transform(localities_f.begin(), localities_f.end(), localities.begin(),
///                 [](auto& locality_f)
///                 {
///                     return locality_f.get();
///                 });

///             return localities;
///         });
/// }


// template<template<typename> typename Collection, template<typename> typename Future, typename Element>
// hpx::future<Collection<Element>> when_all_get(
//     Collection<Future<Element>>&& futures)
// {
//     // Given a collection of futures to elements, wait for all futures to become ready
//     // and return a collection with the elements originally pointed to by the futures.

//     return hpx::when_all(futures.begin(), futures.end()).then(
//         hpx::unwrapping(
//             [](std::vector<Future<Element>>&& futures)
//             {
//                 Collection<Element> elements(std::size(futures));

//                 std::transform(futures.begin(), futures.end(), elements.begin(),
//                     [](Future<Element>& future) { return future.get(); });

//                 return elements;
//             }));
// }


///     template<Rank rank, typename Functor>
///     void walk(SerialRoute<rank> const& route, Functor&& functor)
///     {
///         // Input:
///         // - Partitioned serial route containing zero or more routes
///
///         // Goal:
///         // - Visit each route, passing in a copy of the functor
///
///         // Challenge:
///         // - Route fragments in route partitions refer to downstream route fragments in other
///         //   route partitions. Here we use Walk components to do the walking. We create a
///         //   component for each route partition. We need to do some preprocessing to be able
///         //   to map each route partition to a corresponding walk partition. Also, while walking,
///         //   a linear partition idx is used to tell the functor which partition is currently visited.
///
///         // Algorithm:
///         //
///         // - Iterate over each route partition and determine which downstream route partitions are
///         //   referred to by its route fragments. If this collection is not empty, create a
///         //   walk component. Return ([downstream_route_partition_id], walk_component). If the partition
///         //   ID collection is empty, walk_component is not valid. Don't use it.
///         //
///         // - Create a map between {partition_id, walk_component}
///         //
///         // - Iterate over each partition and send a {downstream_partition_id, walk_component} map to it
///         //
///         // - Iterate over each route start and start visiting all its route fragments. Keep
///         //   track of each fragment visited. In case all routes of a partition are visited, the
///         //   functor can be told to finish it off (set the corresponding partition promise's
///         //   value to the corresponding _tmp_result partition).
///
///         using DownstreamRoutePartitionIDs = std::vector<hpx::shared_future<hpx::id_type>>;
///         using WalkComponent = detail::Walk<rank, Functor>;
///
///         auto const& route_partitions{route.partitions()};
///         Count const nr_partitions{lue::nr_elements(route_partitions)};
///
///         std::vector<hpx::future<hpx::id_type>> route_partition_ids_f(nr_partitions);
///         std::vector<hpx::future<DownstreamRoutePartitionIDs>>
///         downstream_route_partitions_ids_f(nr_partitions); std::vector<hpx::future<WalkComponent>>
///         walk_components_f(nr_partitions);
///
///         for (Index p = 0; p < nr_partitions; ++p)
///         {
///             hpx::tie(route_partition_ids_f[p], downstream_route_partitions_ids_f[p], walk_components_f[p])
///             =
///                 hpx::split_future(hpx::dataflow(
///                     hpx::launch::async,
///                     [functor=std::move(functor)](auto const& route_partition) mutable
///                     {
///                         hpx::future<hpx::id_type> locality_f{
///                             hpx::get_colocation_id(route_partition.get_id())};
///                         hpx::id_type const route_partition_id{route_partition.get_id()};
///
///                         hpx::future<DownstreamRoutePartitionIDs> downstream_route_partitions_ids_f{};
///                         hpx::future<WalkComponent> walk_component_f{};
///
///                         // Once partition is ready, obtain collection with downstream fragment locations
///                         // and wait for the collection to arrive
///
///                         hpx::tie(downstream_route_partitions_ids_f, walk_component_f) =
///                             hpx::split_future(hpx::dataflow(
///                                 hpx::launch::async,
///                                 hpx::unwrapping(
///                                     [functor=std::move(functor)](auto const&
///                                     downstream_fragment_locations, hpx::id_type const locality)
///                                     {
///                                         DownstreamRoutePartitionIDs downstream_route_partition_ids(
///                                             downstream_fragment_locations.size());
///
///                                         std::transform(
///                                             downstream_fragment_locations.begin(),
///                                             downstream_fragment_locations.end(),
///                                             downstream_route_partition_ids.begin(),
///                                             [](auto const& fragment_location)
///                                             { return std::get<0>(fragment_location); });
///
///                                         /// // If the partition contains route fragments that continue in
///                                         /// // other partitions
///                                         /// if (!downstream_route_partition_ids.empty())
///                                         /// {
///                                             // Create Walk component
///                                             WalkComponent walk_component =
///                                             hpx::new_<WalkComponent>(locality, functor);
///                                         /// }
///
///                                         return hpx::make_tuple(
///                                             std::move(downstream_route_partition_ids),
///                                             std::move(walk_component));
///                                     }),
///                                 route_partition.downstream_route_fragment_locations(),
///                                 locality_f));
///
///                         return hpx::make_tuple(
///                             route_partition_id,
///                             std::move(downstream_route_partitions_ids_f),
///                             std::move(walk_component_f));
///                     },
///                     route_partitions[p]));
///         }
///
///         // What we have now:
///         // std::vector<hpx::future<hpx::id_type>> route_partition_ids_f
///         // → Futures to the IDs of the route partitions we just iterated over
///         //
///         // std::vector<hpx::future<DownstreamRoutePartitionIDs>> downstream_route_partitions_ids_f
///         // → For each route partition, a future to the IDs of the downstream route partitions
///         //   referred to by route fragments
///         //
///         // std::vector<hpx::future<WalkComponent>> walk_components_f
///         // → Futures to Walk components. These are only valid for route partitions containing
///         //   route fragments.
///
///         // What we want now to be able to reach our goal:
///         // - Create a map between {route_partition_id, walk_component}
///         //     → Note that to be able to create this map, we have to wait for all route
///         //       partitions and their associated walk components to be ready. This means that
///         //       it is not need to wait on these in future steps: components wrap a
///         //       shared_future, sharing the same state (the component ID).
///         //     - For this, we need all route partition IDs (wait on route_partition_ids_f)
///         //     - We also need the walk components (wait on walk_components_f)
///         //
///         // - Create a map between {downstream_route_partition_id, walk_component}, per partition
///         //     - For this, we need all downstream route partition IDs (wait on
///         downstream_route_partitions_ids_f)
///         //     - We also need the {route_partition_id, walk_component} map created in the first step
///
///         // std::vector<hpx::future<hpx::id_type>> → hpx::future<std::vector<hpx::id_type>>
///         hpx::future<std::vector<hpx::id_type>> route_partition_ids =
///         when_all_get(std::move(route_partition_ids_f));
///
///         // std::vector<hpx::future<std::vector<hpx::shared_future<hpx::id_type>>>> →
///         hpx::future<std::vector<std::vector<hpx::id_type>>>
///         hpx::future<std::vector<std::vector<hpx::id_type>>> downstream_route_partitions_ids =
///         hpx::when_all(downstream_route_partitions_ids_f.begin(),
///         downstream_route_partitions_ids_f.end()).then(
///             hpx::unwrapping(
///                 [](std::vector<hpx::future<std::vector<hpx::shared_future<hpx::id_type>>>>&&
///                 downstream_route_partitions_ids_f)
///                 {
///                     Count const
///                     nr_partitions{static_cast<Count>(std::size(downstream_route_partitions_ids_f))};
///
///                     std::vector<std::vector<hpx::shared_future<hpx::id_type>>>
///                     downstream_route_partitions_ids(nr_partitions);
///
///                     std::transform(downstream_route_partitions_ids_f.begin(),
///                     downstream_route_partitions_ids_f.end(), downstream_route_partitions_ids.begin(),
///                         [](hpx::future<std::vector<hpx::shared_future<hpx::id_type>>>&
///                         downstream_route_partition_ids)
///                         {
///                             return downstream_route_partition_ids.get();
///                         });
///
///                     std::vector<hpx::future<std::vector<hpx::id_type>>>
///                     downstream_route_partitions_ids2(nr_partitions);
///
///                     for(Index p = 0; p < nr_partitions; ++p)
///                     {
///                         std::vector<hpx::shared_future<hpx::id_type>>&
///                         downstream_route_partition_ids{downstream_route_partitions_ids[p]};
///
///                         downstream_route_partitions_ids2[p] =
///                         when_all_get(std::move(downstream_route_partition_ids));
///                     }
///
///                     hpx::future<std::vector<std::vector<hpx::id_type>>> downstream_route_partitions_ids3 =
///                     when_all_get(std::move(downstream_route_partitions_ids2));
///
///                     return downstream_route_partitions_ids3;
///                 }));
///
///         // std::vector<hpx::future<WalkComponent>> → hpx::future<std::vector<WalkComponent>>
///         hpx::shared_future<std::vector<WalkComponent>> walk_components =
///         when_all_get(std::move(walk_components_f)).share();
///
///         // What we have now:
///         // - hpx::future<std::vector<hpx::id_type>> route_partition_ids
///         // - hpx::future<std::vector<std::vector<hpx::id_type>> downstream_route_partitions_ids
///         // - hpx::shared_future<std::vector<WalkComponent>> walk_components
///
///         // Wait on these and then we have everything we need
///
///         hpx::future<std::vector<hpx::future<void>>> walk_components_ready_f{};
///         hpx::future<std::map<hpx::id_type, WalkComponent>> walk_components_map_f{};
///
///         hpx::tie(walk_components_ready_f, walk_components_map_f) =
///         hpx::split_future(hpx::dataflow(hpx::launch::async,
///             hpx::unwrapping(
///                 [](
///                     std::vector<hpx::id_type> const& route_partition_ids,
///                     std::vector<std::vector<hpx::id_type>> const& downstream_route_partitions_ids,
///                     std::vector<WalkComponent> walk_components)
///                 {
///                     lue_hpx_assert(route_partition_ids.size() == downstream_route_partitions_ids.size());
///                     lue_hpx_assert(route_partition_ids.size() == walk_components.size());
///
///                     std::map<hpx::id_type, WalkComponent> walk_components_map{};
///
///                     Count const nr_partitions{static_cast<Count>(std::size(route_partition_ids))};
///
///                     for(Index p = 0; p < nr_partitions; ++p)
///                     {
///                         walk_components_map[route_partition_ids[p]] = walk_components[p];
///                     }
///
///                     std::vector<hpx::future<void>> walk_components_ready(nr_partitions);
///
///                     for(Index p = 0; p < nr_partitions; ++p)
///                     {
///                         std::map<hpx::id_type, std::vector<WalkComponent>>
///                         partition_walk_components_map{};
///
///                         for(hpx::id_type const& partition_id: downstream_route_partitions_ids[p])
///                         {
///                             partition_walk_components_map[partition_id].push_back(walk_components_map[partition_id]);
///                         }
///
///                         walk_components_ready[p] =
///                         walk_components[p].set_downstream_components(std::move(partition_walk_components_map));
///                     }
///
///                     return hpx::make_tuple(std::move(walk_components_ready),
///                     std::move(walk_components_map));
///                 }),
///             route_partition_ids,
///             downstream_route_partitions_ids,
///             walk_components));
///
///
///         // - Iterate over each route start and start visiting all its route fragments. Keep
///         //   track of each fragment visited. In case all routes of a partition are visited, the
///         //   functor can be told to finish it off (set the corresponding partition promise's
///         //   value to the corresponding _tmp_result partition).
///
///         // Start walking the routes
///
///         // Once all components have been set-up, we can start walking the routes
///         hpx::when_all(walk_components_ready_f, walk_components_map_f).then(
///             hpx::unwrapping(
///                 [route](
///                     hpx::tuple<hpx::future<std::vector<hpx::future<void>>>,
///                     hpx::shared_future<std::map<hpx::id_type, WalkComponent>>>&& futures)
///                 {
///                     std::vector<hpx::future<void>> walk_components_ready_f{std::get<0>(futures).get()};
///                     std::map<hpx::id_type, WalkComponent> const&
///                     walk_components_map{std::get<1>(futures).get()};
///
///                     hpx::when_all(walk_components_ready_f.begin(), walk_components_ready_f.end()).then(
///                         [route, walk_components_map=std::move(walk_components_map)](auto&&)
///                         {
///                             for(auto [route_id, start_route_fragment_location]: route.starts())
///                             {
///                                 // Given all the above, it is safe to assume that the fragment location is
///                                 ready
///                                 lue_hpx_assert(std::get<0>(start_route_fragment_location).is_ready());
///
///                                 hpx::id_type
///                                 start_partition_id{std::get<0>(start_route_fragment_location).get()};
///
///                                 /// // This should be the case since we end up here after we already
///                                 /// // used the walk component
///                                 /// lue_hpx_assert(walk_components[p].is_ready());
///
///                                 // // Given the partition-id, we can find the Walk component to start the
///                                 walk in
///                                 // walk_components[start_partition_id].get().walk(route_id)
///                             }
///                         });
///                 }));
///
///
///
///
///
///
///         // Keep the components alive for as long as the result is not ready
///         hpx::dataflow(hpx::launch::async,
///             []([[maybe_unused]] hpx::future<void> const& functor_done,
///             hpx::shared_future<std::vector<WalkComponent>> const& components)
///             {
///                 HPX_UNUSED(components);
///             },
///             functor.then(),
///             walk_components);
///
///
///
///
///
///
///
///
///
///
///
///
///         /// auto meh = hpx::when_all(walk_components_ready_f.begin(), walk_components_ready_f.end()).then(
///         ///     [route](auto&&)
///         ///     {
///         ///         for(auto [route_id, start_route_fragment_location]: route.starts())
///         ///         {
///         ///             // Given all the above, it is safe to assume that the fragment location is ready
///         ///             lue_hpx_assert(std::get<0>(start_route_fragment_location).is_ready());
///
///         ///             hpx::id_type start_partition_id{std::get<0>(start_route_fragment_location).get()};
///
///         ///             // TODO
///         ///             // Given the partition-id, we can find the Walk component to start the walk in
///         ///             // walk_component.walk(route_id, functor)
///
///
///         ///         }
///
///         ///     });
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///
///         /// hpx::when_all(downstream_route_partitions_ids_f.begin(),
///         downstream_route_partitions_ids_f.end())
///         ///     .then(
///         ///         [route_partition_ids_f = std::move(route_partition_ids_f),
///         ///          walk_components_f = std::move(walk_components_f),
///         ///          functor = std::move(functor)](auto&& downstream_route_partition_ids_ff) mutable
///         ///         {
///         ///             auto downstream_route_partitions_ids_f{downstream_route_partition_ids_ff.get()};
///
///         ///             Count const
///         nr_partitions{static_cast<Count>(downstream_route_partitions_ids_f.size())};
///
///         ///             for (Index p = 0; p < nr_partitions; ++p)
///         ///             {
///         ///                 // Futures to IDs of all downstream route partitions referred to by the route
///         ///                 // fragments in the current partitions
///         ///                 DownstreamRoutePartitionIDs downstream_route_partition_ids_f{
///         ///                     downstream_route_partitions_ids_f[p].get()};
///
///         ///                 if (downstream_route_partition_ids_f.empty())
///         ///                 {
///         ///                     // This route partition does not contain any route fragments. We can
///         ///                     // tell the functor to mark result partition(s) as finished.
///         ///                     functor.finish_partition(p);
///         ///                 }
///
///         ///                 downstream_partition_ids[p] =
///         hpx::when_all(downstream_route_partition_ids_f.begin(),
///         downstream_route_partition_ids_f.end()).then(
///         ///                     hpx::unwrapping(
///         ///                         [](DownstreamRoutePartitionIDs&& downstream_route_partition_ids_f)
///         ///                         {
///         ///                             std::vector<hpx::id_type>
///         downstream_route_partition_ids(std::size(downstream_route_partition_ids_f));
///
///         ///                             std::transform(downstream_route_partition_ids_f.begin(),
///         downstream_route_partition_ids_f.end(),
///         ///                                 [](hpx::shared_future<hpx::id_type>& partition_id_f)
///         ///                                 {
///         ///                                     lue_hpx_assert(partition_id_f.valid());
///         ///                                     lue_hpx_assert(partition_id_f.is_ready());
///         ///                                     return partition_id_f.get();
///         ///                                 });
///         ///                         }));
///
///
///
///
///
///
///         ///                 ///     // We have a non-empty collection of downstream route fragment
///         locations. Now
///         ///                 ///     // we can wait for everything we need to get things going
///
///         ///                 ///     // Future to ID of current route partition
///         ///                 ///     hpx::future<hpx::id_type> route_partition_id_f{
///         ///                 ///         std::move(route_partition_ids_f[p])};
///
///         ///                 ///     // Future to Walk component associated with current partition
///         ///                 ///     hpx::future<WalkComponent>
///         walk_component_f{std::move(walk_components_f[p])};
///
///         ///                 ///     /// auto meh = hpx::dataflow(hpx::launch::async,
///         ///                 ///     ///         hpx::unwrapping(
///         ///                 ///     ///             [](
///         ///                 ///     ///                 DownstreamRoutePartitionIDs&&
///         downstream_route_partition_ids_f,
///         ///                 ///     ///                 hpx::id_type const route_partition_id,
///         ///                 ///     ///                 WalkComponent const& walk_component)
///         ///                 ///     ///             {
///         ///                 ///     ///                 std::vector<hpx::id_type>
///         downstream_route_partition_ids(std::size(downstream_route_partition_ids_f));
///
///         ///                 ///     /// std::transform(downstream_route_partition_ids_f.begin(),
///         downstream_route_partition_ids_f.end(),
///         ///                 ///     ///                     [](hpx::shared_future<hpx::id_type>&
///         partition_id_f)
///         ///                 ///     ///                     {
///         ///                 ///     ///                         lue_hpx_assert(partition_id_f.valid());
///         ///                 ///     ///                         lue_hpx_assert(partition_id_f.is_ready());
///         ///                 ///     ///                         return partition_id_f.get();
///         ///                 ///     ///                     });
///
///
///         ///                 ///     ///                 return hpx::make_tuple(route_partition_id,
///         std::move(downstream_route_partition_ids));
///         ///                 ///     ///             }),
///         ///                 ///     ///         hpx::when_all(downstream_route_partition_ids_f.begin(),
///         downstream_route_partition_ids_f.end());
///         ///                 ///     ///         route_partition_id_f,
///         ///                 ///     ///         walk_component_f);
///         ///                 /// }
///         ///             }
///
///
///
///
///         ///             // // For each partition the collection of downstream route partition IDs referred
///         to
///         ///             // std::vector<DownstreamRoutePartitionIDs>>
///         ///             // downstream_route_partition_ids(downstream_route_partition_ids_f.size());
///
///         ///             // std::transform(downstream_route_partition_ids_f.begin(),
///         ///             // downstream_route_partition_ids_f.end(), downstream_route_partition_ids.begin,
///         ///             //     [](auto const& ids_f)
///         ///             //     {
///         ///             //         return ids_f.get();
///         ///             //     });
///
///         ///             // std::vector<DownstreamRoutePartitionIDs>>
///         ///             // downstream_route_partition_ids(downstream_route_partition_ids_f.size());
///         ///         });
///
///
///
///
///
///
///
///         ///                             WalkComponent walk_component{};
///         ///                             std::optional<detail::RoutePartitionData> data{};
///
///         ///                             // If the partition contains route fragments that continue in
///         ///                             // other partitions
///         ///                             if(!downstream_fragment_locations.empty())
///         ///                             {
///         ///                                 // Create Walk component
///         ///                                 walk_component = hpx::new_<WalkComponent>(locality);
///         ///                                 // route_partition_data = std::make_tuple();
///         ///                             }
///
///
///         ///                             //     hpx::new_<Walk>(locality).then(
///         ///                             //         [](Component&& component)
///         ///                             //         {
///         ///                             //             return hpx::make_tuple(hpx::new_<Walk>(locality),
///         ///                             downstream_fragment_locations);
///
///         ///                             //             // Aggregate this information:
///         ///                             //             // - For this walk component ID, the collection of:
///         ///                             //             //     - Downstream route partition ID
///         ///                             //             //     - Downstream route partition linear idx
///         ///                             //             //
///
///
///         ///                             return hpx::make_tuple(walk_component, data);
///
///
///         ///                             //     ///         // detail::RoutePartitionData data{};
///         ///                             //     ///         // std::vector<
///
///         ///                             //     ///         // for(auto const& location:
///         ///                             downstream_fragment_locations)
///         ///                             //     ///         // {
///         ///                             //     ///         //     data[p].push_back(loc
///         ///                             //     ///         // }
///
///
///         ///                             //     ///         // RoutePartition
///         ///                             partition_id_f{component.route_partition()};
///         ///                             //     ///         // hpx::future<MaxValue> max_values_f{
///         ///                             //     ///         //     component.sort_values(policies,
///         ///                             value_partition)};
///
///         ///                             //     ///         // return hpx::make_tuple(
///         ///                             //     ///         //     std::move(component),
///         ///                             //     ///         //     std::move(partition_id_f),
///         ///                             //     ///         //     std::move(max_values_f));
///
///
///         ///                             //             std::vector<
///         ///                             //             std::optional<detail::RoutePartitionData>;
///
///         ///                             //         }
///         ///                             // }
///         ///                         }
///         ///                     ),
///         ///                     route_partition.downstream_route_fragment_locations(),
///         ///                     locality_f));
///
///         ///             return hpx::make_tuple(walk_component, route_partition_data_f);
///
///
///         ///             // Once partition is ready, obtain collection with downstream fragment locations
///         ///             // and wait for the collection to arrive
///
///         ///             auto [walk_component, route_partition_data_f] =
///         ///             hpx::split_future(hpx::dataflow(hpx::launch::async,
///         ///                     hpx::unwrapping(
///         ///                         [p, route_partition_id](
///         ///                             auto const& downstream_fragment_locations,
///         ///                             hpx::id_type const locality)
///         ///                         {
///         ///                             WalkComponent walk_component{};
///         ///                             std::optional<detail::RoutePartitionData> data{};
///
///         ///                             // If the partition contains route fragments that continue in
///         ///                             // other partitions
///         ///                             if(!downstream_fragment_locations.empty())
///         ///                             {
///         ///                                 // Create Walk component
///         ///                                 walk_component = hpx::new_<WalkComponent>(locality);
///         ///                                 // route_partition_data = std::make_tuple();
///         ///                             }
///
///
///         ///                             //     hpx::new_<Walk>(locality).then(
///         ///                             //         [](Component&& component)
///         ///                             //         {
///         ///                             //             return hpx::make_tuple(hpx::new_<Walk>(locality),
///         ///                             downstream_fragment_locations);
///
///         ///                             //             // Aggregate this information:
///         ///                             //             // - For this walk component ID, the collection of:
///         ///                             //             //     - Downstream route partition ID
///         ///                             //             //     - Downstream route partition linear idx
///         ///                             //             //
///
///
///         ///                             return hpx::make_tuple(walk_component, data);
///
///
///         ///                             //     ///         // detail::RoutePartitionData data{};
///         ///                             //     ///         // std::vector<
///
///         ///                             //     ///         // for(auto const& location:
///         ///                             downstream_fragment_locations)
///         ///                             //     ///         // {
///         ///                             //     ///         //     data[p].push_back(loc
///         ///                             //     ///         // }
///
///
///         ///                             //     ///         // RoutePartition
///         ///                             partition_id_f{component.route_partition()};
///         ///                             //     ///         // hpx::future<MaxValue> max_values_f{
///         ///                             //     ///         //     component.sort_values(policies,
///         ///                             value_partition)};
///
///         ///                             //     ///         // return hpx::make_tuple(
///         ///                             //     ///         //     std::move(component),
///         ///                             //     ///         //     std::move(partition_id_f),
///         ///                             //     ///         //     std::move(max_values_f));
///
///
///         ///                             //             std::vector<
///         ///                             //             std::optional<detail::RoutePartitionData>;
///
///         ///                             //         }
///         ///                             // }
///         ///                         }
///         ///                     ),
///         ///                     route_partition.downstream_route_fragment_locations(),
///         ///                     locality_f));
///
///         ///             return hpx::make_tuple(walk_component, route_partition_data_f);
///
///
///         ///             // return route_partition.downstream_route_fragment_locations().then(
///         ///             //     [p, route_partition_id](auto&& downstream_fragment_locations_f)
///         ///             //     {
///         ///             //         auto
///         downstream_fragment_locations{downstream_fragment_locations_f.get()};
///
///         ///             //         // If the partition contains route fragments that continue in
///         ///             //         // other partitions
///         ///             //         if(!downstream_fragment_locations.empty())
///         ///             //         {
///         ///             //             // Create Walk component
///
///         ///             //             hpx::new_<Walk>(hpx::colocated(route_partition_id)).then(
///         ///             //                 [](Component&& component)
///         ///             //                 {
///
///         ///             //             ///         // detail::RoutePartitionData data{};
///         ///             //             ///         // std::vector<
///
///         ///             //             ///         // for(auto const& location:
///         downstream_fragment_locations)
///         ///             //             ///         // {
///         ///             //             ///         //     data[p].push_back(loc
///         ///             //             ///         // }
///
///
///         ///             //             ///         // RoutePartition
///         ///             partition_id_f{component.route_partition()};
///         ///             //             ///         // hpx::future<MaxValue> max_values_f{
///         ///             //             ///         //     component.sort_values(policies,
///         value_partition)};
///
///         ///             //             ///         // return hpx::make_tuple(
///         ///             //             ///         //     std::move(component),
///         ///             //             ///         //     std::move(partition_id_f),
///         ///             //             ///         //     std::move(max_values_f));
///
///
///         ///             //             ///     });
///
///
///         ///             //         }
///         ///             //     });
///
///
///         // future to {route_partition_id: (walk_component_id, partition_idx)}
///
///         // Linear idx is the idx within the vector
///         // Last vector is the collection of downstream route partition ids
///         // Start with collecting this information:
///         // std::vector<std::tuple<current_route_partition_id, walk_component,
///         // std::vector<downstream_route_partition_id>>>
///
///
///         // Then postprocess to map current_route_partition_id to: walk_component,
///         // std::vector<downstream_route_partition_id>, linear_idx
///
///         // Then pass this information to the walk component
///
///
///         // Iterate over all route partitions. For each partition:
///         // - When partition is ready:
///         //     - If it contains route fragments:
///         //         - Create Walk component
///         //         - When walk component is ready:
///         //             - Store mapping between the current / local route partition id and
///         //                 - the corresponding walk component id
///         //                 - linear idx of the current partition
///         //                 - collection of downstream (to the current partition) partition locations
///         //                 → return future to {route_partition_id: (walk_component_id, partition_idx)}
///
///         // When all future {route_partition_id, (walk_component_id, partition_idx)} are ready:
///         // - Iterate over all route partitions. For each partition:
///         //     - Obtain the downstream route partition ids used by the fragments
///         //     - Create subset of map {route_partition_id: (walk_component_id, partition_idx)}
///         //       relevant for the walk component associated with the current partition and send
///         //       it to the walk component. `Walk::set_component_ids(...)`
///
///
///         // Now all required Walk components are created and they know:
///         // - The ID of the downstream Walk components which handle the route partitions
///         // - The idx of the corresponding result partition
///
///
///         // This must be easy to do by the Walk server component:
///         // - void visit(Idx const partition_idx, Idx const cell_idx)
///         //     → partition_idx must be passed in
///         //     → cell_idx can be obtained from the route fragment
///
///
///         /// // Aggregate stuff per route?!
///
///         /// auto const& route_partitions{route.partitions()};
///         /// Count const nr_partitions{lue::nr_elements(route_partitions)};
///         /// std::vector<hpx::future<std::optional<detail::RoutePartitionData>>>
///         /// route_partition_data(nr_partitions); std::vector<WalkComponent>
///         walk_components(nr_partitions);
///
///         /// /// // Obtain localities of all route partitions
///         /// /// auto localities_f = localities(route_partitions);
///
///         /// /// localities_f.then(
///         /// ///         [route_partitions = std::move(route_partitions)](auto&& localities_f)
///         /// ///         {
///         /// ///             auto const localities{localities_f.get()};
///         /// ///         });
///
///
///         /// for (Index p = 0; p < nr_partitions; ++p)
///         /// {
///         ///     std::tie(walk_components[p], route_partition_data[p]) =
///         ///     hpx::split_future(hpx::dataflow(hpx::launch::async,
///         ///         [p](auto const& route_partition)
///         ///         {
///         ///             hpx::future<hpx::id_type>
///         ///             locality_f{hpx::get_colocation_id(route_partition.get_id())}; hpx::id_type const
///         ///             route_partition_id{route_partition.get_id()};
///
///         ///             // Once partition is ready, obtain collection with downstream fragment locations
///         ///             // and wait for the collection to arrive
///
///         ///             auto [walk_component, route_partition_data_f] =
///         ///             hpx::split_future(hpx::dataflow(hpx::launch::async,
///         ///                     hpx::unwrapping(
///         ///                         [p, route_partition_id](
///         ///                             auto const& downstream_fragment_locations,
///         ///                             hpx::id_type const locality)
///         ///                         {
///         ///                             WalkComponent walk_component{};
///         ///                             std::optional<detail::RoutePartitionData> data{};
///
///         ///                             // If the partition contains route fragments that continue in
///         ///                             // other partitions
///         ///                             if(!downstream_fragment_locations.empty())
///         ///                             {
///         ///                                 // Create Walk component
///         ///                                 walk_component = hpx::new_<WalkComponent>(locality);
///         ///                                 // route_partition_data = std::make_tuple();
///         ///                             }
///
///
///         ///                             //     hpx::new_<Walk>(locality).then(
///         ///                             //         [](Component&& component)
///         ///                             //         {
///         ///                             //             return hpx::make_tuple(hpx::new_<Walk>(locality),
///         ///                             downstream_fragment_locations);
///
///         ///                             //             // Aggregate this information:
///         ///                             //             // - For this walk component ID, the collection of:
///         ///                             //             //     - Downstream route partition ID
///         ///                             //             //     - Downstream route partition linear idx
///         ///                             //             //
///
///
///         ///                             return hpx::make_tuple(walk_component, data);
///
///
///         ///                             //     ///         // detail::RoutePartitionData data{};
///         ///                             //     ///         // std::vector<
///
///         ///                             //     ///         // for(auto const& location:
///         ///                             downstream_fragment_locations)
///         ///                             //     ///         // {
///         ///                             //     ///         //     data[p].push_back(loc
///         ///                             //     ///         // }
///
///
///         ///                             //     ///         // RoutePartition
///         ///                             partition_id_f{component.route_partition()};
///         ///                             //     ///         // hpx::future<MaxValue> max_values_f{
///         ///                             //     ///         //     component.sort_values(policies,
///         ///                             value_partition)};
///
///         ///                             //     ///         // return hpx::make_tuple(
///         ///                             //     ///         //     std::move(component),
///         ///                             //     ///         //     std::move(partition_id_f),
///         ///                             //     ///         //     std::move(max_values_f));
///
///
///         ///                             //             std::vector<
///         ///                             //             std::optional<detail::RoutePartitionData>;
///
///         ///                             //         }
///         ///                             // }
///         ///                         }
///         ///                     ),
///         ///                     route_partition.downstream_route_fragment_locations(),
///         ///                     locality_f));
///
///         ///             return hpx::make_tuple(walk_component, route_partition_data_f);
///
///
///         ///             // return route_partition.downstream_route_fragment_locations().then(
///         ///             //     [p, route_partition_id](auto&& downstream_fragment_locations_f)
///         ///             //     {
///         ///             //         auto
///         downstream_fragment_locations{downstream_fragment_locations_f.get()};
///
///         ///             //         // If the partition contains route fragments that continue in
///         ///             //         // other partitions
///         ///             //         if(!downstream_fragment_locations.empty())
///         ///             //         {
///         ///             //             // Create Walk component
///
///         ///             //             hpx::new_<Walk>(hpx::colocated(route_partition_id)).then(
///         ///             //                 [](Component&& component)
///         ///             //                 {
///
///         ///             //             ///         // detail::RoutePartitionData data{};
///         ///             //             ///         // std::vector<
///
///         ///             //             ///         // for(auto const& location:
///         downstream_fragment_locations)
///         ///             //             ///         // {
///         ///             //             ///         //     data[p].push_back(loc
///         ///             //             ///         // }
///
///
///         ///             //             ///         // RoutePartition
///         ///             partition_id_f{component.route_partition()};
///         ///             //             ///         // hpx::future<MaxValue> max_values_f{
///         ///             //             ///         //     component.sort_values(policies,
///         value_partition)};
///
///         ///             //             ///         // return hpx::make_tuple(
///         ///             //             ///         //     std::move(component),
///         ///             //             ///         //     std::move(partition_id_f),
///         ///             //             ///         //     std::move(max_values_f));
///
///
///         ///             //             ///     });
///
///
///         ///             //         }
///         ///             //     });
///         ///         },
///         ///         route_partitions[p]));
///         /// }
///     }

/// }  // namespace lue


// #define LUE_REGISTER_WALK_ACTIONS(rank, Functor, FunctorName)                                                \
//                                                                                                              \
//     using WalkServer = hpx::components::component<lue::detail::server::Walk<rank, Functor>>;                 \
//                                                                                                              \
//     HPX_REGISTER_COMPONENT(WalkServer, Walk_##rank##_##FunctorName)                                          \
//                                                                                                              \
//     HPX_REGISTER_ACTION_DECLARATION(                                                                         \
//         WalkServer::SetDownstreamComponentsAction,                                                               \
//         Walk_##rank##_##FunctorName_SetDownstreamComponentsAction)                                               \
//                                                                                                              \
//     HPX_REGISTER_ACTION_DECLARATION(                                                                         \
//         WalkServer::WalkAction,                                                               \
//         Walk_##rank##_##FunctorName_WalkAction)                                               \


// #define LUE_INSTANTIATE_WALK(Policies)
//
//     template LUE_ROUTING_OPERATION_EXPORT void walk<ArgumentType<void(Policies)>, 2>(
//         ArgumentType<void(Policies)> const&,
//         SerialRoute<2> const&,
//         Functor&&);
