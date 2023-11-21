#pragma once
#include "lue/framework/partitioned_array.hpp"
#include "lue/framework/serial_route.hpp"
#include <map>


namespace lue {

    namespace detail {

        using RoutePartitionID = hpx::id_type;

        using WalkComponentID = hpx::id_type;

        using PartitionIdx = Index;

        using RoutePartitionData = std::tuple<WalkComponentID, PartitionIdx>;

        using RoutePartitionMap = std::map<RoutePartitionID, RoutePartitionData>;


        namespace server {

            template<Rank rank, typename Functor>
            class Walk: public hpx::components::component_base<Walk<rank, Functor>>
            {

                private:

                    using ComponentServerBase = hpx::components::component_base<Walk<rank, Functor>>;


                public:

                    Walk():

                        ComponentServerBase{}

                    {
                    }

                private:
            };

        }  // namespace server


        template<Rank rank, typename Functor>
        class Walk: public hpx::components::client_base<Walk<rank, Functor>, server::Walk<rank, Functor>>

        {

            private:

                using ComponentServer = server::Walk<rank, Functor>;
                using ComponentClientBase =
                    hpx::components::client_base<Walk<rank, Functor>, ComponentServer>;

            public:

                Walk():

                    ComponentClientBase{}

                {
                }


                Walk(hpx::id_type const component_id):

                    ComponentClientBase{component_id}

                {
                }


                Walk(hpx::future<hpx::id_type>&& component_id):

                    ComponentClientBase{std::move(component_id)}

                {
                }


                Walk(Walk const&) = default;

                Walk(Walk&&) = default;

                ~Walk() = default;

                Walk& operator=(Walk const&) = default;

                Walk& operator=(Walk&&) = default;
        };

    }  // namespace detail


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


