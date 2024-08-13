#pragma once
#include "lue/framework/algorithm/decreasing_order.hpp"
#include "lue/framework/algorithm/detail/verify_compatible.hpp"
#include "lue/framework/algorithm/routing_operation_export.hpp"
#include "lue/framework/core/component.hpp"
#include "lue/macro.hpp"
#include <hpx/include/components.hpp>
#include <hpx/synchronization/shared_mutex.hpp>
#include <algorithm>
#include <map>
#include <mutex>
#include <shared_mutex>
#include <tuple>
#include <type_traits>
#include <vector>


namespace lue {
    namespace detail {

        // NOTE: deque is slower than vector

        template<typename Policies, Rank rank>
        class DecreasingOrderZonal;

        template<typename Policies, Rank rank>
        class DecreasingOrderGlobal;

        template<typename Value>
        using LocalValue = std::tuple<Value, lue::Index>;

        template<typename Value>
        using LocalValues = std::vector<LocalValue<Value>>;

        template<typename Value>
        bool operator>(LocalValue<Value> const& lhs, LocalValue<Value> const& rhs)
        {
            return std::get<0>(lhs) > std::get<0>(rhs);
        }

        template<typename Value>
        struct CompareLocalMaxima
        {
                bool operator()(LocalValue<Value> const& lhs, LocalValue<Value> const& rhs) const
                {
                    return lhs > rhs;
                }
        };

        template<typename Value>
        using DownstreamMaximum = std::tuple<Value, hpx::id_type>;

        template<typename Value>
        using DownstreamMaxima = std::vector<DownstreamMaximum<Value>>;

        template<typename Value>
        bool operator>(DownstreamMaximum<Value> const& lhs, DownstreamMaximum<Value> const& rhs)
        {
            return std::get<0>(lhs) > std::get<0>(rhs);
        }

        template<typename Value>
        struct CompareDownstreamMaxima
        {
                bool operator()(
                    DownstreamMaximum<Value> const& lhs, DownstreamMaximum<Value> const& rhs) const
                {
                    return lhs > rhs;
                }
        };


        namespace server {

            template<typename Policies, Rank rank>
            class DecreasingOrderData
            {

                private:

                    using RouteID = policy::InputElementT<Policies, 0>;

                    using Value = policy::InputElementT<Policies, 1>;

                public:

                    using ValuePartition = lue::ArrayPartition<Value, rank>;

                    using Offset = typename SerialRoutePartition<RouteID, rank>::Offset;

                    using Shape = typename SerialRoutePartition<RouteID, rank>::Shape;

                    using RoutePartition = lue::SerialRoutePartition<RouteID, rank>;

                    using RouteFragments = typename RoutePartition::RouteFragments;

                    using RouteFragment = typename RoutePartition::RouteFragment;

                    using RouteFragmentLocation = typename RouteFragment::Location;

                    using CellIdxs = typename RouteFragment::CellIdxs;


                    DecreasingOrderData(Offset const& offset, Shape const& shape):

                        _route_partition{hpx::find_here(), offset, shape},
                        _route_partition_p{},
                        _record_route_fragment_mutex{},
                        _route_fragments{},
                        _route_fragment_ends{},
                        _route_fragment_ends_idxs{}

                    {
                    }


                    /*!
                        @brief      Return the route partition client instance
                        @warning    Call this function only once.


                        The instance returned isn't pointing to a server instance yet, but
                        we'll promise to fix that later.
                    */
                    auto route_partition() -> RoutePartition
                    {
                        return RoutePartition{_route_partition_p.get_future()};
                    }


                    void finish_partition()
                    {
                        // This function is only accessed once. It must be the last function
                        // called because member variables are moved from.

                        // Wait for all route fragment continuations to finish and end the
                        // associated route fragments
                        lue_hpx_assert(all_are_valid(_route_fragment_ends));

                        hpx::when_all(_route_fragment_ends.begin(), _route_fragment_ends.end())
                            .then(
                                [route_fragment_ends_idxs = std::move(_route_fragment_ends_idxs),
                                 route_fragments = std::move(_route_fragments),
                                 route_partition = std::move(_route_partition),
                                 route_partition_p =
                                     std::move(_route_partition_p)](auto&& route_fragment_ends_f) mutable
                                {
                                    // All ready downstream fragment locations
                                    std::vector<hpx::future<RouteFragmentLocation>> route_fragment_ends{
                                        route_fragment_ends_f.get()};

                                    // Iterate over all downstream fragment locations
                                    for (auto const& [route_id, idxs] : route_fragment_ends_idxs)
                                    {
                                        // For the current route, iterate over all downstream
                                        // fragment locations
                                        for (auto const& [fragment_idx, fragment_end_idx] : idxs)
                                        {
                                            lue_hpx_assert(
                                                fragment_idx <
                                                static_cast<Index>(std::size(route_fragments[route_id])));
                                            lue_hpx_assert(
                                                fragment_end_idx <
                                                static_cast<Index>(std::size(route_fragment_ends)));

                                            RouteFragmentLocation route_fragment_end{
                                                route_fragment_ends[fragment_end_idx].get()};
                                            lue_hpx_assert(route_fragment_end.valid());

                                            route_fragments[route_id][fragment_idx].end(route_fragment_end);
                                        }
                                    }

                                    // Write all route fragments to the partition
                                    lue_hpx_assert(route_partition.valid());
                                    auto route_partition_ptr{ready_component_ptr(route_partition)};
                                    route_partition_ptr->set_route_fragments(std::move(route_fragments));

                                    // Done with this partition
                                    route_partition_p.set_value(route_partition.get_id());
                                });

                        _route_fragments.clear();
                        _route_fragment_ends.clear();
                        _route_fragment_ends_idxs.clear();
                    }


                    // protected:

                    //! The route partition that will be filled with route fragments
                    RoutePartition _route_partition;

                    //! Promise for the ID of the route partition server component
                    hpx::promise<hpx::id_type> _route_partition_p;

                    hpx::shared_mutex _record_route_fragment_mutex;

                    RouteFragments _route_fragments;

                    std::vector<hpx::future<RouteFragmentLocation>> _route_fragment_ends;

                    std::map<RouteID, std::vector<std::tuple<Index, Index>>> _route_fragment_ends_idxs;
            };


