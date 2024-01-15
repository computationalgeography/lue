#pragma once
// #include "lue/framework/algorithm/decreasing_order.hpp"
// #include "lue/framework/algorithm/detail/localities.hpp"
#include "lue/framework/algorithm/image_land_operation_export.hpp"
#include "lue/framework/algorithm/integrate_and_allocate.hpp"
// #include "lue/framework/algorithm/routing/walk/accumulator.hpp"
// #include "lue/framework/algorithm/routing/walk/cell_counter.hpp"
// #include "lue/framework/algorithm/walk.hpp"
// #include "lue/framework/core/component.hpp"
#include "lue/macro.hpp"
// #include <hpx/include/components.hpp>
// #include <hpx/synchronization/shared_mutex.hpp>
// #include <set>


namespace lue::detail::integrate_and_allocate {

    // template<typename RouteID, typename Element>
    // class Data: public CellCounter, public Accumulator<RouteID, Element>
    // {

    //     public:

    //         Data():

    //             Data{0}

    //         {
    //         }

    //         Data(Count const max_nr_cells):

    //             CellCounter{max_nr_cells},
    //             Accumulator<RouteID, Element>{}

    //         {
    //         }

    //     private:

    //         friend class hpx::serialization::access;

    //         template<typename Archive>
    //         void serialize(Archive& archive, unsigned int const version)
    //         {
    //             CellCounter::serialize(archive, version);
    //             Accumulator<RouteID, Element>::serialize(archive, version);
    //         }
    // };


    // template<typename Policies, Rank rank>
    // class Walk;


    // namespace server {

    //     template<typename Policies, Rank rank>
    //     class Walk: public hpx::components::component_base<Walk<Policies, rank>>
    //     {

    //         private:

    //             using ComponentServerBase = hpx::components::component_base<Walk<Policies, rank>>;
    //             using ComponentClient = integrate_and_allocate::Walk<Policies, rank>;

    //             using OutputElement = policy::OutputElementT<Policies, 0>;
    //             using RouteID = policy::InputElementT<Policies, 0>;
    //             using IntegrandElement = policy::InputElementT<Policies, 1>;

    //         public:

    //             using OutputPartition = ArrayPartition<OutputElement, rank>;
    //             using RoutePartition = lue::SerialRoutePartition<RouteID, rank>;
    //             using IntegrandPartition = ArrayPartition<IntegrandElement, rank>;

    //             Walk(
    //                 Policies const& policies,
    //                 RoutePartition const& route_partition,
    //                 IntegrandPartition const& integrand_partition):

    //                 ComponentServerBase{},
    //                 _policies{policies},
    //                 _output_partition_promise{},
    //                 _output_partition{},
    //                 _route_partition{route_partition},
    //                 _integrand_partition{integrand_partition},
    //                 _walk_mutex{},
    //                 _downstream_components{},
    //                 _fragment_idxs{}

    //             {
    //                 using OutputPartitionServer = typename OutputPartition::Server;

    //                 auto const& ondp = std::get<0>(_policies.outputs_policies()).output_no_data_policy();
    //                 OutputElement no_data_value;

    //                 ondp.mark_no_data(no_data_value);

    //                 lue_hpx_assert(_route_partition.is_ready());
    //                 lue_hpx_assert(_integrand_partition.is_ready());
    //                 // _route_partition.wait();
    //                 auto route_partition_ptr{ready_component_ptr(_route_partition)};

    //                 _output_partition = OutputPartition{hpx::new_<OutputPartitionServer>(
    //                     hpx::find_here(),
    //                     route_partition_ptr->offset(),
    //                     route_partition_ptr->shape(),
    //                     no_data_value)};

    //                 std::set<RouteID> const route_ids{route_partition_ptr->route_ids()};

    //                 for (RouteID const route_id : route_ids)
    //                 {
    //                     // Initialize with idx. First fragment to visit has index
    //                     // zero. This index increases during the walk, until the last
    //                     // fragment has been handled, after which the index for this
    //                     // route_id can be removed. When all indices have been removed,
    //                     // the walk has finished.
    //                     _fragment_idxs[route_id] = 0;
    //                 }
    //             }


    //             OutputPartition result_partition()
    //             {
    //                 // Connect the promise with the future
    //                 // This function must be called exactly once
    //                 OutputPartition partition{_output_partition_promise.get_future()};

