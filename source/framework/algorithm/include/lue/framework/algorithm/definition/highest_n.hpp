#pragma once
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/decreasing_order.hpp"
#include "lue/framework/algorithm/detail/promise.hpp"
#include "lue/framework/algorithm/highest_n.hpp"
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
#include <vector>


namespace lue {
    namespace detail {
        namespace highest_n {

            // TODO Move to translation unit if possible
            class Data
            {

                public:

                    Data():

                        _nr_cells_visited{},
                        _max_nr_cells_to_visit{}

                    {
                    }


                    Data(Count const max_nr_cells):

                        _nr_cells_visited{},
                        _max_nr_cells_to_visit{max_nr_cells}

                    {
                    }

                    Data(Data const&) = default;

                    Data(Data&&) = default;

                    ~Data() = default;

                    Data& operator=(Data const&) = default;

                    Data& operator=(Data&&) = default;


                    Count& operator++()
                    {
                        return ++_nr_cells_visited;
                    }


                    bool keep_walking() const
                    {
                        return _nr_cells_visited < _max_nr_cells_to_visit;
                    }


                private:

                    friend class hpx::serialization::access;


                    template<typename Archive>
                    void serialize(Archive& archive, unsigned int const)
                    {
                        // clang-format off
                        archive & _max_nr_cells_to_visit;
                        // clang-format on
                    }

                    Count _nr_cells_visited;

                    Count _max_nr_cells_to_visit;
            };

            // TODO bitwise serializable


            template<typename Policies, Rank rank>
            class Walk;

            namespace server {

                template<typename Policies, Rank rank>
                class Walk: public hpx::components::component_base<Walk<Policies, rank>>
                {

                    private:

                        using ComponentServerBase = hpx::components::component_base<Walk<Policies, rank>>;
                        using ComponentClient = highest_n::Walk<Policies, rank>;
                        using OutputElement = policy::OutputElementT<Policies>;

                    protected:

                        using OutputArray = PartitionedArray<OutputElement, 2>;
                        using Offset = OffsetT<OutputArray>;
                        using Shape = ShapeT<OutputArray>;

                    public:

                        using RouteID = Index;
                        using OutputPartition = PartitionT<OutputArray>;
                        using RoutePartition = lue::SerialRoutePartition<rank>;


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

                            auto const& ondp =
                                std::get<0>(_policies.outputs_policies()).output_no_data_policy();
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

                            // TODO For some reason this function gets called for a route_id
                            //      for which _fragments_idxs does not contain a record
                            //      anymore. Protecting all code with a unique_lock solves it.
                            // TODO std::shared_lock read_lock{_walk_mutex, std::defer_lock};
                            // TODO std::unique_lock write_lock{_walk_mutex, std::defer_lock};
                            std::unique_lock write_lock{_walk_mutex};

                            // First, do stuff, using state veriables. Don't change stuff that
                            // is shared between threads. Changing different cells in a raster
                            // is fine, just don't change the same values.
                            // TODO read_lock.lock();

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

                            bool const keep_walking{data.keep_walking()};

                            if (!route_fragment.is_last())
                            {
                                if (keep_walking)
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
                                    // Notify downstream components to skip walking the rest
                                    // of the route. We are done with this route.
                                    notify_skip_walking_route_fragments(route_id);
                                }
                            }

                            // TODO read_lock.unlock();

                            // Now update the state variables to reflect the fact that we just
                            // walked an additional fragment of the route
                            // TODO write_lock.lock();

                            ++fragment_idx;

                            lue_hpx_assert(
                                (route_fragment.is_last() &&
                                 fragment_idx == static_cast<Size>(std::size(route_fragments))) ||
                                !route_fragment.is_last());

                            if ((fragment_idx == static_cast<Size>(std::size(route_fragments))) ||
                                !keep_walking)
                            {
                                // There are reasons to stop walking the current route:
                                // - We have handled the last fragment
                                // - We have handled a sufficiently number of cells
                                // - ...

                                // Erase the index from the collection
                                // Note: fragment_idx reference dangles from now on
                                _fragment_idxs.erase(route_id);

                                // If we just handled the last route, then we can finish the
                                // output partition
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

                            {
                                // This function is called from another component. Since we will
                                // be changing the state of the current component, we need to obtain
                                // a write lock.

                                std::unique_lock write_lock{_walk_mutex};

                                lue_hpx_assert(!_fragment_idxs.empty());
                                lue_hpx_assert(_fragment_idxs.find(route_id) != _fragment_idxs.end());

                                _fragment_idxs.erase(route_id);

                                if (_fragment_idxs.empty())
                                {
                                    finish_partition();
                                }
                            }

                            notify_skip_walking_route_fragments(route_id);
                        }