            template<typename Policies, Rank rank>
            class DecreasingOrderZonal:
                public hpx::components::component_base<DecreasingOrderZonal<Policies, rank>>
            {

                private:

                    using Data = DecreasingOrderData<Policies, rank>;

                    using ComponentServerBase =
                        hpx::components::component_base<DecreasingOrderZonal<Policies, rank>>;

                    using RouteID = policy::InputElementT<Policies, 0>;

                    using Zone = RouteID;

                    using Value = policy::InputElementT<Policies, 1>;

                public:

                    using Offset = typename Data::Offset;

                    using Shape = typename Data::Shape;

                    using RoutePartition = typename Data::RoutePartition;

                    using ValuePartition = typename Data::ValuePartition;

                    using RouteFragments = typename Data::RouteFragments;

                    using RouteFragment = typename Data::RouteFragment;

                    using RouteFragmentLocation = typename Data::RouteFragmentLocation;

                    using CellIdxs = typename Data::CellIdxs;

                    using ZonePartition = lue::ArrayPartition<Zone, rank>;


                    DecreasingOrderZonal(Offset const& offset, Shape const& shape):

                        ComponentServerBase{},
                        _data{offset, shape},
                        _local_values_by_zone{}

                    {
                    }


                    auto route_partition() -> RoutePartition
                    {
                        return _data.route_partition();
                    }


                    /*!
                        @brief      Sort the values within each zone
                        @return     Per zone the maximum value

                        The values themselves are stored in a member variable for later use.
                    */
                    std::map<Zone, Value> sort_values(
                        Policies const& policies,
                        ZonePartition const& zone_partition,
                        ValuePartition const& value_partition)
                    {
                        // This function is only accessed by a single thread at the same time

                        lue_hpx_assert(zone_partition.is_ready());
                        lue_hpx_assert(value_partition.is_ready());

                        auto const& zone_ndp = std::get<0>(policies.inputs_policies()).input_no_data_policy();
                        auto const& value_ndp =
                            std::get<1>(policies.inputs_policies()).input_no_data_policy();

                        auto const zone_partition_data{ready_component_ptr(zone_partition)->data()};
                        auto const value_partition_data{ready_component_ptr(value_partition)->data()};

                        Count const nr_elements{lue::nr_elements(value_partition_data)};
                        lue_hpx_assert(lue::nr_elements(value_partition_data) == nr_elements);

                        // Iterate over all zones and values. Per cell with a valid zone and value:
                        // - Create empty {value, cell_idxs} collection for the zone, if it
                        //   does not already exist
                        // - Add the value to this collection
                        for (Index idx = 0; idx < nr_elements; ++idx)
                        {
                            if (!zone_ndp.is_no_data(zone_partition_data, idx) &&
                                !value_ndp.is_no_data(value_partition_data, idx))
                            {
                                _local_values_by_zone[zone_partition_data[idx]].push_back(
                                    std::make_tuple(value_partition_data[idx], idx));
                            }
                        }

                        // Create an empty {zone, value} map for storing the maximum value per zone
                        std::map<Zone, Value> result{};

                        // Iterate over all {value, cell_idxs} collections. Per collection:
                        // - Sort the {value, cell_idxs} tuples, by value, in decreasing order
                        // - Copy the highest value to the {zone, value} map

                        // Per zone, sort all values in decreasing order, and store the maximum
                        // value in the result map
                        for (auto& [zone, values] : _local_values_by_zone)
                        {
                            std::sort(values.begin(), values.end(), CompareLocalMaxima<Value>{});

                            lue_hpx_assert(!values.empty());  // Cannot be
                            result[zone] = std::get<0>(values.front());
                        }

                        if (_local_values_by_zone.empty())
                        {
                            _data.finish_partition();
                        }

                        return result;
                    }


                    /*!
                        @brief      Record a route fragment
                        @return     Location of the route fragment
                    */
                    RouteFragmentLocation record_route_fragment(
                        RouteID const route_id,
                        DownstreamMaxima<Value>&& downstream_maxima,
                        Count current_nr_cells,
                        Count const max_nr_cells)
                    {
                        // This function is possibly accessed by multiple threads at the same
                        // time, for different routes and possibly even for the same route
                        // (but they will be ordered, handling upstream fragments first).
                        // All code referencing member variables must be guarded by a lock.

                        std::shared_lock read_lock{_data._record_route_fragment_mutex, std::defer_lock};
                        std::unique_lock write_lock{_data._record_route_fragment_mutex, std::defer_lock};

                        // Record route fragment. This is a read-only section. We are not
                        // updating member variables here. Multiple threads can be executing this
                        // section at the same time.
                        read_lock.lock();

                        lue_hpx_assert(_local_values_by_zone.find(route_id) != _local_values_by_zone.end());

                        // Reference, later on we will be updating this collection
                        LocalValues<Value>& local_values{_local_values_by_zone.find(route_id)->second};

                        // Otherwise we should not be here at all
                        lue_hpx_assert(!downstream_maxima.empty());
                        lue_hpx_assert(!local_values.empty());

                        // Current maximum corresponds with this partition's first value and ID
                        lue_hpx_assert(std::get<0>(downstream_maxima[0]) == std::get<0>(local_values[0]));
                        lue_hpx_assert(std::get<1>(downstream_maxima[0]) == this->get_id());

                        Value remote_value{std::numeric_limits<Value>::min()};
                        hpx::id_type downstream_component_id{};

                        if (downstream_maxima.size() > 1)
                        {
                            std::tie(remote_value, downstream_component_id) = downstream_maxima[1];

                            // Value in other partition must be equal or lower then this
                            // partition's first value
                            lue_hpx_assert(remote_value <= std::get<0>(local_values[0]));
                            lue_hpx_assert(downstream_component_id != this->get_id());
                        }

                        // Collection for storing idxs of cells in the current partition
                        // containing values that are all higher than or equal to the highest
                        // value in any other partition
                        CellIdxs cell_idxs{};

                        // Add as many local values to the route fragment as possible. This
                        // means, all values that are higher or equal to the first remote maximum.

                        // This variable ends up pointing to the one-past-the-last value
                        // not part of the recorded fragment (this value may not exist)
                        Index local_value_idx{0};

                        {
                            Index const nr_local_values{static_cast<Index>(std::size(local_values))};

                            for (; local_value_idx < nr_local_values && current_nr_cells < max_nr_cells;
                                 ++local_value_idx, ++current_nr_cells)
                            {
                                auto [local_value, local_idx] = local_values[local_value_idx];

                                if (local_value >= remote_value)
                                {
                                    cell_idxs.push_back(local_idx);
                                }
                                else
                                {
                                    break;
                                }
                            }
                        }

                        RouteFragment route_fragment{std::move(cell_idxs)};

                        // Pop current record from the front of the downstream_maxima collection. We
                        // just handled it.
                        auto maximum_and_component_id = downstream_maxima[0];
                        downstream_maxima.erase(downstream_maxima.begin());

                        // Done recording route fragment and querying state
                        read_lock.unlock();

                        // Now we need to update member variables. Only one thread can do this
                        // at any given moment. This will block until all threads are finished
                        // executing the previous section.
                        write_lock.lock();

                        local_values.erase(local_values.begin(), local_values.begin() + local_value_idx);

                        bool const continue_downstream{
                            current_nr_cells < max_nr_cells && !downstream_maxima.empty()};

                        if (!continue_downstream)
                        {
                            notify_skip_recording_route_fragments(route_id, std::move(downstream_maxima));

                            // Done with this route, but not all local values are used. Get rid
                            // of the pair in the map anyway.
                            // Note: local_values dangles from now on!
                            _local_values_by_zone.erase(route_id);

                            // Get rid of any remaining remote maxima. They are not needed
                            // anymore. We'll never reach them.
                            downstream_maxima.clear();
                        }
                        else
                        {
                            bool const local_values_empty{local_values.empty()};

                            if (local_values_empty)
                            {
                                // This partition is done with this route and all local values are
                                // used. Get rid of the pair in the map.
                                // Note: local_values dangles from now on!
                                _local_values_by_zone.erase(route_id);
                            }
                            else
                            {
                                // We may get back here (depending on current_nr_cells vs
                                // max_nr_cells), to record remaining values. We need to insert
                                // our new maximum value back to the downstream_maxima collection.

                                // Grab new maximum. This is the first one in our collection.
                                Value const new_maximum = std::get<0>(local_values[0]);

                                lue_hpx_assert(new_maximum < std::get<0>(downstream_maxima.front()));

                                // Update current record with new maximum
                                std::get<0>(maximum_and_component_id) = new_maximum;

                                // Insert record in a position that maintains the decreasing order of maxima
                                downstream_maxima.insert(
                                    // Find first element in downstream_maxima collection for which its
                                    // maximum is smaller or equal than our new_maximum. This is where we
                                    // want to insert the updated record.
                                    std::lower_bound(
                                        downstream_maxima.begin(),
                                        downstream_maxima.end(),
                                        new_maximum,
                                        [](auto const& maximum_and_component_id, Value const new_maximum)
                                        { return std::get<0>(maximum_and_component_id) > new_maximum; }),
                                    maximum_and_component_id);
                            }

                            // Spawn a task on the remote component
                            lue_hpx_assert(downstream_component_id != this->get_id());

                            detail::DecreasingOrderZonal<Policies, rank> component{downstream_component_id};

                            auto downstream_route_fragment_location_f = component.record_route_fragment(
                                route_id, std::move(downstream_maxima), current_nr_cells, max_nr_cells);
                            lue_hpx_assert(downstream_route_fragment_location_f.valid());

                            // Store the future to the downstream route fragment location for later use
                            Index const fragment_idx{
                                static_cast<Index>(std::size(_data._route_fragments[route_id]))};
                            Index const fragment_end_idx{
                                static_cast<Index>(std::size(_data._route_fragment_ends))};

                            _data._route_fragment_ends.push_back(
                                std::move(downstream_route_fragment_location_f));
                            _data._route_fragment_ends_idxs[route_id].push_back(
                                std::make_tuple(fragment_idx, fragment_end_idx));
                        }

                        _data._route_fragments[route_id].push_back(route_fragment);

                        RouteFragmentLocation fragment_location{_data._route_partition.share()};

                        if (_local_values_by_zone.empty())
                        {
                            _data.finish_partition();
                        }

                        lue_hpx_assert(fragment_location.valid());

                        return fragment_location;
                    }


                    void skip_recording_route_fragments(RouteID const route_id)
                    {
                        // This function is possibly accessed by multiple threads at the same
                        // time, for different routes

                        std::unique_lock write_lock{_data._record_route_fragment_mutex};
                        lue_hpx_assert(_local_values_by_zone.find(route_id) != _local_values_by_zone.end());
                        _local_values_by_zone.erase(route_id);

                        if (_local_values_by_zone.empty())
                        {
                            _data.finish_partition();
                        }
                    }


                    HPX_DEFINE_COMPONENT_ACTION(DecreasingOrderZonal, route_partition, RoutePartitionAction);

                    HPX_DEFINE_COMPONENT_ACTION(DecreasingOrderZonal, sort_values, SortValuesAction);

                    HPX_DEFINE_COMPONENT_ACTION(
                        DecreasingOrderZonal, record_route_fragment, RecordRouteFragmentAction);

                    HPX_DEFINE_COMPONENT_ACTION(
                        DecreasingOrderZonal,
                        skip_recording_route_fragments,
                        SkipRecordingRouteFragmentsAction);

                private:

                    void notify_skip_recording_route_fragments(
                        RouteID const route_id, DownstreamMaxima<Value>&& downstream_maxima)
                    {
                        // Iterate over all downstream maxima and notify the associated component
                        // that recording of route fragments for the current route will be
                        // skipped. Not notifying them may prevent the downstream route
                        // partition from ever being finished.

                        for (auto const& [value, downstream_component_id] : downstream_maxima)
                        {
                            lue_hpx_assert(downstream_component_id != this->get_id());

                            detail::DecreasingOrderZonal<Policies, rank> component{downstream_component_id};

                            component.skip_recording_route_fragments(route_id);
                        }
                    }


                    Data _data;

                    std::map<Zone, LocalValues<Value>> _local_values_by_zone;
            };

        }  // namespace server