    //                 // If the route partition contains no fragments, the output partition
    //                 // can be marked as finished immediately
    //                 if (_fragment_idxs.empty())
    //                 {
    //                     finish_partition();
    //                 }

    //                 return partition;
    //             }


    //             void set_downstream_components(std::map<hpx::id_type, ComponentClient>&& components)
    //             {
    //                 _downstream_components = components;
    //             }


    //             void walk(RouteID const route_id, Data<RouteID, IntegrandElement>&& data)
    //             {
    //                 // This function may be called by multiple threads, for different
    //                 // routes, or for the same route but for different fragments. In
    //                 // the latter case, these calls will be ordered, with the first
    //                 // call handling an upstream fragment and subsequent calls handling
    //                 // downstream fragments.

    //                 std::shared_lock read_lock{_walk_mutex, std::defer_lock};
    //                 std::unique_lock write_lock{_walk_mutex, std::defer_lock};

    //                 // First, do stuff, *using* state veriables. Don't change stuff that
    //                 // is shared between threads. Changing different cells in a raster
    //                 // is fine, just don't change the same values.
    //                 read_lock.lock();

    //                 auto route_partition_server_ptr{ready_component_ptr(_route_partition)};
    //                 auto& route_partition_server{*route_partition_server_ptr};

    //                 auto integrand_partition_server_ptr{ready_component_ptr(_integrand_partition)};
    //                 auto& integrand_partition_server{*integrand_partition_server_ptr};
    //                 auto integrand_partition_data{integrand_partition_server.data()};
    //                 auto const& integrand_indp =
    //                     std::get<1>(_policies.inputs_policies()).input_no_data_policy();

    //                 auto output_partition_server_ptr{ready_component_ptr(_output_partition)};
    //                 auto& output_partition_server{*output_partition_server_ptr};
    //                 auto output_partition_data{output_partition_server.data()};

    //                 lue_hpx_assert(
    //                     route_partition_server.route_ids().find(route_id) !=
    //                     route_partition_server.route_ids().end());

    //                 lue_hpx_assert(!_fragment_idxs.empty());
    //                 lue_hpx_assert(_fragment_idxs.find(route_id) != _fragment_idxs.end());

    //                 Index& fragment_idx{_fragment_idxs.at(route_id)};

    //                 auto const& route_fragments{route_partition_server.route_fragments(route_id)};

    //                 lue_hpx_assert(fragment_idx < static_cast<Size>(std::size(route_fragments)));

    //                 auto const& route_fragment{route_fragments[fragment_idx]};

    //                 auto const& cell_idxs{route_fragment.cell_idxs()};

    //                 // For as long as needed, iterate over each cell in the current
    //                 // fragment. For each cell:
    //                 // - Calculate an output value
    //                 // - Assign output value to corresponding cell in the output partition

    //                 OutputElement& accumulated_value{data.value(route_id)};

    //                 for (std::size_t idx = 0; idx < cell_idxs.size() && data.keep_walking(); ++idx, ++data)
    //                 {
    //                     if (!integrand_indp.is_no_data(integrand_partition_data, cell_idxs[idx]))
    //                     {
    //                         accumulated_value += integrand_partition_data[cell_idxs[idx]];
    //                         output_partition_data[cell_idxs[idx]] = accumulated_value;
    //                     }
    //                 }

    //                 read_lock.unlock();
    //                 write_lock.lock();
    //                 ++fragment_idx;

    //                 if (!route_fragment.is_last())
    //                 {
    //                     if (data.keep_walking())
    //                     {
    //                         // Continue the walk in a downstream partition
    //                         lue_hpx_assert(route_fragment.next_fragment_location().valid());
    //                         lue_hpx_assert(route_fragment.next_fragment_location().is_ready());

    //                         hpx::id_type const downstream_route_partition_id{
    //                             route_fragment.next_fragment_location().get()};

    //                         lue_hpx_assert(
    //                             _downstream_components.find(downstream_route_partition_id) !=
    //                             _downstream_components.end());

    //                         _downstream_components.at(downstream_route_partition_id)
    //                             .walk(route_id, std::move(data));
    //                     }
    //                     else
    //                     {
    //                         lue_hpx_assert(route_fragment.next_fragment_location().valid());
    //                         lue_hpx_assert(route_fragment.next_fragment_location().is_ready());

    //                         hpx::id_type const downstream_route_partition_id{
    //                             route_fragment.next_fragment_location().get()};