                        HPX_DEFINE_COMPONENT_ACTION(Walk, result_partition, ResultPartitionAction);

                        HPX_DEFINE_COMPONENT_ACTION(
                            Walk, set_downstream_components, SetDownstreamComponentsAction);

                        HPX_DEFINE_COMPONENT_ACTION(Walk, walk, WalkAction);

                        HPX_DEFINE_COMPONENT_ACTION(
                            Walk, skip_walking_route_fragments, SkipWalkingRouteFragmentsAction);

                    private:

                        void notify_skip_walking_route_fragments(RouteID const route_id)
                        {
                            // Tell the *downstream partitions* that further walking the
                            // route will be skipped. This should continue until the
                            // end of the route. In case skipping these fragments results in
                            // an empty _fragment_idxs collection, then apparantly we have handled
                            // all partitions of all routes and the output partition can be finished.

                            // This function is called from walk, with the read lock locked. This
                            // is great, since we are only sending messages to downstream
                            // components. We're not changing the state of the current component.

                            {
                                lue_hpx_assert(!_fragment_idxs.empty());
                                lue_hpx_assert(_fragment_idxs.find(route_id) != _fragment_idxs.end());
                                Index& fragment_idx{_fragment_idxs.at(route_id)};
                                auto const& route_fragments{
                                    ready_component_ptr(_route_partition)->route_fragments(route_id)};
                                lue_hpx_assert(fragment_idx < static_cast<Size>(std::size(route_fragments)));

                                while (fragment_idx < static_cast<Size>(std::size(route_fragments)))
                                {
                                    auto const& route_fragment{route_fragments[fragment_idx]};

                                    lue_hpx_assert(route_fragment.next_fragment_location().valid());
                                    lue_hpx_assert(route_fragment.next_fragment_location().is_ready());

                                    hpx::id_type const downstream_route_partition_id{
                                        route_fragment.next_fragment_location().get()};

                                    lue_hpx_assert(
                                        _downstream_components.find(downstream_route_partition_id) !=
                                        _downstream_components.end());

                                    _downstream_components.at(downstream_route_partition_id)
                                        .skip_walking_route_fragments(route_id);

                                    ++fragment_idx;
                                }
                            }
                        }


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
            class Walk:
                public hpx::components::client_base<Walk<Policies, rank>, server::Walk<Policies, rank>>
            {

                private:

                    using ComponentServer = server::Walk<Policies, rank>;
                    using ComponentClientBase =
                        hpx::components::client_base<Walk<Policies, rank>, ComponentServer>;
                    using RouteID = typename ComponentServer::RouteID;
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

        }  // namespace highest_n
    }      // namespace detail


    // TODO
    // There is no need for visiting the cells in turn. Procedure can be made fully parallel. Maybe have
    // a different walk / visit function for this case.
    // - walk: Walk along a route, visiting cells in order. Needed when there is a dependency
    //   between the cells.
    // - visit: Just visit all cells in whatever order is the fastest. Possible when there is
    //   no dependency between the cells.