        template<typename Policies, Rank rank>
        class DecreasingOrderZonal:
            public hpx::components::client_base<
                DecreasingOrderZonal<Policies, rank>,
                server::DecreasingOrderZonal<Policies, rank>>

        {

            public:

                using Server = server::DecreasingOrderZonal<Policies, rank>;

            private:

                using ComponentClientBase =
                    hpx::components::client_base<DecreasingOrderZonal<Policies, rank>, Server>;

                using RouteID = policy::InputElementT<Policies, 0>;

                using Zone = RouteID;

                using Value = policy::InputElementT<Policies, 1>;

            public:

                using ZonePartition = typename Server::ZonePartition;

                using ValuePartition = typename Server::ValuePartition;

                using RoutePartition = SerialRoutePartition<RouteID, rank>;

                using RouteFragment = SerialRouteFragment<rank>;

                using RouteFragmentLocation = typename RouteFragment::Location;

                using Offset = typename Server::Offset;

                using Shape = typename Server::Shape;


                DecreasingOrderZonal():

                    ComponentClientBase{}

                {
                }


                DecreasingOrderZonal(hpx::id_type const component_id):

                    ComponentClientBase{component_id}

                {
                }


                DecreasingOrderZonal(hpx::future<hpx::id_type>&& component_id):

                    ComponentClientBase{std::move(component_id)}

                {
                }


                DecreasingOrderZonal(Offset const& offset, Shape const& shape):

                    ComponentClientBase{offset, shape}

                {
                }


                DecreasingOrderZonal(DecreasingOrderZonal const&) = default;

                DecreasingOrderZonal(DecreasingOrderZonal&&) = default;

                ~DecreasingOrderZonal() = default;

                DecreasingOrderZonal& operator=(DecreasingOrderZonal const&) = default;

                DecreasingOrderZonal& operator=(DecreasingOrderZonal&&) = default;


                auto route_partition() const -> RoutePartition
                {
                    lue_hpx_assert(this->is_ready());
                    lue_hpx_assert(this->get_id());

                    typename Server::RoutePartitionAction action;

                    return hpx::async(action, this->get_id());
                }


                hpx::future<std::map<Zone, Value>> sort_values(
                    Policies const& policies,
                    ZonePartition const& zone_partition,
                    ValuePartition const& value_partition) const
                {
                    lue_hpx_assert(this->is_ready());
                    lue_hpx_assert(this->get_id());

                    typename Server::SortValuesAction action;

                    return hpx::async(action, this->get_id(), policies, zone_partition, value_partition);
                }


                hpx::future<RouteFragmentLocation> record_route_fragment(
                    RouteID const route_id,
                    DownstreamMaxima<Value>&& downstream_maxima,
                    Count current_nr_cells,
                    Count const max_nr_cells)
                {
                    lue_hpx_assert(this->is_ready());
                    lue_hpx_assert(this->get_id());

                    typename Server::RecordRouteFragmentAction action;

                    return hpx::async(
                        action,
                        this->get_id(),
                        route_id,
                        std::move(downstream_maxima),
                        current_nr_cells,
                        max_nr_cells);
                }


                hpx::future<void> skip_recording_route_fragments(RouteID const route_id)
                {
                    lue_hpx_assert(this->is_ready());
                    lue_hpx_assert(this->get_id());

                    typename Server::SkipRecordingRouteFragmentsAction action;

                    return hpx::async(action, this->get_id(), route_id);
                }
        };