    //                         lue_hpx_assert(
    //                             _downstream_components.find(downstream_route_partition_id) !=
    //                             _downstream_components.end());

    //                         _downstream_components.at(downstream_route_partition_id)
    //                             .skip_walking_route_fragments(route_id);
    //                     }
    //                 }

    //                 lue_hpx_assert(
    //                     (route_fragment.is_last() &&
    //                      fragment_idx == static_cast<Size>(std::size(route_fragments))) ||
    //                     !route_fragment.is_last());

    //                 if (fragment_idx == static_cast<Size>(std::size(route_fragments)))
    //                 {
    //                     // This was the last fragment of the route that is located in
    //                     // this partition. If it was the last fragment at all, we can / must
    //                     // finish the partition.
    //                     _fragment_idxs.erase(route_id);

    //                     if (_fragment_idxs.empty())
    //                     {
    //                         finish_partition();
    //                     }
    //                 }
    //             }


    //             void skip_walking_route_fragments(RouteID const route_id)
    //             {
    //                 // Skip walking route fragments for the route with the ID passed in.
    //                 // If this results in an empty _fragment_idxs collection, then
    //                 // apparently we have handled all fragments of all routes, and the
    //                 // output partition can be finished.

    //                 // This function is called from another component. Since we will
    //                 // be changing the state of the current component, we need to obtain
    //                 // a write lock.

    //                 std::shared_lock read_lock{_walk_mutex, std::defer_lock};
    //                 std::unique_lock write_lock{_walk_mutex, std::defer_lock};

    //                 read_lock.lock();

    //                 lue_hpx_assert(!_fragment_idxs.empty());
    //                 lue_hpx_assert(_fragment_idxs.find(route_id) != _fragment_idxs.end());

    //                 Index& fragment_idx{_fragment_idxs.at(route_id)};

    //                 auto const& route_fragments{
    //                     ready_component_ptr(_route_partition)->route_fragments(route_id)};
    //                 lue_hpx_assert(fragment_idx < static_cast<Size>(std::size(route_fragments)));

    //                 auto const& route_fragment{route_fragments[fragment_idx]};

    //                 if (!route_fragment.is_last())
    //                 {
    //                     lue_hpx_assert(route_fragment.next_fragment_location().valid());
    //                     lue_hpx_assert(route_fragment.next_fragment_location().is_ready());

    //                     hpx::id_type const downstream_route_partition_id{
    //                         route_fragment.next_fragment_location().get()};

    //                     lue_hpx_assert(
    //                         _downstream_components.find(downstream_route_partition_id) !=
    //                         _downstream_components.end());

    //                     _downstream_components.at(downstream_route_partition_id)
    //                         .skip_walking_route_fragments(route_id);
    //                 }

    //                 read_lock.unlock();
    //                 write_lock.lock();

    //                 ++fragment_idx;

    //                 if (fragment_idx == static_cast<Size>(std::size(route_fragments)))
    //                 {
    //                     _fragment_idxs.erase(route_id);

    //                     if (_fragment_idxs.empty())
    //                     {
    //                         finish_partition();
    //                     }
    //                 }
    //             }


    //             HPX_DEFINE_COMPONENT_ACTION(Walk, result_partition, ResultPartitionAction);

    //             HPX_DEFINE_COMPONENT_ACTION(Walk, set_downstream_components,
    //             SetDownstreamComponentsAction);

    //             HPX_DEFINE_COMPONENT_ACTION(Walk, walk, WalkAction);

    //             HPX_DEFINE_COMPONENT_ACTION(
    //                 Walk, skip_walking_route_fragments, SkipWalkingRouteFragmentsAction);

    //         private:

    //             void finish_partition()
    //             {
    //                 // This function must be called exactly once
    //                 lue_hpx_assert(_output_partition.valid());
    //                 lue_hpx_assert(_output_partition.is_ready());
    //                 lue_hpx_assert(_fragment_idxs.empty());

    //                 _output_partition_promise.set_value(_output_partition.get_id());

    //                 _downstream_components.clear();
    //             }

    //             Policies _policies;

    //             mutable hpx::promise<hpx::id_type> _output_partition_promise;

    //             OutputPartition _output_partition;

    //             RoutePartition _route_partition;

    //             IntegrandPartition _integrand_partition;

    //             hpx::shared_mutex _walk_mutex;

    //             //! Component client by route partition ID
    //             std::map<hpx::id_type, ComponentClient> _downstream_components;