    template<Rank rank, typename Functor>
    void walk(SerialRoute<rank> const& route, Functor&& functor)
    {
        // Input:
        // - Partitioned serial route containing zero or more routes

        // Goal:
        // - Visit each route, passing in a copy of the functor

        // Challenge:
        // - Route fragments in route partitions refer to downstream route fragments in other
        //   route partitions. Here we use Walk components to do the walking. We create a
        //   component for each route partition. We need to do some preprocessing to be able
        //   to map each route partition to a corresponding walk partition. Also, while walking,
        //   a linear partition idx is used to tell the functor which partition is currently visited.

        // Algorithm:
        //
        // - Iterate over each route partition and determine which remote route partitions are
        //   referred to by its route fragments. If this collection is not empty, create a
        //   walk component. Return ([remote_route_partition_id], walk_component). If the partition
        //   ID collection is empty, walk_component is not valid. Don't use it.
        //
        // - Create a map between {partition_id, walk_component}
        //
        // - Iterate over each partition and send a {remote_partition_id, walk_component} map to it
        //
        // - Iterate over each route start and start visiting all its route fragments. Keep
        //   track of each fragment visited. In case all routes of a partition are visited, the
        //   functor can be told to finish it off (set the corresponding partition promise's
        //   value to the corresponding _tmp_result partition).

        using RemoteRoutePartitionIDs = std::vector<hpx::shared_future<hpx::id_type>>;
        using WalkComponent = detail::Walk<rank, Functor>;

        auto const& route_partitions{route.partitions()};
        Count const nr_partitions{lue::nr_elements(route_partitions)};

        std::vector<hpx::future<hpx::id_type>> route_partition_ids_f(nr_partitions);
        std::vector<hpx::future<RemoteRoutePartitionIDs>> remote_route_partitions_ids_f(nr_partitions);
        std::vector<hpx::future<WalkComponent>> walk_components_f(nr_partitions);

        for (Index p = 0; p < nr_partitions; ++p)
        {
            hpx::tie(route_partition_ids_f[p], remote_route_partitions_ids_f[p], walk_components_f[p]) =
                hpx::split_future(hpx::dataflow(
                    hpx::launch::async,
                    [](auto const& route_partition)
                    {
                        hpx::future<hpx::id_type> locality_f{
                            hpx::get_colocation_id(route_partition.get_id())};
                        hpx::id_type const route_partition_id{route_partition.get_id()};

                        hpx::future<RemoteRoutePartitionIDs> remote_route_partitions_ids_f{};
                        hpx::future<WalkComponent> walk_component_f{};

                        // Once partition is ready, obtain collection with remote fragment locations
                        // and wait for the collection to arrive

                        hpx::tie(remote_route_partitions_ids_f, walk_component_f) =
                            hpx::split_future(hpx::dataflow(
                                hpx::launch::async,
                                hpx::unwrapping(
                                    [](auto const& remote_fragment_locations, hpx::id_type const locality)
                                    {
                                        RemoteRoutePartitionIDs remote_route_partition_ids(
                                            remote_fragment_locations.size());
                                        WalkComponent walk_component{};

                                        std::transform(
                                            remote_fragment_locations.begin(),
                                            remote_fragment_locations.end(),
                                            remote_route_partition_ids.begin(),
                                            [](auto const& fragment_location)
                                            { return std::get<0>(fragment_location); });

                                        // If the partition contains route fragments that continue in
                                        // other partitions
                                        if (!remote_route_partition_ids.empty())
                                        {
                                            // Create Walk component
                                            walk_component = hpx::new_<WalkComponent>(locality);
                                        }

                                        return hpx::make_tuple(
                                            std::move(remote_route_partition_ids), std::move(walk_component));
                                    }),
                                route_partition.remote_route_fragment_locations(),
                                locality_f));

                        return hpx::make_tuple(
                            route_partition_id,
                            std::move(remote_route_partitions_ids_f),
                            std::move(walk_component_f));
                    },
                    route_partitions[p]));
        }

        // What we have now:
        // std::vector<hpx::future<hpx::id_type>> route_partition_ids_f
        // → Futures to the IDs of the route partitions we just iterated over
        //
        // std::vector<hpx::future<RemoteRoutePartitionIDs>> remote_route_partitions_ids_f
        // → For each route partition, a future to the IDs of the remote route partitions
        //   referred to by route fragments
        //
        // std::vector<hpx::future<WalkComponent>> walk_components_f
        // → Futures to Walk components. These are only valid for route partitions containing
        //   route fragments.

        // We don't need to wait for anything related to partitions that do not contain
        // route fragments. Therefore it makes sense to first wait on the remote route partition
        // ID collections. Once these are obtained we can possibly process a subset of the
        // partitions. Other partitions can be marked as finished immediately.


        hpx::when_all(remote_route_partitions_ids_f.begin(), remote_route_partitions_ids_f.end())
            .then(
                [route_partition_ids_f = std::move(route_partition_ids_f),
                 walk_components_f = std::move(walk_components_f),
                 functor = std::move(functor)](auto&& remote_route_partition_ids_ff) mutable
                {
                    auto remote_route_partitions_ids_f{remote_route_partition_ids_ff.get()};

                    Count const nr_partitions{static_cast<Count>(remote_route_partitions_ids_f.size())};

                    for (Index p = 0; p < nr_partitions; ++p)
                    {
                        // Futures to IDs of all remote route partitions referred to by the route
                        // fragments in the current partitions
                        RemoteRoutePartitionIDs remote_route_partition_ids_f{
                            remote_route_partitions_ids_f[p].get()};

                        if (remote_route_partition_ids_f.empty())
                        {
                            // This route partition does not contain any route fragments. We can
                            // tell the functor to mark result partition(s) as finished.
                            functor.finish_partition(p);
                        }
                        else
                        {
                            // We have a non-empty collection of remote route fragment locations. Now
                            // we can wait for everything we need to get things going

                            // Future to ID of current route partition
                            hpx::future<hpx::id_type> route_partition_id_f{
                                std::move(route_partition_ids_f[p])};

                            // Future to Walk component associated with current partition
                            hpx::future<WalkComponent> walk_component_f{std::move(walk_components_f[p])};


                            // TODO
                        }
                    }


                    // // For each partition the collection of remote route partition IDs referred to
                    // std::vector<RemoteRoutePartitionIDs>>
                    // remote_route_partition_ids(remote_route_partition_ids_f.size());

                    // std::transform(remote_route_partition_ids_f.begin(),
                    // remote_route_partition_ids_f.end(), remote_route_partition_ids.begin,
                    //     [](auto const& ids_f)
                    //     {
                    //         return ids_f.get();
                    //     });

                    // std::vector<RemoteRoutePartitionIDs>>
                    // remote_route_partition_ids(remote_route_partition_ids_f.size());
                });


        ///                             WalkComponent walk_component{};
        ///                             std::optional<detail::RoutePartitionData> data{};

        ///                             // If the partition contains route fragments that continue in
        ///                             // other partitions
        ///                             if(!remote_fragment_locations.empty())
        ///                             {
        ///                                 // Create Walk component
        ///                                 walk_component = hpx::new_<WalkComponent>(locality);
        ///                                 // route_partition_data = std::make_tuple();
        ///                             }


        ///                             //     hpx::new_<Walk>(locality).then(
        ///                             //         [](Component&& component)
        ///                             //         {
        ///                             //             return hpx::make_tuple(hpx::new_<Walk>(locality),
        ///                             remote_fragment_locations);

        ///                             //             // Aggregate this information:
        ///                             //             // - For this walk component ID, the collection of:
        ///                             //             //     - Remote route partition ID
        ///                             //             //     - Remote route partition linear idx
        ///                             //             //


        ///                             return hpx::make_tuple(walk_component, data);


        ///                             //     ///         // detail::RoutePartitionData data{};
        ///                             //     ///         // std::vector<

        ///                             //     ///         // for(auto const& location:
        ///                             remote_fragment_locations)
        ///                             //     ///         // {
        ///                             //     ///         //     data[p].push_back(loc
        ///                             //     ///         // }


        ///                             //     ///         // RoutePartition
        ///                             partition_id_f{component.route_partition()};
        ///                             //     ///         // hpx::future<MaxValue> max_values_f{
        ///                             //     ///         //     component.sort_values(policies,
        ///                             value_partition)};

        ///                             //     ///         // return hpx::make_tuple(
        ///                             //     ///         //     std::move(component),
        ///                             //     ///         //     std::move(partition_id_f),
        ///                             //     ///         //     std::move(max_values_f));


        ///                             //             std::vector<
        ///                             //             std::optional<detail::RoutePartitionData>;

        ///                             //         }
        ///                             // }
        ///                         }
        ///                     ),
        ///                     route_partition.remote_route_fragment_locations(),
        ///                     locality_f));

        ///             return hpx::make_tuple(walk_component, route_partition_data_f);


        ///             // Once partition is ready, obtain collection with remote fragment locations
        ///             // and wait for the collection to arrive

        ///             auto [walk_component, route_partition_data_f] =
        ///             hpx::split_future(hpx::dataflow(hpx::launch::async,
        ///                     hpx::unwrapping(
        ///                         [p, route_partition_id](
        ///                             auto const& remote_fragment_locations,
        ///                             hpx::id_type const locality)
        ///                         {
        ///                             WalkComponent walk_component{};
        ///                             std::optional<detail::RoutePartitionData> data{};

        ///                             // If the partition contains route fragments that continue in
        ///                             // other partitions
        ///                             if(!remote_fragment_locations.empty())
        ///                             {
        ///                                 // Create Walk component
        ///                                 walk_component = hpx::new_<WalkComponent>(locality);
        ///                                 // route_partition_data = std::make_tuple();
        ///                             }


        ///                             //     hpx::new_<Walk>(locality).then(
        ///                             //         [](Component&& component)
        ///                             //         {
        ///                             //             return hpx::make_tuple(hpx::new_<Walk>(locality),
        ///                             remote_fragment_locations);

        ///                             //             // Aggregate this information:
        ///                             //             // - For this walk component ID, the collection of:
        ///                             //             //     - Remote route partition ID
        ///                             //             //     - Remote route partition linear idx
        ///                             //             //


        ///                             return hpx::make_tuple(walk_component, data);


        ///                             //     ///         // detail::RoutePartitionData data{};
        ///                             //     ///         // std::vector<

        ///                             //     ///         // for(auto const& location:
        ///                             remote_fragment_locations)
        ///                             //     ///         // {
        ///                             //     ///         //     data[p].push_back(loc
        ///                             //     ///         // }


        ///                             //     ///         // RoutePartition
        ///                             partition_id_f{component.route_partition()};
        ///                             //     ///         // hpx::future<MaxValue> max_values_f{
        ///                             //     ///         //     component.sort_values(policies,
        ///                             value_partition)};

        ///                             //     ///         // return hpx::make_tuple(
        ///                             //     ///         //     std::move(component),
        ///                             //     ///         //     std::move(partition_id_f),
        ///                             //     ///         //     std::move(max_values_f));


        ///                             //             std::vector<
        ///                             //             std::optional<detail::RoutePartitionData>;

        ///                             //         }
        ///                             // }
        ///                         }
        ///                     ),
        ///                     route_partition.remote_route_fragment_locations(),
        ///                     locality_f));

        ///             return hpx::make_tuple(walk_component, route_partition_data_f);


        ///             // return route_partition.remote_route_fragment_locations().then(
        ///             //     [p, route_partition_id](auto&& remote_fragment_locations_f)
        ///             //     {
        ///             //         auto remote_fragment_locations{remote_fragment_locations_f.get()};

        ///             //         // If the partition contains route fragments that continue in
        ///             //         // other partitions
        ///             //         if(!remote_fragment_locations.empty())
        ///             //         {
        ///             //             // Create Walk component

        ///             //             hpx::new_<Walk>(hpx::colocated(route_partition_id)).then(
        ///             //                 [](Component&& component)
        ///             //                 {

        ///             //             ///         // detail::RoutePartitionData data{};
        ///             //             ///         // std::vector<

        ///             //             ///         // for(auto const& location: remote_fragment_locations)
        ///             //             ///         // {
        ///             //             ///         //     data[p].push_back(loc
        ///             //             ///         // }


        ///             //             ///         // RoutePartition
        ///             partition_id_f{component.route_partition()};
        ///             //             ///         // hpx::future<MaxValue> max_values_f{
        ///             //             ///         //     component.sort_values(policies, value_partition)};

        ///             //             ///         // return hpx::make_tuple(
        ///             //             ///         //     std::move(component),
        ///             //             ///         //     std::move(partition_id_f),
        ///             //             ///         //     std::move(max_values_f));


        ///             //             ///     });


        ///             //         }
        ///             //     });


        // future to {route_partition_id: (walk_component_id, partition_idx)}

        // Linear idx is the idx within the vector
        // Last vector is the collection of remote route partition ids
        // Start with collecting this information:
        // std::vector<std::tuple<current_route_partition_id, walk_component,
        // std::vector<remote_route_partition_id>>>


        // Then postprocess to map current_route_partition_id to: walk_component,
        // std::vector<remote_route_partition_id>, linear_idx

        // Then pass this information to the walk component


        // Iterate over all route partitions. For each partition:
        // - When partition is ready:
        //     - If it contains route fragments:
        //         - Create Walk component
        //         - When walk component is ready:
        //             - Store mapping between the current / local route partition id and
        //                 - the corresponding walk component id
        //                 - linear idx of the current partition
        //                 - collection of remote (to the current partition) partition locations
        //                 → return future to {route_partition_id: (walk_component_id, partition_idx)}

        // When all future {route_partition_id, (walk_component_id, partition_idx)} are ready:
        // - Iterate over all route partitions. For each partition:
        //     - Obtain the remote route partition ids used by the fragments
        //     - Create subset of map {route_partition_id: (walk_component_id, partition_idx)}
        //       relevant for the walk component associated with the current partition and send
        //       it to the walk component. `Walk::set_component_ids(...)`


        // Now all required Walk components are created and they know:
        // - The ID of the remote Walk components which handle the route partitions
        // - The idx of the corresponding result partition


        // This must be easy to do by the Walk server component:
        // - void visit(Idx const partition_idx, Idx const cell_idx)
        //     → partition_idx must be passed in
        //     → cell_idx can be obtained from the route fragment


        /// // Aggregate stuff per route?!

        /// auto const& route_partitions{route.partitions()};
        /// Count const nr_partitions{lue::nr_elements(route_partitions)};
        /// std::vector<hpx::future<std::optional<detail::RoutePartitionData>>>
        /// route_partition_data(nr_partitions); std::vector<WalkComponent> walk_components(nr_partitions);

        /// /// // Obtain localities of all route partitions
        /// /// auto localities_f = localities(route_partitions);

        /// /// localities_f.then(
        /// ///         [route_partitions = std::move(route_partitions)](auto&& localities_f)
        /// ///         {
        /// ///             auto const localities{localities_f.get()};
        /// ///         });


        /// for (Index p = 0; p < nr_partitions; ++p)
        /// {
        ///     std::tie(walk_components[p], route_partition_data[p]) =
        ///     hpx::split_future(hpx::dataflow(hpx::launch::async,
        ///         [p](auto const& route_partition)
        ///         {
        ///             hpx::future<hpx::id_type>
        ///             locality_f{hpx::get_colocation_id(route_partition.get_id())}; hpx::id_type const
        ///             route_partition_id{route_partition.get_id()};

        ///             // Once partition is ready, obtain collection with remote fragment locations
        ///             // and wait for the collection to arrive

        ///             auto [walk_component, route_partition_data_f] =
        ///             hpx::split_future(hpx::dataflow(hpx::launch::async,
        ///                     hpx::unwrapping(
        ///                         [p, route_partition_id](
        ///                             auto const& remote_fragment_locations,
        ///                             hpx::id_type const locality)
        ///                         {
        ///                             WalkComponent walk_component{};
        ///                             std::optional<detail::RoutePartitionData> data{};

        ///                             // If the partition contains route fragments that continue in
        ///                             // other partitions
        ///                             if(!remote_fragment_locations.empty())
        ///                             {
        ///                                 // Create Walk component
        ///                                 walk_component = hpx::new_<WalkComponent>(locality);
        ///                                 // route_partition_data = std::make_tuple();
        ///                             }


        ///                             //     hpx::new_<Walk>(locality).then(
        ///                             //         [](Component&& component)
        ///                             //         {
        ///                             //             return hpx::make_tuple(hpx::new_<Walk>(locality),
        ///                             remote_fragment_locations);

        ///                             //             // Aggregate this information:
        ///                             //             // - For this walk component ID, the collection of:
        ///                             //             //     - Remote route partition ID
        ///                             //             //     - Remote route partition linear idx
        ///                             //             //


        ///                             return hpx::make_tuple(walk_component, data);


        ///                             //     ///         // detail::RoutePartitionData data{};
        ///                             //     ///         // std::vector<

        ///                             //     ///         // for(auto const& location:
        ///                             remote_fragment_locations)
        ///                             //     ///         // {
        ///                             //     ///         //     data[p].push_back(loc
        ///                             //     ///         // }


        ///                             //     ///         // RoutePartition
        ///                             partition_id_f{component.route_partition()};
        ///                             //     ///         // hpx::future<MaxValue> max_values_f{
        ///                             //     ///         //     component.sort_values(policies,
        ///                             value_partition)};

        ///                             //     ///         // return hpx::make_tuple(
        ///                             //     ///         //     std::move(component),
        ///                             //     ///         //     std::move(partition_id_f),
        ///                             //     ///         //     std::move(max_values_f));


        ///                             //             std::vector<
        ///                             //             std::optional<detail::RoutePartitionData>;

        ///                             //         }
        ///                             // }
        ///                         }
        ///                     ),
        ///                     route_partition.remote_route_fragment_locations(),
        ///                     locality_f));

        ///             return hpx::make_tuple(walk_component, route_partition_data_f);


        ///             // return route_partition.remote_route_fragment_locations().then(
        ///             //     [p, route_partition_id](auto&& remote_fragment_locations_f)
        ///             //     {
        ///             //         auto remote_fragment_locations{remote_fragment_locations_f.get()};

        ///             //         // If the partition contains route fragments that continue in
        ///             //         // other partitions
        ///             //         if(!remote_fragment_locations.empty())
        ///             //         {
        ///             //             // Create Walk component

        ///             //             hpx::new_<Walk>(hpx::colocated(route_partition_id)).then(
        ///             //                 [](Component&& component)
        ///             //                 {

        ///             //             ///         // detail::RoutePartitionData data{};
        ///             //             ///         // std::vector<

        ///             //             ///         // for(auto const& location: remote_fragment_locations)
        ///             //             ///         // {
        ///             //             ///         //     data[p].push_back(loc
        ///             //             ///         // }


        ///             //             ///         // RoutePartition
        ///             partition_id_f{component.route_partition()};
        ///             //             ///         // hpx::future<MaxValue> max_values_f{
        ///             //             ///         //     component.sort_values(policies, value_partition)};

        ///             //             ///         // return hpx::make_tuple(
        ///             //             ///         //     std::move(component),
        ///             //             ///         //     std::move(partition_id_f),
        ///             //             ///         //     std::move(max_values_f));


        ///             //             ///     });


        ///             //         }
        ///             //     });
        ///         },
        ///         route_partitions[p]));
        /// }
    }

}  // namespace lue


#define LUE_REGISTER_WALK_ACTIONS(rank, FunctorName, Functor)                                                \
                                                                                                             \
    using WalkServerType_##rank##_##FunctorName =                                                            \
        hpx::components::component<lue::detail::server::Walk<rank, Functor>>;                                \
                                                                                                             \
    HPX_REGISTER_COMPONENT(WalkServerType_##rank##_##FunctorName, Walk_##rank##_##FunctorName)


// #define LUE_INSTANTIATE_WALK(Policies)
//
//     template LUE_ROUTING_OPERATION_EXPORT void walk<ArgumentType<void(Policies)>, 2>(
//         ArgumentType<void(Policies)> const&,
//         SerialRoute<2> const&,
//         Functor&&);