        namespace server {

            template<typename Policies, Rank rank>
            class DecreasingOrderGlobal:
                public hpx::components::component_base<DecreasingOrderGlobal<Policies, rank>>
            {

                private:

                    using Data = DecreasingOrderData<Policies, rank>;

                    using ComponentServerBase =
                        hpx::components::component_base<DecreasingOrderGlobal<Policies, rank>>;

                    using RouteID = policy::InputElementT<Policies, 0>;

                    using Value = policy::InputElementT<Policies, 1>;

                public:

                    using Offset = typename Data::Offset;

                    using Shape = typename Data::Shape;

                    using RoutePartition = typename Data::RoutePartition;

                    using ValuePartition = typename Data::ValuePartition;

                    using RouteFragments = typename Data::RouteFragments;

                    using RouteFragment = typename Data::RouteFragment;

                    using RouteFragmentLocation = typename Data::RouteFragmentLocation;

                    using CellIdxs = typename Data::CellIdxs;


                    /*!
                        @brief      Construct a component instance
                        @param      offset Offset of route partition
                        @param      offset Shape of route partition
                    */
                    DecreasingOrderGlobal(Offset const& offset, Shape const& shape):

                        ComponentServerBase{},
                        _data{offset, shape},
                        _local_values{}

                    {
                    }


                    DecreasingOrderGlobal(DecreasingOrderGlobal const&) = default;

                    DecreasingOrderGlobal(DecreasingOrderGlobal&&) = default;

                    ~DecreasingOrderGlobal() = default;

                    DecreasingOrderGlobal& operator=(DecreasingOrderGlobal const&) = default;

                    DecreasingOrderGlobal& operator=(DecreasingOrderGlobal&&) = default;


                    auto route_partition() -> RoutePartition
                    {
                        return _data.route_partition();
                    }


                    /*!
                        @brief      Sort the values
                        @return     The maximum value

                        The values themselves are stored in a member variable for later use.
                    */
                    std::optional<Value> sort_values(
                        Policies const& policies, ValuePartition const& value_partition)
                    {
                        // This function is only accessed by a single thread at the same time

                        lue_hpx_assert(value_partition.is_ready());

                        auto const& value_ndp =
                            std::get<1>(policies.inputs_policies()).input_no_data_policy();

                        auto const value_partition_data{ready_component_ptr(value_partition)->data()};

                        Count const nr_elements{lue::nr_elements(value_partition_data)};
                        lue_hpx_assert(lue::nr_elements(value_partition_data) == nr_elements);

                        // Iterate over all values and add all valid values and their idx to
                        // the collection
                        for (Index idx = 0; idx < nr_elements; ++idx)
                        {
                            if (!value_ndp.is_no_data(value_partition_data, idx))
                            {
                                _local_values.push_back(std::make_tuple(value_partition_data[idx], idx));
                            }
                        }

                        std::optional<Value> maximum{};

                        if (_local_values.empty())
                        {
                            _data.finish_partition();
                        }
                        else
                        {
                            // Sort all values in decreasing order, and grab the maximum value
                            std::sort(
                                _local_values.begin(), _local_values.end(), CompareLocalMaxima<Value>{});

                            maximum = std::get<0>(_local_values.front());
                        }

                        return maximum;
                    }


                    /*!
                        @brief      Record a route fragment
                        @return     Location of the route fragment
                    */
                    RouteFragmentLocation record_route_fragment(
                        RouteID const route_id,
                        DownstreamMaxima<Value>&& downstream_maxima,
                        Count current_nr_cells,
                        Count const max_nr_cells)
                    {
                        // This function is possibly accessed by multiple threads at the same
                        // time for the same route (but they will be ordered, handling upstream
                        // fragments first).
                        // All code referencing member variables must be guarded by a lock.

                        std::shared_lock read_lock{_data._record_route_fragment_mutex, std::defer_lock};
                        std::unique_lock write_lock{_data._record_route_fragment_mutex, std::defer_lock};

                        // Record route fragment. This is a read-only section. We are not
                        // updating member variables here. Multiple threads can be executing this
                        // section at the same time.
                        read_lock.lock();

                        // Reference, later on we will be updating this collection
                        LocalValues<Value>& local_values{_local_values};

                        // Otherwise we should not be here at all
                        lue_hpx_assert(!downstream_maxima.empty());
                        lue_hpx_assert(!local_values.empty());

                        // Current maximum corresponds with this partition's first value and ID
                        lue_hpx_assert(std::get<0>(downstream_maxima[0]) == std::get<0>(local_values[0]));
                        lue_hpx_assert(std::get<1>(downstream_maxima[0]) == this->get_id());

                        Value remote_value{std::numeric_limits<Value>::min()};
                        hpx::id_type downstream_component_id{};

                        if (downstream_maxima.size() > 1)
                        {
                            std::tie(remote_value, downstream_component_id) = downstream_maxima[1];

                            // Value in other partition must be equal or lower then this
                            // partition's first value
                            lue_hpx_assert(remote_value <= std::get<0>(local_values[0]));
                            lue_hpx_assert(downstream_component_id != this->get_id());
                        }

                        // Collection for storing idxs of cells in the current partition
                        // containing values that are all higher than or equal to the highest
                        // value in any other partition
                        CellIdxs cell_idxs{};

                        // Add as many local values to the route fragment as possible. This
                        // means, all values that are higher or equal to the first remote maximum.

                        // This variable ends up pointing to the one-past-the-last value
                        // not part of the recorded fragmement (this value may not exist)
                        Index local_value_idx{0};

                        {
                            Index const nr_local_values{static_cast<Index>(std::size(local_values))};

                            for (; local_value_idx < nr_local_values && current_nr_cells < max_nr_cells;
                                 ++local_value_idx, ++current_nr_cells)
                            {
                                auto [local_value, local_idx] = local_values[local_value_idx];

                                if (local_value >= remote_value)
                                {
                                    cell_idxs.push_back(local_idx);
                                }
                                else
                                {
                                    break;
                                }
                            }
                        }

                        RouteFragment route_fragment{std::move(cell_idxs)};

                        // Pop current record from the front of the downstream_maxima collection. We
                        // just handled it.
                        auto maximum_and_component_id = downstream_maxima[0];
                        downstream_maxima.erase(downstream_maxima.begin());

                        // Done recording route fragment and querying state
                        read_lock.unlock();

                        // Now we need to update member variables. Only one thread can do this
                        // at any given moment. This will block until all threads are finished
                        // executing the previous section.
                        write_lock.lock();

                        local_values.erase(local_values.begin(), local_values.begin() + local_value_idx);

                        bool const continue_downstream{
                            current_nr_cells < max_nr_cells && !downstream_maxima.empty()};

                        if (!continue_downstream)
                        {
                            notify_skip_recording_route_fragments(route_id, std::move(downstream_maxima));

                            // Done with this route, but not all local values are used
                            _local_values.clear();

                            // Get rid of any remaining remote maxima. They are not needed
                            // anymore. We'll never reach them.
                            // The move likely handled this already. Also, we don't use this
                            // collection anymore.
                            downstream_maxima.clear();
                        }
                        else
                        {
                            bool const local_values_empty{local_values.empty()};

                            if (!local_values_empty)
                            {
                                // We may get back here (depending on current_nr_cells vs
                                // max_nr_cells), to record remaining values. We need to insert
                                // our new maximum value back to the downstream_maxima collection.

                                // Grab new maximum. This is the first one in our collection.
                                Value const new_maximum = std::get<0>(local_values[0]);

                                lue_hpx_assert(new_maximum < std::get<0>(downstream_maxima.front()));

                                // Update current record with new maximum
                                std::get<0>(maximum_and_component_id) = new_maximum;

                                // Insert record in a position that maintains the decreasing order of maxima
                                downstream_maxima.insert(
                                    // Find first element in downstream_maxima collection for which its
                                    // maximum is smaller or equal than our new_maximum. This is where we
                                    // want to insert the updated record.
                                    std::lower_bound(
                                        downstream_maxima.begin(),
                                        downstream_maxima.end(),
                                        new_maximum,
                                        [](auto const& maximum_and_component_id, Value const new_maximum)
                                        { return std::get<0>(maximum_and_component_id) > new_maximum; }),
                                    maximum_and_component_id);
                            }

                            // Spawn a task on the remote component
                            lue_hpx_assert(downstream_component_id != this->get_id());

                            detail::DecreasingOrderGlobal<Policies, rank> component{downstream_component_id};

                            auto downstream_route_fragment_location_f = component.record_route_fragment(
                                route_id, std::move(downstream_maxima), current_nr_cells, max_nr_cells);
                            lue_hpx_assert(downstream_route_fragment_location_f.valid());

                            // Store the future to the downstream route fragment location for later use
                            Index const fragment_idx{
                                static_cast<Index>(std::size(_data._route_fragments[route_id]))};
                            Index const fragment_end_idx{
                                static_cast<Index>(std::size(_data._route_fragment_ends))};

                            _data._route_fragment_ends.push_back(
                                std::move(downstream_route_fragment_location_f));
                            _data._route_fragment_ends_idxs[route_id].push_back(
                                std::make_tuple(fragment_idx, fragment_end_idx));
                        }

                        _data._route_fragments[route_id].push_back(route_fragment);

                        RouteFragmentLocation fragment_location{_data._route_partition.share()};

                        if (_local_values.empty())
                        {
                            _data.finish_partition();
                        }

                        lue_hpx_assert(fragment_location.valid());

                        return fragment_location;
                    }


                    void skip_recording_route_fragments([[maybe_unused]] RouteID const route_id)
                    {
                        // This function is never accessed by multiple threads at the same
                        // time. There is only a single route.
                        lue_hpx_assert(!_local_values.empty());
                        _local_values.clear();
                        _data.finish_partition();
                    }


                    HPX_DEFINE_COMPONENT_ACTION(DecreasingOrderGlobal, route_partition, RoutePartitionAction);

                    HPX_DEFINE_COMPONENT_ACTION(DecreasingOrderGlobal, sort_values, SortValuesAction);

                    HPX_DEFINE_COMPONENT_ACTION(
                        DecreasingOrderGlobal, record_route_fragment, RecordRouteFragmentAction);

                    HPX_DEFINE_COMPONENT_ACTION(
                        DecreasingOrderGlobal,
                        skip_recording_route_fragments,
                        SkipRecordingRouteFragmentsAction);

                private:

                    void notify_skip_recording_route_fragments(
                        RouteID const route_id, DownstreamMaxima<Value>&& downstream_maxima)
                    {
                        // Iterate over all downstream maxima and notify the associated component
                        // that recording of route fragments for the current route will be
                        // skipped. Not notifying them may prevent the downstream route
                        // partition from ever being finished.

                        for (auto const& [value, downstream_component_id] : downstream_maxima)
                        {
                            detail::DecreasingOrderGlobal<Policies, rank> component{downstream_component_id};

                            component.skip_recording_route_fragments(route_id);
                        }
                    }


                    Data _data;

                    LocalValues<Value> _local_values;
            };

        }  // namespace server