    //             //! Route fragment index by route ID
    //             std::map<RouteID, Index> _fragment_idxs;
    //     };

    // }  // namespace server


    // template<typename Policies, Rank rank>
    // class Walk: public hpx::components::client_base<Walk<Policies, rank>, server::Walk<Policies, rank>>
    // {

    //     private:

    //         using ComponentServer = server::Walk<Policies, rank>;
    //         using ComponentClientBase = hpx::components::client_base<Walk<Policies, rank>,
    //         ComponentServer>; using OutputPartition = typename ComponentServer::OutputPartition;

    //     public:

    //         using RouteID = policy::InputElementT<Policies, 0>;
    //         using IntegrandElement = policy::InputElementT<Policies, 1>;

    //         Walk():

    //             ComponentClientBase{}

    //         {
    //         }


    //         Walk(hpx::id_type const& component_id):

    //             ComponentClientBase{component_id}

    //         {
    //         }


    //         Walk(hpx::future<hpx::id_type>&& component_id):

    //             ComponentClientBase{std::move(component_id)}

    //         {
    //         }


    //         OutputPartition result_partition() const
    //         {
    //             lue_hpx_assert(this->is_ready());
    //             lue_hpx_assert(this->get_id());

    //             typename ComponentServer::ResultPartitionAction action;

    //             return hpx::async(action, this->get_id());
    //         }


    //         hpx::future<void> set_downstream_components(std::map<hpx::id_type, Walk>&& components)
    //         {
    //             lue_hpx_assert(this->is_ready());
    //             lue_hpx_assert(this->get_id());

    //             typename ComponentServer::SetDownstreamComponentsAction action;

    //             return hpx::async(action, this->get_id(), std::move(components));
    //         }


    //         hpx::future<void> walk(RouteID const route_id, Data<RouteID, IntegrandElement> data) const
    //         {
    //             lue_hpx_assert(this->is_ready());
    //             lue_hpx_assert(this->get_id());

    //             typename ComponentServer::WalkAction action;

    //             return hpx::async(action, this->get_id(), route_id, std::move(data));
    //         }


    //         hpx::future<void> skip_walking_route_fragments(RouteID const route_id) const
    //         {
    //             lue_hpx_assert(this->is_ready());
    //             lue_hpx_assert(this->get_id());

    //             typename ComponentServer::SkipWalkingRouteFragmentsAction action;

    //             return hpx::async(action, this->get_id(), route_id);
    //         }
    // };

}  // namespace lue::detail::integrate_and_allocate


namespace lue {

