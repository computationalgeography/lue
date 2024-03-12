#pragma once
#include "lue/framework/algorithm/detail/localities.hpp"
#include "lue/framework/algorithm/detail/promise.hpp"
#include "lue/framework/algorithm/first_n.hpp"
#include "lue/framework/algorithm/routing/walk/cell_counter.hpp"
#include "lue/framework/algorithm/routing_operation_export.hpp"
#include "lue/framework/algorithm/walk.hpp"
#include "lue/framework/core/component.hpp"
#include "lue/macro.hpp"
#include <hpx/include/components.hpp>
#include <hpx/synchronization/shared_mutex.hpp>
#include <algorithm>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <type_traits>
#include <vector>


namespace lue {
    namespace detail::first_n {

        using Data = CellCounter;

        template<typename Policies, Rank rank>
        class Walk;

        namespace server {

            template<typename Policies, Rank rank>
            class Walk: public hpx::components::component_base<Walk<Policies, rank>>
            {

                private:

                    using ComponentServerBase = hpx::components::component_base<Walk<Policies, rank>>;
                    using ComponentClient = first_n::Walk<Policies, rank>;
                    using OutputElement = policy::OutputElementT<Policies, 0>;
                    using RouteID = policy::InputElementT<Policies, 0>;

                protected:

                    using OutputArray = PartitionedArray<OutputElement, 2>;
                    using Offset = OffsetT<OutputArray>;
                    using Shape = ShapeT<OutputArray>;

                public:

                    using OutputPartition = PartitionT<OutputArray>;
                    using RoutePartition = lue::SerialRoutePartition<RouteID, rank>;


                    Walk(Policies const& policies, RoutePartition route_partition):

                        ComponentServerBase{},
                        _policies{policies},
                        _output_partition_promise{},
                        _output_partition{},
                        _route_partition{std::move(route_partition)},
                        _walk_mutex{},
                        _downstream_components{},
                        _fragment_idxs{}

                    {
                        using OutputPartitionServer = typename OutputPartition::Server;

                        auto const& ondp = std::get<0>(_policies.outputs_policies()).output_no_data_policy();
                        OutputElement no_data_value;

                        ondp.mark_no_data(no_data_value);

                        _route_partition.wait();
                        auto route_partition_ptr{ready_component_ptr(_route_partition)};

                        _output_partition = OutputPartition{hpx::new_<OutputPartitionServer>(
                            hpx::find_here(),
                            route_partition_ptr->offset(),
                            route_partition_ptr->shape(),
                            no_data_value)};

                        std::set<RouteID> const route_ids{route_partition_ptr->route_ids()};

                        for (RouteID const route_id : route_ids)
                        {
                            // Initialize with idx. First fragment to visit has index
                            // zero. This index increases during the walk, until the last
                            // fragment has been handled, after which the index for this
                            // route_id can be removed. When all indices have been removed,
                            // the walk has finished.
                            _fragment_idxs[route_id] = 0;
                        }
                    }


                    OutputPartition result_partition()
                    {
                        // Connect the promise with the future
                        // This function must be called exactly once
                        OutputPartition partition{_output_partition_promise.get_future()};

                        // If the route partition contains no fragments, the output partition
                        // can be marked as finished immediately
                        if (_fragment_idxs.empty())
                        {
                            finish_partition();
                        }

                        return partition;
                    }


                    void set_downstream_components(std::map<hpx::id_type, ComponentClient>&& components)
                    {
                        _downstream_components = components;
                    }


                    void walk(RouteID const route_id, Data&& data)
                    {
                        // This function may be called by multiple threads, for different
                        // routes, or for the same route but for different fragments. In
                        // the latter case, these calls will be ordered, with the first
                        // call handling an upstream fragment and subsequent calls handling
                        // downstream fragments.

                        // std::shared_lock read_lock{_walk_mutex, std::defer_lock};
                        std::unique_lock write_lock{_walk_mutex, std::defer_lock};

                        // First, do stuff, *using* state veriables. Don't change stuff that
                        // is shared between threads. Changing different cells in a raster
                        // is fine, just don't change the same values.
                        // TODO read_lock.lock();
                        write_lock.lock();

                        auto route_partition_server_ptr{ready_component_ptr(_route_partition)};
                        auto& route_partition_server{*route_partition_server_ptr};

                        auto output_partition_server_ptr{ready_component_ptr(_output_partition)};
                        auto& output_partition_server{*output_partition_server_ptr};
                        auto output_partition_data{output_partition_server.data()};

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
                        // - Calculate an output value
                        // - Assign output value to corresponding cell in the output partition
                        for (std::size_t idx = 0; idx < cell_idxs.size() && data.keep_walking();
                             ++idx, ++data)
                        {
                            output_partition_data[cell_idxs[idx]] = 1;
                        }

                        // TODO read_lock.unlock();
                        // TODO write_lock.lock();

                        ++fragment_idx;

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
                                finish_partition();
                            }
                        }
                    }