        template<typename Policies, Rank rank>
        class DecreasingOrderGlobal:
            public hpx::components::client_base<
                DecreasingOrderGlobal<Policies, rank>,
                server::DecreasingOrderGlobal<Policies, rank>>

        {

            public:

                using Server = server::DecreasingOrderGlobal<Policies, rank>;

            private:

                using ComponentClientBase =
                    hpx::components::client_base<DecreasingOrderGlobal<Policies, rank>, Server>;

                using RouteID = policy::InputElementT<Policies, 0>;

                using Value = policy::InputElementT<Policies, 1>;

            public:

                using ValuePartition = typename Server::ValuePartition;

                using RoutePartition = SerialRoutePartition<RouteID, rank>;

                using RouteFragment = SerialRouteFragment<rank>;

                using RouteFragmentLocation = typename RouteFragment::Location;

                using Offset = typename Server::Offset;

                using Shape = typename Server::Shape;


                DecreasingOrderGlobal():

                    ComponentClientBase{}

                {
                }


                DecreasingOrderGlobal(hpx::id_type const component_id):

                    ComponentClientBase{component_id}

                {
                }


                DecreasingOrderGlobal(hpx::future<hpx::id_type>&& component_id):

                    ComponentClientBase{std::move(component_id)}

                {
                }


                DecreasingOrderGlobal(Offset const& offset, Shape const& shape):

                    ComponentClientBase{offset, shape}

                {
                }


                DecreasingOrderGlobal(DecreasingOrderGlobal const&) = default;

                DecreasingOrderGlobal(DecreasingOrderGlobal&&) = default;

                ~DecreasingOrderGlobal() = default;

                DecreasingOrderGlobal& operator=(DecreasingOrderGlobal const&) = default;

                DecreasingOrderGlobal& operator=(DecreasingOrderGlobal&&) = default;


                RoutePartition route_partition() const
                {
                    lue_hpx_assert(this->is_ready());
                    lue_hpx_assert(this->get_id());

                    typename Server::RoutePartitionAction action;

                    return hpx::async(action, this->get_id());
                }


                hpx::future<std::optional<Value>> sort_values(
                    Policies const& policies, ValuePartition const& value_partition) const
                {
                    lue_hpx_assert(this->is_ready());
                    lue_hpx_assert(this->get_id());

                    typename Server::SortValuesAction action;

                    return hpx::async(action, this->get_id(), policies, value_partition);
                }


                hpx::future<RouteFragmentLocation> record_route_fragment(
                    RouteID const route_id,
                    DownstreamMaxima<Value>&& downstream_maxima,
                    Count current_nr_cells,
                    Count const max_nr_cells)
                {
                    lue_hpx_assert(this->is_ready());
                    lue_hpx_assert(this->get_id());

                    typename Server::RecordRouteFragmentAction action;

                    return hpx::async(
                        action,
                        this->get_id(),
                        route_id,
                        std::move(downstream_maxima),
                        current_nr_cells,
                        max_nr_cells);
                }


                hpx::future<void> skip_recording_route_fragments(RouteID const route_id)
                {
                    lue_hpx_assert(this->is_ready());
                    lue_hpx_assert(this->get_id());

                    typename Server::SkipRecordingRouteFragmentsAction action;

                    return hpx::async(action, this->get_id(), route_id);
                }
        };

    }  // namespace detail