    template<typename Policies, Rank rank>
    PartitionedArray<policy::OutputElementT<Policies>, rank> highest_n(
        Policies const& policies, SerialRoute<rank> const& route, Count const max_nr_cells)
    {
        using Route = SerialRoute<rank>;
        using RoutePartition = PartitionT2<Route>;

        using InputElement = policy::InputElementT<Policies>;

        using OutputElement = policy::OutputElementT<Policies>;
        using OutputArray = PartitionedArray<OutputElement, rank>;
        using OutputPartition = PartitionT<OutputArray>;
        using OutputPartitions = PartitionsT<OutputArray>;

        using WalkPolicies = policy::DefaultValuePolicies<
            policy::AllValuesWithinDomain<InputElement>,
            policy::OutputElements<OutputElement>,
            policy::InputElements<InputElement>>;

        // Iterate over all route partitions and:
        // - Create a new Walk component, passing in the route partition

        using WalkComponentClient = detail::highest_n::Walk<WalkPolicies, rank>;
        using WalkComponentServer = detail::highest_n::server::Walk<WalkPolicies, rank>;

        // TODO
        // Since a partioned array currently caches locality IDs, we need to retrieve them,
        // unfortunately. The localities stuff can be removed once arrays don't cache locality
        // IDs anymore.

        auto const& route_partitions{route.partitions()};
        Count const nr_partitions{nr_elements(route_partitions.shape())};
        Array<hpx::future<hpx::id_type>, rank> localities_f{route_partitions.shape()};
        Array<WalkComponentClient, rank> walk_components{route_partitions.shape()};

        for (Index p = 0; p < nr_partitions; ++p)
        {
            lue_hpx_assert(route_partitions[p].valid());

            hpx::future<hpx::id_type> meh_f;

            // hpx::tie(localities_f[p], walk_components[p]) =
            // hpx::split_future(hpx::dataflow(hpx::launch::async,
            hpx::tie(localities_f[p], meh_f) = hpx::split_future(hpx::dataflow(
                hpx::launch::async,
                [policies](RoutePartition const& route_partition)
                {
                    lue_hpx_assert(route_partition.valid());
                    lue_hpx_assert(route_partition.is_ready());

                    hpx::id_type const route_partition_id{route_partition.get_id()};

                    hpx::future<hpx::id_type> locality_f{hpx::get_colocation_id(route_partition_id)};
                    hpx::future<hpx::id_type> walk_component_f{hpx::new_<WalkComponentServer>(
                        hpx::colocated(route_partition_id), policies, route_partition)};

                    return hpx::make_tuple(std::move(locality_f), std::move(walk_component_f));
                },
                route_partitions[p]));

            walk_components[p] = WalkComponentClient{std::move(meh_f)};

            lue_hpx_assert(localities_f[p].valid());
            lue_hpx_assert(walk_components[p].valid());
        }

        // TODO Get rid of this wait and localities stuff
        hpx::wait_all(localities_f.begin(), localities_f.end());

        Array<hpx::id_type, rank> localities{localities_f.shape()};

        lue_hpx_assert(localities.shape() == localities_f.shape());

        std::transform(
            localities_f.begin(),
            localities_f.end(),
            localities.begin(),
            [](hpx::future<hpx::id_type>& locality_f) { return locality_f.get(); });

        OutputPartitions partitions{walk_components.shape()};

        for (Index p = 0; p < nr_partitions; ++p)
        {
            partitions[p] = walk_components[p].then(
                hpx::unwrapping([](WalkComponentClient const& component)
                                { return OutputPartition{component.result_partition()}; }));
        }

        detail::highest_n::Data data{max_nr_cells};

        // Do whatever it takes to end up with ready output partitions
        walk(route, walk_components, std::move(data));

        // Keep the components alive for as long as the partitions are not ready
        hpx::when_all(partitions.begin(), partitions.end())
            .then([components = std::move(walk_components)](auto&&) { HPX_UNUSED(components); });

        return OutputArray{route.shape(), std::move(localities), std::move(partitions)};
    }


    template<typename Policies, typename ZoneElement, typename InputElement, Rank rank>
    PartitionedArray<policy::OutputElementT<Policies>, rank> highest_n(
        [[maybe_unused]] /* TODO */ Policies const& policies,
        PartitionedArray<ZoneElement, rank> const& zone,
        PartitionedArray<InputElement, rank> const& array,
        Count const max_nr_cells)
    {
        // TODO Make this work
        // using DecreasingOrderPolicies = policy::Policies<
        //     policy::AllValuesWithinDomain<ZoneElement, InputElement>,
        //     policy::OutputsPolicies<>,
        //     policy::InputsPolicies<policy::InputPoliciesT<Policies, 0>, policy::InputPoliciesT<Policies,
        //     1>>>;

        using DecreasingOrderPolicies = policy::DefaultValuePolicies<
            policy::AllValuesWithinDomain<ZoneElement, InputElement>,
            policy::OutputElements<>,
            policy::InputElements<ZoneElement, InputElement>>;

        // TODO Make this depend on the policy and its state passed in
        using OutputElement = policy::OutputElementT<Policies>;

        using WalkPolicies = policy::DefaultValuePolicies<
            policy::AllValuesWithinDomain<InputElement>,
            policy::OutputElements<OutputElement>,
            policy::InputElements<InputElement>>;

        SerialRoute<rank> route = decreasing_order(DecreasingOrderPolicies{}, zone, array, max_nr_cells);

        return highest_n(WalkPolicies{}, route, max_nr_cells);
    }


