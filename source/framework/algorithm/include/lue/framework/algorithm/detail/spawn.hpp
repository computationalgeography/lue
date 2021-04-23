#pragma once
#include "lue/framework/algorithm/detail/halo_partition.hpp"
#include "lue/framework/core/array_partition_data.hpp"
#include "lue/framework/core/annotate.hpp"


namespace lue::detail {

    template<
        typename Component,
        Rank rank>
    hpx::future<ArrayPartitionData<Component, rank>> when_all_get(
        ArrayPartitionData<Component, rank>&& array_of_components)
    {
        // Given an array of components, wait for all components to become ready
        // and return an array with the ready components.
        // hpx::when_all returns a linear vector. We want an nD array.

        return hpx::when_all(array_of_components.begin(), array_of_components.end()).then(
            [shape=array_of_components.shape()](
                hpx::future<std::vector<Component>>&& f)
            {
                std::vector<Component> vector_of_ready_components{f.get()};
                ArrayPartitionData<Component, rank> array_of_ready_components{{shape}};

                std::move(
                    vector_of_ready_components.begin(), vector_of_ready_components.end(),
                    array_of_ready_components.begin());

                return array_of_ready_components;
            });
    }


    template<
        typename... OutputComponent,
        typename Policies,
        typename Action,
        typename... InputComponents>
    hpx::util::tuple<OutputComponent...> spawn_component(
        hpx::id_type const locality_id,
        Action const& action,
        Policies const& policies,
        Offset<Index, 2> const& component_offset,
        InputComponents&&... input_components)
    {
        // Attach a continuation to the future which becomes ready once all
        // input components have become ready.

        static_assert(sizeof...(OutputComponent) > 0);

        auto result = hpx::dataflow(
            hpx::launch::async,
            hpx::util::unwrapping(

                    [locality_id, action, policies, component_offset](
                        InputComponents&&... input_components)
                    {
                        AnnotateFunction annotation{"spawn_component"};
                        return action(
                            locality_id, policies, component_offset, std::move(input_components)...);
                    }

                ),
            when_all_get(std::move(input_components))...);

        if constexpr (sizeof...(OutputComponent) == 1)
        {
            // result is future<component>
            // Turn this into tuple<component...>
            return hpx::util::make_tuple(std::move(result));
        }
        else
        {
            // result is future<tuple<component...>>
            // Turn this into tuple<component...>
            return hpx::split_future(std::move(result));
        }
    }


    template<
        typename... OutputComponent,
        typename Policies,
        typename Action,
        typename... InputComponents>
    hpx::util::tuple<OutputComponent...> spawn_component(
        hpx::id_type const locality_id,
        Action const& action,
        Policies const& policies,
        Offset<Index, 2> const& component_offset,
        std::tuple<InputComponents...>&& input_components)
    {
        auto spawn = [&locality_id, &action, &policies, &component_offset](
            InputComponents&&... input_components)
        {
            return spawn_component<OutputComponent...>(
                locality_id, action, policies, component_offset, std::move(input_components)...);
        };

        return std::apply(spawn, std::move(input_components));
    }


    template<
        typename ComponentCollection,
        typename... Index>
    ComponentT<ComponentCollection>& component_ref(
        ComponentCollection& components,
        Index const... component_idxs)
    {
        // Given a collection of components and component indices, return
        // a reference to the component pointed to by those component indices

        return components(component_idxs...);
    }


    template<
        typename... ComponentCollections,
        std::size_t... collection_idxs,
        typename... Index>
    hpx::util::tuple<ComponentT<ComponentCollections>&...> component_refs(
        std::tuple<ComponentCollections...>& components,
        std::index_sequence<collection_idxs...>,
        Index const... component_idxs)
    {
        // Given a number of collections of components and component indices,
        // return a tuple with for each of the component collections a
        // reference to the component pointed to by those component indices

        return hpx::util::tuple<ComponentT<ComponentCollections>&...>{
            component_ref(std::get<collection_idxs>(components), component_idxs...)...};
    }


    template<
        typename... ComponentCollections,
        typename... Index>
    hpx::util::tuple<ComponentT<ComponentCollections>&...> component_refs(
        std::tuple<ComponentCollections...>& components,
        Index const... component_idxs)
    {
        // Given a number of collections of components and component indices,
        // return a tuple with for each of the component collections a
        // reference to the component pointed to by those component indices

        return component_refs(
            components, std::index_sequence_for<ComponentCollections...>(), component_idxs...);
    }