    template<typename Policies, Rank rank>
    auto decreasing_order(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const& zone,
        PartitionedArray<policy::InputElementT<Policies, 1>, rank> const& value,
        Count const max_nr_cells) -> SerialRoute<policy::OutputElementT<Policies, 0>, rank>
    {
        // For each zone, create a route from the cell with the highest value to the cell
        // with the lowest value. Stop when the route has the same length as the maximum length
        // passed in.

        // Algorithm:
        //
        // 1. Per partition, sort all values per zone. Return the maxima per zone.
        // 2. For each zone, determine the partition in which the maximum value is located
        //    in. Store this information in the resulting serial route instance and start
        //    recording a route fragment in this start partition. Pass in the maxima of all
        //    partitions for that zone.
        // 3. Continue storing route fragments for as long as the current local maximum is
        //    higher than the global one, located in another partition.
        //    Once the local maximum is lower than the global one, continue with the procedure
        //    for the partition that has the global maximum.
        //    Continue until done, jumping from partition to partition. This is a (mostly)
        //    serial operation per zone, but can be done in parallel for each zone.
        //    Take max_nr_cells into account. Stop as soon as the current length of the route
        //    is equal to the max_nr_cells passed in. Keep track of the length of the route while
        //    recording it.

        static_assert(
            std::is_same_v<policy::OutputElementT<Policies, 0>, policy::InputElementT<Policies, 0>>);

        detail::verify_compatible(zone, value);

        using Zone = policy::InputElementT<Policies, 0>;
        using ZoneArray = PartitionedArray<Zone, rank>;
        using ZonePartitions = PartitionsT<ZoneArray>;
        using ZonePartition = PartitionT<ZoneArray>;

        using Value = policy::InputElementT<Policies, 1>;
        using ValueArray = PartitionedArray<Value, rank>;
        using ValuePartitions = PartitionsT<ValueArray>;
        using ValuePartition = PartitionT<ValueArray>;

        using RouteID = policy::OutputElementT<Policies, 0>;
        using Route = SerialRoute<RouteID, rank>;
        using RoutePartitions = PartitionsT2<Route>;
        using RoutePartition = PartitionT2<Route>;
        using RouteStarts = typename Route::Starts;
        using Shape = ShapeT<Route>;

        Shape const array_shape{value.shape()};
        Shape const shape_in_partitions{value.partitions().shape()};
        Count const nr_partitions{lue::nr_elements(value.partitions())};
        RoutePartitions route_partitions{shape_in_partitions};

        Localities<rank> const& localities{value.localities()};
        ZonePartitions const& zone_partitions{zone.partitions()};
        ValuePartitions const& value_partitions{value.partitions()};

        // Create collection of (futures to) components that will do all the work, distributed
        using Component = detail::DecreasingOrderZonal<Policies, rank>;

        std::vector<hpx::future<Component>> component_fs(nr_partitions);

        // Create collection of (futures to) maps, for storing (per partition) the maximum
        // value per zone
        using MaxValueByZone = std::map<Zone, Value>;
        using MaxValueByPartitionByZone = std::vector<hpx::future<MaxValueByZone>>;

        MaxValueByPartitionByZone max_value_fs(nr_partitions);

        for (Index partition_idx = 0; partition_idx < nr_partitions; ++partition_idx)
        {
            // Create component that will help with creating the routes. Tell it to sort the
            // input values, within each zone. This will result in:
            // - A future to the component
            // - The result partition (not ready yet)
            // - A future to a map containing (per partition) the maximum value per zone

            std::tie(
                component_fs[partition_idx], route_partitions[partition_idx], max_value_fs[partition_idx]) =
                hpx::split_future(
                    hpx::when_all(zone_partitions[partition_idx], value_partitions[partition_idx])
                        .then(hpx::unwrapping(

                            [policies, locality = localities[partition_idx]](auto&& tuple_f)
                            {
                                ZonePartition zone_partition{std::get<0>(tuple_f)};
                                ValuePartition value_partition{std::get<1>(tuple_f)};

                                return hpx::when_all(value_partition.offset(), value_partition.shape())
                                    .then(hpx::unwrapping(

                                        [policies, locality, zone_partition, value_partition](auto&& tuple_f)
                                        {
                                            auto const offset{std::get<0>(tuple_f).get()};
                                            auto const shape{std::get<1>(tuple_f).get()};

                                            // Create a component and tell it to sort values per zone
                                            return hpx::new_<Component>(locality, offset, shape)
                                                .then(

                                                    [policies, zone_partition, value_partition](
                                                        Component&& component)
                                                    {
                                                        RoutePartition partition_id_f{
                                                            component.route_partition()};
                                                        hpx::future<MaxValueByZone> max_value_fs{
                                                            component.sort_values(
                                                                policies, zone_partition, value_partition)};

                                                        return std::make_tuple(
                                                            std::move(component),
                                                            std::move(partition_id_f),
                                                            std::move(max_value_fs));
                                                    });
                                        }));
                            })));
        }

        // Given the information we have now, we can start recording route fragments

        auto components_ready_f = hpx::when_all(component_fs.begin(), component_fs.end());
        auto max_values_ready_f = hpx::when_all(max_value_fs.begin(), max_value_fs.end());

        auto [starts, components] = hpx::split_future(
            hpx::when_all(components_ready_f, max_values_ready_f)
                .then(hpx::unwrapping(

                    [max_nr_cells](auto&& tuple_f)
                    {
                        auto component_fs = std::get<0>(tuple_f).get();
                        auto max_value_fs = std::get<1>(tuple_f).get();

                        // Once all maximum values per zone are ready to be used, initialize each route

                        // First collect all maximum values per zone. Keep track of the component
                        // that handles the partition containing each value.

                        using DownstreamMaximaByZone = std::map<Zone, detail::DownstreamMaxima<Value>>;

                        DownstreamMaximaByZone downstream_maxima_by_zone{};

                        Count const nr_partitions = std::size(component_fs);

                        std::vector<Component> components(nr_partitions);

                        std::transform(
                            component_fs.begin(),
                            component_fs.end(),
                            components.begin(),
                            [](hpx::future<Component>& component_f)
                            {
                                lue_hpx_assert(component_f.is_ready());
                                return component_f.get();
                            });

                        for (Index partition_idx = 0; partition_idx < nr_partitions; ++partition_idx)
                        {
                            Component const& component{components[partition_idx]};
                            MaxValueByZone max_value{max_value_fs[partition_idx].get()};

                            for (auto const [zone, value] : max_value)
                            {
                                // Add the maximum value for the current partition and zone to the collection
                                downstream_maxima_by_zone[zone].push_back(
                                    std::make_tuple(value, component.get_id()));
                            }
                        }

                        // Per zone, sort all maximum values, in decreasing order
                        for (auto& [zone, downstream_maxima] : downstream_maxima_by_zone)
                        {
                            std::sort(
                                downstream_maxima.begin(),
                                downstream_maxima.end(),
                                detail::CompareDownstreamMaxima<Value>{});
                        }

                        std::vector<Zone> zones(downstream_maxima_by_zone.size());
                        std::vector<hpx::future<typename Route::FragmentLocation>> start_fs(
                            downstream_maxima_by_zone.size());

                        {
                            Index idx{0};

                            for (auto& [zone, values] : downstream_maxima_by_zone)
                            {
                                lue_hpx_assert(!values.empty());

                                Component component{std::get<1>(values.front())};

                                zones[idx] = zone;
                                start_fs[idx] = component.record_route_fragment(
                                    zone, std::move(values), Count{0}, max_nr_cells);

                                ++idx;
                            }
                        }

                        return hpx::when_all(start_fs.begin(), start_fs.end())
                            .then(
                                [zones = std::move(zones), components = std::move(components)](
                                    hpx::future<std::vector<hpx::future<typename Route::FragmentLocation>>>&&
                                        starts_ff)
                                {
                                    // Collect the route starts
                                    RouteStarts starts{};
                                    auto start_fs{starts_ff.get()};

                                    {
                                        Index const nr_zones{static_cast<Index>(std::size(zones))};

                                        for (Index idx = 0; idx < nr_zones; ++idx)
                                        {
                                            starts[zones[idx]] = start_fs[idx].get();
                                        }
                                    }

                                    return hpx::make_tuple(std::move(starts), std::move(components));
                                });
                    })));

        // As long as not all output route partitions are ready, we need to keep the components
        // alive
        hpx::when_all(route_partitions.begin(), route_partitions.end())
            .then([components = std::move(components)]([[maybe_unused]] auto&& partitions_f)
                  { HPX_UNUSED(components); });

        return Route{array_shape, std::move(starts), std::move(route_partitions)};
    }