    template<typename Policies, typename InputElement, Rank rank>
    PartitionedArray<policy::OutputElementT<Policies>, rank> highest_n(
        [[maybe_unused]] /* TODO */ Policies const& policies,
        PartitionedArray<InputElement, rank> const& array,
        Count const max_nr_cells)
    {
        // TODO Make this work
        // using DecreasingOrderPolicies = policy::Policies<
        //     policy::AllValuesWithinDomain<InputElement>,
        //     policy::OutputsPolicies<>,
        //     policy::InputsPolicies<policy::InputPoliciesT<Policies, 0, InputElement>>>;

        using DecreasingOrderPolicies = policy::DefaultValuePolicies<
            policy::AllValuesWithinDomain<InputElement>,
            policy::OutputElements<>,
            policy::InputElements<InputElement>>;

        // TODO Make this depend on the policy and its state passed in
        using OutputElement = policy::OutputElementT<Policies>;

        using WalkPolicies = policy::DefaultValuePolicies<
            policy::AllValuesWithinDomain<InputElement>,
            policy::OutputElements<OutputElement>,
            policy::InputElements<InputElement>>;

        SerialRoute<rank> route = decreasing_order(DecreasingOrderPolicies{}, array, max_nr_cells);

        return highest_n(WalkPolicies{}, route, max_nr_cells);
    }

}  // namespace lue


#define LUE_REGISTER_HIGHEST_N_ACTION_DECLARATIONS(Policies, rank, unique)                                   \
                                                                                                             \
    using WalkServer = hpx::components::component<lue::detail::highest_n::server::Walk<Policies, rank>>;     \
                                                                                                             \
    HPX_REGISTER_COMPONENT(WalkServer, HighestNWalk_##rank##_##unique)                                       \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        WalkServer::ResultPartitionAction, HighestNWalk_##rank##_##unique##_ResultPartitionAction)           \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        WalkServer::SetDownstreamComponentsAction,                                                           \
        HighestNWalk_##rank##_##unique##_SetRemoteComponentsAction)                                          \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(WalkServer::WalkAction, HighestNWalk_##rank##_##unique##_WalkAction)     \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        WalkServer::SkipWalkingRouteFragmentsAction,                                                         \
        HighestNWalk_##rank##_##unique##_SkipWalkingRouteFragmentsAction)


#define LUE_REGISTER_HIGHEST_N_ACTIONS(Policies, rank, unique)                                               \
                                                                                                             \
    using WalkServer = hpx::components::component<lue::detail::highest_n::server::Walk<Policies, rank>>;     \
                                                                                                             \
    HPX_REGISTER_COMPONENT(WalkServer, HighestNWalk_##rank##_##unique)                                       \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        WalkServer::ResultPartitionAction, HighestNWalk_##rank##_##unique##_ResultPartitionAction)           \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        WalkServer::SetDownstreamComponentsAction,                                                           \
        HighestNWalk_##rank##_##unique##_SetRemoteComponentsAction)                                          \
                                                                                                             \
    HPX_REGISTER_ACTION(WalkServer::WalkAction, HighestNWalk_##rank##_##unique##_WalkAction)                 \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        WalkServer::SkipWalkingRouteFragmentsAction,                                                         \
        HighestNWalk_##rank##_##unique##_SkipWalkingRouteFragmentsAction)


#define LUE_INSTANTIATE_HIGHEST_N(Policies, InputElement)                                                    \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT PartitionedArray<policy::OutputElementT<Policies>, 2>              \
    highest_n<ArgumentType<void(Policies)>, InputElement, 2>(                                                \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<InputElement, 2> const&,                                                            \
        Count const max_nr_cells);

#define LUE_INSTANTIATE_HIGHEST_N_ZONE(Policies, ZoneElement, InputElement)                                  \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT PartitionedArray<policy::OutputElementT<Policies>, 2>              \
    highest_n<ArgumentType<void(Policies)>, ZoneElement, InputElement, 2>(                                   \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<ZoneElement, 2> const&,                                                             \
        PartitionedArray<InputElement, 2> const&,                                                            \
        Count const max_nr_cells);