    template<typename Policies, Rank rank>
    auto integrate_and_allocate(
        Policies const& policies,
        SerialRoute<policy::InputElementT<Policies, 0>, rank> const& route,
        PartitionedArray<policy::InputElementT<Policies, 1>, rank> const& zone,
        std::vector<
            std::reference_wrapper<PartitionedArray<policy::InputElementT<Policies, 2>, rank> const>> const&
            crop_fractions)
        -> std::tuple<
            std::vector<PartitionedArray<policy::OutputElementT<Policies, 0>, rank>>,
            std::map<policy::InputElementT<Policies, 1>, policy::OutputElementT<Policies, 1>>>
    {
        using RouteID = policy::InputElementT<Policies, 0>;
        using ZoneElement = policy::InputElementT<Policies, 1>;
        using CropFractionElement = policy::InputElementT<Policies, 2>;

        static_assert(std::is_integral_v<RouteID>);
        static_assert(std::is_integral_v<ZoneElement>);
        static_assert(std::is_floating_point_v<CropFractionElement>);

        static_assert(std::is_same_v<CropFractionElement, policy::OutputElementT<Policies, 0>>);

        using ProductionElement = policy::OutputElementT<Policies, 1>;

        static_assert(std::is_floating_point_v<ProductionElement>);

        using Route = SerialRoute<RouteID, rank>;
        using RoutePartition = PartitionT2<Route>;
        using RoutePartitions = PartitionsT2<Route>;

        using ZoneArray = PartitionedArray<ZoneElement, rank>;
        using ZonePartition = PartitionT<ZoneArray>;
        using ZonePartitions = PartitionsT<ZoneArray>;

        using CropFractionArray = PartitionedArray<CropFractionElement, rank>;
        using CropFractionPartition = PartitionT<CropFractionArray>;
        using CropFractionPartitions = PartitionsT<CropFractionArray>;
        using CropFractionArrays = std::vector<PartitionedArray<CropFractionElement, rank>>;

        using ZonalProduction = std::map<ZoneElement, ProductionElement>;

        CropFractionArrays crop_fraction_arrays{};
        ZonalProduction zonal_production{};


        // using IntegrandElement = policy::InputElementT<Policies, 1>;
        // using IntegrandArray = PartitionedArray<IntegrandElement, rank>;
        // using IntegrandPartition = PartitionT<IntegrandArray>;
        // using IntegrandPartitions = PartitionsT<IntegrandArray>;

        // using OutputElement = policy::OutputElementT<Policies, 0>;
        // using OutputArray = PartitionedArray<OutputElement, rank>;
        // using OutputPartition = PartitionT<OutputArray>;
        // using OutputPartitions = PartitionsT<OutputArray>;

        // // Iterate over all route partitions and:
        // // - Create a new Walk component, passing in the route partition

        // using WalkComponentClient = detail::integrate_and_allocate::Walk<Policies, rank>;
        // using WalkComponentServer = detail::integrate_and_allocate::server::Walk<Policies, rank>;

        // RoutePartitions const& route_partitions{route.partitions()};
        // IntegrandPartitions const& integrand_partitions{integrand.partitions()};

        // Array<hpx::future<hpx::id_type>, rank> locality_fs{detail::localities(route_partitions)};

        // Count const nr_partitions{nr_elements(route_partitions.shape())};
        // Array<WalkComponentClient, rank> walk_components{route_partitions.shape()};

        // for (Index idx = 0; idx < nr_partitions; ++idx)
        // {
        //     walk_components[idx] = WalkComponentClient{hpx::dataflow(
        //         hpx::launch::async,
        //         [policies](
        //             RoutePartition const& route_partition, IntegrandPartition const& integrand_partition)
        //         {
        //             return hpx::new_<WalkComponentServer>(
        //                 hpx::colocated(route_partition.get_id()),
        //                 policies,
        //                 route_partition,
        //                 integrand_partition);
        //         },
        //         route_partitions[idx],
        //         integrand_partitions[idx])};
        // }

        // OutputPartitions partitions{walk_components.shape()};

        // for (Index idx = 0; idx < nr_partitions; ++idx)
        // {
        //     partitions[idx] = walk_components[idx].then(
        //         hpx::unwrapping([](WalkComponentClient const& component)
        //                         { return OutputPartition{component.result_partition()}; }));
        // }

        // detail::integrate_and_allocate::Data<RouteID, IntegrandElement> data{max_nr_cells};

        // // Do whatever it takes to end up with ready output partitions
        // walk(route, walk_components, std::move(data));

        // // Keep the components alive for as long as the partitions are not ready
        // hpx::when_all(partitions.begin(), partitions.end())
        //     .then([components = std::move(walk_components)](auto&&) { HPX_UNUSED(components); });

        // // TODO Get rid of this wait and localities stuff
        // Array<hpx::id_type, rank> localities{locality_fs.shape()};
        // {
        //     hpx::wait_all(locality_fs.begin(), locality_fs.end());
        //     std::transform(
        //         locality_fs.begin(),
        //         locality_fs.end(),
        //         localities.begin(),
        //         [](hpx::future<hpx::id_type>& locality_f) { return locality_f.get(); });
        // }

        // return OutputArray{route.shape(), std::move(localities), std::move(partitions)};


        return std::make_tuple(std::move(crop_fraction_arrays), std::move(zonal_production));
    }

}  // namespace lue


#define LUE_INSTANTIATE_INTEGRATE_AND_ALLOCATE(Policies, rank)                                               \
                                                                                                             \
    template LUE_IMAGE_LAND_OPERATION_EXPORT std::tuple<                                                     \
        std::vector<PartitionedArray<policy::OutputElementT<Policies, 0>, rank>>,                            \
        std::map<policy::InputElementT<Policies, 1>, policy::OutputElementT<Policies, 1>>>                   \
    integrate_and_allocate<ArgumentType<void(Policies)>, rank>(                                              \
        ArgumentType<void(Policies)> const&,                                                                 \
        SerialRoute<policy::InputElementT<Policies, 0>, rank> const&,                                        \
        PartitionedArray<policy::InputElementT<Policies, 1>, rank> const&,                                   \
        std::vector<std::reference_wrapper<                                                                  \
            PartitionedArray<policy::InputElementT<Policies, 2>, rank> const>> const&);