    template<typename Policies, Rank rank>
    auto decreasing_order(
        Policies const& policies,
        PartitionedArray<policy::InputElementT<Policies, 1>, rank> const& value,
        Count const max_nr_cells) -> SerialRoute<policy::OutputElementT<Policies, 0>, rank>
    {
        // Create a single route from the cell with the highest value to the cell
        // with the lowest value. Stop when the route has the same length as the length passed in.

        // Algorithm:
        //
        // 1. Per partition, sort all values. Return the maxima.
        // 2. Determine the partition in which the maximum value is located in. Store this
        //    information in the resulting serial route instance and start recording a route
        //    fragment in this start partition. Pass in the maxima of all partitions.
        // 3. Continue storing route fragments for as long as the current local maximum is
        //    higher than the global one, located in another partition.
        //    Once the local maximum is lower than the global one, continue with the procedure
        //    for the partition that has the global maximum.
        //    Continue until done, jumping from partition to partition.
        //    Take max_nr_cells into account. Stop as soon as the current length of the route
        //    is equal to the max_nr_cells passed in. Keep track of the length of the route
        //    while recording it.

        static_assert(
            std::is_same_v<policy::OutputElementT<Policies, 0>, policy::InputElementT<Policies, 0>>);

        using Value = policy::InputElementT<Policies, 1>;
        using ValueArray = PartitionedArray<Value, rank>;
        using ValuePartitions = PartitionsT<ValueArray>;
        using ValuePartition = PartitionT<ValueArray>;

        using RouteID = policy::OutputElementT<Policies, 0>;
        using Route = SerialRoute<RouteID, rank>;
        using RoutePartitions = PartitionsT2<Route>;
        using RoutePartition = PartitionT2<Route>;
        using RouteStarts = typename Route::Starts;
        using Shape = ShapeT<Route>;

        Shape const array_shape{value.shape()};
        Shape const shape_in_partitions{value.partitions().shape()};
        Count const nr_partitions{lue::nr_elements(value.partitions())};
        RoutePartitions route_partitions{shape_in_partitions};

        Localities<rank> const& localities{value.localities()};
        ValuePartitions const& value_partitions{value.partitions()};

        // Create collection of (futures to) components that will do all the work, distributed
        using Component = detail::DecreasingOrderGlobal<Policies, rank>;

        std::vector<hpx::future<Component>> component_fs(nr_partitions);

        // Create collection of (futures to) optionals, for storing (per partition) the maximum
        // value per zone
        using MaxValue = std::optional<Value>;
        using MaxValueByPartition = std::vector<hpx::future<MaxValue>>;

        MaxValueByPartition max_value_fs(nr_partitions);

        for (Index partition_idx = 0; partition_idx < nr_partitions; ++partition_idx)
        {
            // Create component that will help with creating the routes. Tell it to sort the
            // input values. This will result in:
            // - A future to the component
            // - The result partition (not ready yet)
            // - A future to an optional containing (per partition) the maximum value

            std::tie(
                component_fs[partition_idx], route_partitions[partition_idx], max_value_fs[partition_idx]) =
                hpx::split_future(
                    hpx::when_all(value_partitions[partition_idx])
                        .then(hpx::unwrapping(

                            [policies, locality = localities[partition_idx]](auto&& tuple_f)
                            {
                                ValuePartition value_partition{std::get<0>(tuple_f)};

                                return hpx::when_all(value_partition.offset(), value_partition.shape())
                                    .then(hpx::unwrapping(

                                        [policies, locality, value_partition](auto&& tuple_f)
                                        {
                                            auto const offset{std::get<0>(tuple_f).get()};
                                            auto const shape{std::get<1>(tuple_f).get()};

                                            return hpx::new_<Component>(locality, offset, shape)
                                                .then(
                                                    [policies, value_partition](Component&& component)
                                                    {
                                                        RoutePartition partition_id_f{
                                                            component.route_partition()};
                                                        hpx::future<MaxValue> max_value_f{
                                                            component.sort_values(policies, value_partition)};

                                                        return std::make_tuple(
                                                            std::move(component),
                                                            std::move(partition_id_f),
                                                            std::move(max_value_f));
                                                    });
                                        }));
                            })));
        }

        // Given the information we have now, we can start recording route fragments

        Index const route_id{0};
        auto components_ready_f = hpx::when_all(component_fs.begin(), component_fs.end());
        auto max_values_ready_f = hpx::when_all(max_value_fs.begin(), max_value_fs.end());

        auto [starts, components] = hpx::split_future(
            hpx::when_all(components_ready_f, max_values_ready_f)
                .then(hpx::unwrapping(

                    [route_id, max_nr_cells](auto&& tuple_f)
                    {
                        auto component_fs = std::get<0>(tuple_f).get();
                        auto max_value_fs = std::get<1>(tuple_f).get();

                        // Once all maximum values are ready to be used, initialize the route

                        // First collect all maximum values. Keep track of the component
                        // that handles the partition containing each value.

                        using DownstreamMaxima = detail::DownstreamMaxima<Value>;

                        DownstreamMaxima downstream_maxima{};

                        Count const nr_partitions = std::size(component_fs);

                        std::vector<Component> components(nr_partitions);

                        std::transform(
                            component_fs.begin(),
                            component_fs.end(),
                            components.begin(),
                            [](hpx::future<Component>& component_f) { return component_f.get(); });

                        for (Index partition_idx = 0; partition_idx < nr_partitions; ++partition_idx)
                        {
                            Component const& component{components[partition_idx]};
                            MaxValue max_value{max_value_fs[partition_idx].get()};

                            if (max_value.has_value())
                            {
                                // Add the maximum value for the current partition to the collection
                                downstream_maxima.push_back(
                                    std::make_tuple(max_value.value(), component.get_id()));
                            }
                        }

                        // Sort all maximum values, in decreasing order
                        std::sort(
                            downstream_maxima.begin(),
                            downstream_maxima.end(),
                            detail::CompareDownstreamMaxima<Value>{});

                        Component component{std::get<1>(downstream_maxima.front())};

                        hpx::future<typename Route::FragmentLocation> start_f =
                            component.record_route_fragment(
                                route_id, std::move(downstream_maxima), Count{0}, max_nr_cells);

                        return start_f.then(
                            [route_id, components = std::move(components)](
                                hpx::future<typename Route::FragmentLocation>&& start_f)
                            {
                                RouteStarts starts{};

                                starts[route_id] = start_f.get();

                                return std::make_tuple(starts, components);
                            });
                    })));

        // As long as not all output route partitions are ready, we need to keep the components
        // alive
        hpx::when_all(route_partitions.begin(), route_partitions.end())
            .then([components = std::move(components)]([[maybe_unused]] auto&& partitions_f)
                  { HPX_UNUSED(components); });

        return Route{array_shape, std::move(starts), std::move(route_partitions)};
    }

}  // namespace lue