                    void skip_walking_route_fragments(RouteID const route_id)
                    {
                        // Skip walking route fragments for the route with the ID passed in.
                        // If this results in an empty _fragment_idxs collection, then
                        // apparently we have handled all fragments of all routes, and the
                        // output partition can be finished.

                        // This function is called from another component. Since we will
                        // be changing the state of the current component, we need to obtain
                        // a write lock.

                        // TODO Had to guard the whole region by a write block. Apparently,
                        //      threads started tripping over each other. For some reason,
                        //      the route_id passed in wasn't in _fragment_idxs anymore, which
                        //      must never be the case.

                        // TODO std::shared_lock read_lock{_walk_mutex, std::defer_lock};
                        std::unique_lock write_lock{_walk_mutex, std::defer_lock};

                        // TODO read_lock.lock();
                        write_lock.lock();

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

                        // TODO read_lock.unlock();
                        // TODO write_lock.lock();

                        ++fragment_idx;

                        if (fragment_idx == static_cast<Size>(std::size(route_fragments)))
                        {
                            // This was the last fragment of the route that is located in
                            // this partition. If it was the last fragment at all, we can / must
                            // finish the partition.
                            _fragment_idxs.erase(route_id);

                            if (_fragment_idxs.empty())
                            {
                                finish_partition();
                            }
                        }
                    }


                    HPX_DEFINE_COMPONENT_ACTION(Walk, result_partition, ResultPartitionAction);

                    HPX_DEFINE_COMPONENT_ACTION(
                        Walk, set_downstream_components, SetDownstreamComponentsAction);

                    HPX_DEFINE_COMPONENT_ACTION(Walk, walk, WalkAction);

                    HPX_DEFINE_COMPONENT_ACTION(
                        Walk, skip_walking_route_fragments, SkipWalkingRouteFragmentsAction);

                private:

                    void finish_partition()
                    {
                        // This function must be called exactly once
                        lue_hpx_assert(_output_partition.valid());
                        lue_hpx_assert(_output_partition.is_ready());
                        lue_hpx_assert(_fragment_idxs.empty());

                        _output_partition_promise.set_value(_output_partition.get_id());

                        _downstream_components.clear();
                    }

                    Policies _policies;

                    mutable hpx::promise<hpx::id_type> _output_partition_promise;

                    OutputPartition _output_partition;

                    RoutePartition _route_partition;

                    hpx::shared_mutex _walk_mutex;

                    //! Component client by route partition ID
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
                using ComponentClientBase =
                    hpx::components::client_base<Walk<Policies, rank>, ComponentServer>;
                using OutputPartition = typename ComponentServer::OutputPartition;

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


                OutputPartition result_partition() const
                {
                    lue_hpx_assert(this->is_ready());
                    lue_hpx_assert(this->get_id());

                    typename ComponentServer::ResultPartitionAction action;

                    return hpx::async(action, this->get_id());
                }


                hpx::future<void> set_downstream_components(std::map<hpx::id_type, Walk>&& components)
                {
                    lue_hpx_assert(this->is_ready());
                    lue_hpx_assert(this->get_id());

                    typename ComponentServer::SetDownstreamComponentsAction action;

                    return hpx::async(action, this->get_id(), std::move(components));
                }


                hpx::future<void> walk(RouteID const route_id, Data data) const
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

    }  // namespace detail::first_n