    template<
        typename... OutputComponentCollection,
        typename Policies,
        typename Action,
        typename Localities,
        typename... InputComponentCollection>
    auto spawn_components(
        Policies const& policies,
        Action const& action,
        Localities const& localities,
        std::tuple<std::array<InputComponentCollection, 3>...> const& halo_components,
        InputComponentCollection const&... input_component_collection)
    {
        static_assert(sizeof...(OutputComponentCollection) > 0);
        static_assert(sizeof...(InputComponentCollection) > 0);

        auto const first_input_component_collection{
            std::get<0>(std::forward_as_tuple(input_component_collection...))};

        auto const& shape_in_components{first_input_component_collection.shape()};
        auto output_components{std::make_tuple<OutputComponentCollection...>(
            OutputComponentCollection{shape_in_components}...)};

        auto const [nr_components0, nr_components1] = shape_in_components;
        lue_hpx_assert(nr_components0 > 0);
        lue_hpx_assert(nr_components1 > 0);

        Rank const rank{2};
        using Offset = lue::Offset<Index, rank>;

        // Component indices
        Index idx0, idx1;

        // North-west component
        idx0 = 0;
        idx1 = 0;

        component_refs(output_components, idx0, idx1) =
            spawn_component<ComponentT<OutputComponentCollection>...>(localities(idx0, idx1),
                action, policies, Offset{idx0, idx1},
                north_west_corner_input_partitions(
                    std::index_sequence_for<InputComponentCollection...>(),
                    halo_components, input_component_collection...));

        if(nr_components1 > 1)
        {
            // North-east component
            idx0 = 0;
            idx1 = nr_components1 - 1;
            component_refs(output_components, idx0, idx1) =
                spawn_component<ComponentT<OutputComponentCollection>...>(localities(idx0, idx1),
                    action, policies, Offset{idx0, idx1},
                    north_east_corner_input_partitions(
                        std::index_sequence_for<InputComponentCollection...>(),
                        halo_components, input_component_collection...));
        }

        if(nr_components0 > 1)
        {
            // South-west component
            idx0 = nr_components0 - 1;
            idx1 = 0;
            component_refs(output_components, idx0, idx1) =
                spawn_component<ComponentT<OutputComponentCollection>...>(localities(idx0, idx1),
                    action, policies, Offset{idx0, idx1},
                    south_west_corner_input_partitions(
                        std::index_sequence_for<InputComponentCollection...>(),
                        halo_components, input_component_collection...));
        }

        if(nr_components0 > 1 && nr_components1 > 1)
        {
            // South-east component
            idx0 = nr_components0 - 1;
            idx1 = nr_components1 - 1;
            component_refs(output_components, idx0, idx1) =
                spawn_component<ComponentT<OutputComponentCollection>...>(localities(idx0, idx1),
                    action, policies, Offset{idx0, idx1},
                    south_east_corner_input_partitions(
                        std::index_sequence_for<InputComponentCollection...>(),
                        halo_components, input_component_collection...));
        }

        for(idx0 = 0, idx1 = 1; idx1 < nr_components1 - 1; ++idx1)
        {
            // North-side components
            component_refs(output_components, idx0, idx1) =
                spawn_component<ComponentT<OutputComponentCollection>...>(localities(idx0, idx1),
                    action, policies, Offset{idx0, idx1},
                    north_side_input_partitions(idx1,
                        std::index_sequence_for<InputComponentCollection...>(),
                        halo_components, input_component_collection...));
        }

        if(nr_components0 > 1)
        {
            // South-side components
            for(idx0 = nr_components0 - 1, idx1 = 1; idx1 < nr_components1 - 1; ++idx1)
            {
                component_refs(output_components, idx0, idx1) =
                    spawn_component<ComponentT<OutputComponentCollection>...>(localities(idx0, idx1),
                        action, policies, Offset{idx0, idx1},
                        south_side_input_partitions(idx1,
                            std::index_sequence_for<InputComponentCollection...>(),
                            halo_components, input_component_collection...));
            }
        }

        // West-side components
        for(idx0 = 1, idx1 = 0; idx0 < nr_components0 - 1; ++idx0)
        {
            component_refs(output_components, idx0, idx1) =
                spawn_component<ComponentT<OutputComponentCollection>...>(localities(idx0, idx1),
                    action, policies, Offset{idx0, idx1},
                    west_side_input_partitions(idx0,
                        std::index_sequence_for<InputComponentCollection...>(),
                        halo_components, input_component_collection...));
        }

        if(nr_components1 > 1)
        {
            // East-side components
            for(idx0 = 1, idx1 = nr_components1 - 1; idx0 < nr_components0 - 1; ++idx0)
            {
                component_refs(output_components, idx0, idx1) =
                    spawn_component<ComponentT<OutputComponentCollection>...>(localities(idx0, idx1),
                        action, policies, Offset{idx0, idx1},
                        east_side_input_partitions(idx0,
                            std::index_sequence_for<InputComponentCollection...>(),
                            halo_components, input_component_collection...));
            }
        }

        // idx0, idx1 is the center component
        for(idx0 = 1; idx0 < nr_components0 - 1; ++idx0) {
            for(idx1 = 1; idx1 < nr_components1 - 1; ++idx1)
            {
                // Center components
                component_refs(output_components, idx0, idx1) =
                    spawn_component<ComponentT<OutputComponentCollection>...>(localities(idx0, idx1),
                        action, policies, Offset{idx0, idx1},
                        inner_input_partitions(idx0, idx1, input_component_collection...));
            }
        }

        if constexpr (sizeof...(OutputComponentCollection) == 1)
        {
            return std::get<0>(output_components);
        }
        else
        {
            return output_components;
        }
    }


    template<
        typename... OutputComponentCollection,
        typename Policies,
        typename Action,
        typename Localities,
        typename InputComponentCollection>
    auto spawn_components(
        Policies const& policies,
        Action const& action,
        Localities const& localities,
        std::array<InputComponentCollection, 3> const& halo_components,
        InputComponentCollection const& input_component_collection)
    {
        return spawn_components<OutputComponentCollection...>(
            policies, action, localities, std::make_tuple(halo_components), input_component_collection);
    }

}  // namespace lue::detail