#define LUE_REGISTER_DECREASING_ORDER_ACTION_DECLARATIONS_ZONAL(Policies, rank, unique)                      \
                                                                                                             \
    namespace lue::detail {                                                                                  \
        using DecreasingOrderZonalServer_##unique =                                                          \
            lue::detail::server::DecreasingOrderZonal<Policies, rank>;                                       \
    }                                                                                                        \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::DecreasingOrderZonalServer_##unique::RoutePartitionAction,                              \
        DecreasingOrderZonalServerRoutePartitionAction_##unique)                                             \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::DecreasingOrderZonalServer_##unique::SortValuesAction,                                  \
        DecreasingOrderZonalServerSortValuesAction_##unique)                                                 \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::DecreasingOrderZonalServer_##unique::RecordRouteFragmentAction,                         \
        DecreasingOrderZonalServerRecordRouteFragmentAction_##unique)                                        \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::DecreasingOrderZonalServer_##unique::SkipRecordingRouteFragmentsAction,                 \
        DecreasingOrderZonalServerSkipRecordingRouteFragmentsAction_##unique)


#define LUE_REGISTER_DECREASING_ORDER_ACTIONS_ZONAL(Policies, rank, unique)                                  \
                                                                                                             \
    namespace lue::detail {                                                                                  \
        using DecreasingOrderZonalServerComponent_##unique =                                                 \
            hpx::components::component<lue::detail::server::DecreasingOrderZonal<Policies, rank>>;           \
    }                                                                                                        \
                                                                                                             \
    HPX_REGISTER_COMPONENT(                                                                                  \
        lue::detail::DecreasingOrderZonalServerComponent_##unique,                                           \
        DecreasingOrderZonalServerComponent_##unique)                                                        \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::DecreasingOrderZonalServer_##unique::RoutePartitionAction,                              \
        DecreasingOrderZonalServerRoutePartitionAction_##unique)                                             \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::DecreasingOrderZonalServer_##unique::SortValuesAction,                                  \
        DecreasingOrderZonalServerSortValuesAction_##unique)                                                 \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::DecreasingOrderZonalServer_##unique::RecordRouteFragmentAction,                         \
        DecreasingOrderZonalServerRecordRouteFragmentAction_##unique)                                        \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::DecreasingOrderZonalServer_##unique::SkipRecordingRouteFragmentsAction,                 \
        DecreasingOrderZonalServerSkipRecordingRouteFragmentsAction_##unique)


#define LUE_INSTANTIATE_DECREASING_ORDER_ZONAL(Policies, rank)                                               \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT SerialRoute<policy::OutputElementT<Policies, 0>, rank>             \
    decreasing_order<ArgumentType<void(Policies)>, rank>(                                                    \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 0>, rank> const&,                                   \
        PartitionedArray<policy::InputElementT<Policies, 1>, rank> const&,                                   \
        Count const max_nr_cells);


#define LUE_REGISTER_DECREASING_ORDER_ACTION_DECLARATIONS_GLOBAL(Policies, rank, unique)                     \
                                                                                                             \
    namespace lue::detail {                                                                                  \
        using DecreasingOrderGlobalServer_##unique =                                                         \
            lue::detail::server::DecreasingOrderGlobal<Policies, rank>;                                      \
    }                                                                                                        \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::DecreasingOrderGlobalServer_##unique::RoutePartitionAction,                             \
        DecreasingOrderGlobalServerRoutePartitionAction_##unique)                                            \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::DecreasingOrderGlobalServer_##unique::SortValuesAction,                                 \
        DecreasingOrderGlobalServerSortValuesAction_##unique)                                                \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::DecreasingOrderGlobalServer_##unique::RecordRouteFragmentAction,                        \
        DecreasingOrderGlobalServerRecordRouteFragmentAction_##unique)                                       \
                                                                                                             \
    HPX_REGISTER_ACTION_DECLARATION(                                                                         \
        lue::detail::DecreasingOrderGlobalServer_##unique::SkipRecordingRouteFragmentsAction,                \
        DecreasingOrderGlobalServerSkipRecordingRouteFragmentsAction_##unique)


#define LUE_REGISTER_DECREASING_ORDER_ACTIONS_GLOBAL(Policies, rank, unique)                                 \
                                                                                                             \
    namespace lue::detail {                                                                                  \
        using DecreasingOrderGlobalServerComponent_##unique =                                                \
            hpx::components::component<lue::detail::server::DecreasingOrderGlobal<Policies, rank>>;          \
    }                                                                                                        \
                                                                                                             \
    HPX_REGISTER_COMPONENT(                                                                                  \
        lue::detail::DecreasingOrderGlobalServerComponent_##unique,                                          \
        DecreasingOrderGlobalServerComponent_##unique)                                                       \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::DecreasingOrderGlobalServer_##unique::RoutePartitionAction,                             \
        DecreasingOrderGlobalServerRoutePartitionAction_##unique)                                            \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::DecreasingOrderGlobalServer_##unique::SortValuesAction,                                 \
        DecreasingOrderGlobalServerSortValuesAction_##unique)                                                \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::DecreasingOrderGlobalServer_##unique::RecordRouteFragmentAction,                        \
        DecreasingOrderGlobalServerRecordRouteFragmentAction_##unique)                                       \
                                                                                                             \
    HPX_REGISTER_ACTION(                                                                                     \
        lue::detail::DecreasingOrderGlobalServer_##unique::SkipRecordingRouteFragmentsAction,                \
        DecreasingOrderGlobalServerSkipRecordingRouteFragmentsAction_##unique)


#define LUE_INSTANTIATE_DECREASING_ORDER_GLOBAL(Policies, rank)                                              \
                                                                                                             \
    template LUE_ROUTING_OPERATION_EXPORT SerialRoute<policy::OutputElementT<Policies, 0>, rank>             \
    decreasing_order<ArgumentType<void(Policies)>, rank>(                                                    \
        ArgumentType<void(Policies)> const&,                                                                 \
        PartitionedArray<policy::InputElementT<Policies, 1>, rank> const&,                                   \
        Count const);