    template<typename Policies, Rank rank>
    auto first_n(
        Policies const& policies,
        SerialRoute<policy::InputElementT<Policies, 0>, rank> const& route,
        Count const max_nr_cells) -> PartitionedArray<policy::OutputElementT<Policies, 0>, rank>
    {
        static_assert(std::is_integral_v<policy::OutputElementT<Policies, 0>>);
        static_assert(std::is_integral_v<policy::InputElementT<Policies, 0>>);

        using RouteID = policy::InputElementT<Policies, 0>;
        using Route = SerialRoute<RouteID, rank>;
        using RoutePartition = PartitionT2<Route>;

        using OutputElement = policy::OutputElementT<Policies, 0>;
        using OutputArray = PartitionedArray<OutputElement, rank>;
        using OutputPartition = PartitionT<OutputArray>;
        using OutputPartitions = PartitionsT<OutputArray>;

        // Iterate over all route partitions and:
        // - Create a new Walk component, passing in the route partition

        using WalkComponentClient = detail::first_n::Walk<Policies, rank>;
        using WalkComponentServer = detail::first_n::server::Walk<Policies, rank>;

        // TODO
        // Since a partioned array currently caches locality IDs, we need to retrieve them,
        // unfortunately. The localities stuff can be removed once arrays don't cache locality
        // IDs anymore.

        auto const& route_partitions{route.partitions()};

        Array<hpx::future<hpx::id_type>, rank> locality_fs{detail::localities(route_partitions)};

        Count const nr_partitions{nr_elements(route_partitions.shape())};
        Array<WalkComponentClient, rank> walk_components{route_partitions.shape()};

        for (Index partition_idx = 0; partition_idx < nr_partitions; ++partition_idx)
        {
            walk_components[partition_idx] = WalkComponentClient{hpx::dataflow(
                hpx::launch::async,
                [policies](RoutePartition const& route_partition)
                {
                    return hpx::new_<WalkComponentServer>(
                        hpx::colocated(route_partition.get_id()), policies, route_partition);
                },
                route_partitions[partition_idx])};
        }

        OutputPartitions partitions{walk_components.shape()};

        for (Index partition_idx = 0; partition_idx < nr_partitions; ++partition_idx)
        {
            partitions[partition_idx] = walk_components[partition_idx].then(
                hpx::unwrapping([](WalkComponentClient const& component)
                                { return OutputPartition{component.result_partition()}; }));
        }

        detail::first_n::Data data{max_nr_cells};

        // Do whatever it takes to end up with ready output partitions
        walk(route, walk_components, std::move(data));

        // Keep the components alive for as long as the partitions are not ready
        hpx::when_all(partitions.begin(), partitions.end())
            .then([components = std::move(walk_components)](auto&&) { HPX_UNUSED(components); });

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

        return OutputArray{route.shape(), std::move(localities), std::move(partitions)};
    }

}  // namespace lue


#define LUE_REGISTER_FIRST_N_ACTION_DECLARATIONS(Policies, rank, unique)                                     \
                                                                                                             \
    namespace lue::detail {                                                                                  \
        using FirstNWalkServer_##unique = lue::detail::first_n::server::Walk<Policies, rank>;                \
    }                                                                                                        \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::FirstNWalkServer_##unique::ResultPartitionAction,                                       \
        FirstNWalkServerResultPartitionAction_##unique)                                                      \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::FirstNWalkServer_##unique::SetDownstreamComponentsAction,                               \
        FirstNWalkServerSetRemoteComponentsAction_##unique)                                                  \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::FirstNWalkServer_##unique::WalkAction, FirstNWalkServerWalkAction_##unique)             \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::FirstNWalkServer_##unique::SkipWalkingRouteFragmentsAction,                             \
        FirstNWalkServerSkipWalkingRouteFragmentsAction_##unique)


#define LUE_REGISTER_FIRST_N_ACTIONS(Policies, rank, unique)                                                 \
                                                                                                             \
    namespace lue::detail {                                                                                  \
        using FirstNWalkServerComponent_##unique =                                                           \
            hpx::components::component<lue::detail::first_n::server::Walk<Policies, rank>>;                  \
    }                                                                                                        \
                                                                                                             \
    HPX_REGISTER_COMPONENT(                                                                                  \
        lue::detail::FirstNWalkServerComponent_##unique, FirstNWalkServerComponent_##unique)                 \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::FirstNWalkServer_##unique::ResultPartitionAction,                                       \
        FirstNWalkServerResultPartitionAction_##unique)                                                      \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::FirstNWalkServer_##unique::SetDownstreamComponentsAction,                               \
        FirstNWalkServerSetRemoteComponentsAction_##unique)                                                  \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::FirstNWalkServer_##unique::WalkAction, FirstNWalkServerWalkAction_##unique)             \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::FirstNWalkServer_##unique::SkipWalkingRouteFragmentsAction,                             \
        FirstNWalkServerSkipWalkingRouteFragmentsAction_##unique)


#define LUE_INSTANTIATE_FIRST_N(Policies, rank)                                                              \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT PartitionedArray<policy::OutputElementT<Policies, 0>, rank>        \
    first_n<ArgumentType<void(Policies)>, rank>(                                                             \
        ArgumentType<void(Policies)> const&,                                                                 \
        SerialRoute<policy::InputElementT<Policies, 0>, rank> const&,                                        \
        Count const);
