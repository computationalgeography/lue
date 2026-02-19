#pragma once
#include "lue/framework/algorithm/flow_direction.hpp"
#include "lue/framework/core/component.hpp"
#include "lue/framework/partitioned_array_decl.hpp"


namespace lue::detail {

    template<typename... T>
    using ShapeT = ShapeT<std::tuple_element_t<0, std::tuple<T...>>>;


    template<typename Element>
    ArrayPartition<Element, 2> corner_halo_partition(
        ShapeT<ArrayPartition<Element, 2>> const& min_shape, Element const fill_value)
    {
        // Asynchronously create a new partition:
        // - With a shape based on the partition paѕsed in
        // - Filled with the fill_value passed in

        using Partition = ArrayPartition<Element, 2>;
        using Offset = OffsetT<Partition>;

        return Partition{hpx::find_here(), Offset{}, min_shape, fill_value};
    }


    template<typename Element>
    ArrayPartition<Element, 2> longitudinal_side_halo_partition(
        ArrayPartition<Element, 2> const& partition,
        ShapeT<ArrayPartition<Element, 2>> const& min_shape,
        Element const fill_value)
    {
        // Asynchronously create a new partition:
        // - With a shape based on the partition paѕsed in
        // - Filled with the fill_value passed in

        using Partition = ArrayPartition<Element, 2>;
        using Shape = ShapeT<Partition>;
        using Offset = OffsetT<Partition>;

        lue_hpx_assert(partition.is_ready());

        return hpx::dataflow(
            hpx::launch::async,
            hpx::unwrapping(

                [extent0 = std::get<0>(min_shape), fill_value](Shape const& shape)
                { return Partition{hpx::find_here(), Offset{}, Shape{{extent0, shape[1]}}, fill_value}; }

                ),
            partition.shape());
    }


    template<typename Element>
    ArrayPartition<Element, 2> latitudinal_side_halo_partition(
        ArrayPartition<Element, 2> const& partition,
        ShapeT<ArrayPartition<Element, 2>> const& min_shape,
        Element const fill_value)
    {
        // Asynchronously create a new partition:
        // - With a shape based on the partition paѕsed in
        // - Filled with the fill_value passed in

        using Partition = ArrayPartition<Element, 2>;
        using Shape = ShapeT<Partition>;
        using Offset = OffsetT<Partition>;

        lue_hpx_assert(partition.is_ready());

        return hpx::dataflow(
            hpx::launch::async,
            hpx::unwrapping(

                [extent1 = std::get<1>(min_shape), fill_value](Shape const& shape)
                { return Partition{hpx::find_here(), Offset{}, Shape{{shape[0], extent1}}, fill_value}; }

                ),
            partition.shape());
    }


    template<typename Element>
    struct CornerHaloPartitionAction:
        hpx::actions::make_action<
            decltype(&corner_halo_partition<Element>),
            &corner_halo_partition<Element>,
            CornerHaloPartitionAction<Element>>::type
    {
    };


    template<typename Element>
    struct LongitudinalSideHaloPartitionAction:
        hpx::actions::make_action<
            decltype(&longitudinal_side_halo_partition<Element>),
            &longitudinal_side_halo_partition<Element>,
            LongitudinalSideHaloPartitionAction<Element>>::type
    {
    };


    template<typename Element>
    struct LatitudinalSideHaloPartitionAction:
        hpx::actions::make_action<
            decltype(&latitudinal_side_halo_partition<Element>),
            &latitudinal_side_halo_partition<Element>,
            LatitudinalSideHaloPartitionAction<Element>>::type
    {
    };


    template<typename InputPolicies, typename InputPartitions, typename Localities>
    auto halo_corner_partitions(
        InputPolicies const& input_policies,
        Localities const& localities,
        ShapeT<InputPartitions> const& min_shape,
        [[maybe_unused]] InputPartitions const& input_partitions) -> InputPartitions
    {
        auto const [nr_partitions0, nr_partitions1] = localities.shape();

        InputPartitions halo_partitions{{2, 2}};

        // Corner halo partitions
        //     +----+----+
        //     | NW | NE |
        //     +----+----+
        //     | SW | SE |
        //     +----+----+

        using Element = ElementT<InputPartitions>;
        using Action = CornerHaloPartitionAction<Element>;

        auto const& hp{input_policies.halo_policy()};

        Action action{};

        // North-west corner halo partition
        halo_partitions(0, 0) = hpx::async(action, localities(0, 0), min_shape, hp.north_west_corner());

        // North-east corner halo partition
        halo_partitions(0, 1) =
            hpx::async(action, localities(0, nr_partitions1 - 1), min_shape, hp.north_east_corner());

        // South-west corner halo partition
        halo_partitions(1, 0) =
            hpx::async(action, localities(nr_partitions0 - 1, 0), min_shape, hp.south_west_corner());

        // South-east corner halo partition
        halo_partitions(1, 1) = hpx::async(
            action, localities(nr_partitions0 - 1, nr_partitions1 - 1), min_shape, hp.south_east_corner());

        lue_hpx_assert(all_are_valid(halo_partitions));

        return halo_partitions;
    }


    template<typename InputPolicies, typename InputPartitions, typename Localities>
    auto halo_longitudinal_side_partitions(
        InputPolicies const& input_policies,
        Localities const& localities,
        ShapeT<InputPartitions> const& min_shape,
        InputPartitions const& input_partitions) -> InputPartitions
    {
        auto const [nr_partitions0, nr_partitions1] = localities.shape();
        InputPartitions halo_partitions{{2, nr_partitions1}};

        // Longitudinal side halo partitions
        //     +---+---+---+
        //     | N | N | N |
        //     +---+---+---+
        //     | S | S | S |
        //     +---+---+---+

        using InputPartition = PartitionT<InputPartitions>;
        using Element = ElementT<InputPartitions>;
        using Action = LongitudinalSideHaloPartitionAction<Element>;

        auto const& hp{input_policies.halo_policy()};

        Action action{};

        for (auto const& [rh, rp, fill_value] :
             {std::tuple<Index, Index, Element>{0, 0, hp.north_side()},
              std::tuple<Index, Index, Element>{1, nr_partitions0 - 1, hp.south_side()}})
        {
            for (Index cp = 0; cp < nr_partitions1; ++cp)
            {
                halo_partitions(rh, cp) = InputPartition{input_partitions(rp, cp)}.then(

                    [action, locality = localities(rp, cp), min_shape, fill_value = fill_value](
                        InputPartition&& input_partition)
                    { return action(locality, std::move(input_partition), min_shape, fill_value); });
            }
        }

        lue_hpx_assert(all_are_valid(halo_partitions));

        return halo_partitions;
    }


    template<typename InputPolicies, typename InputPartitions, typename Localities>
    auto halo_latitudinal_side_partitions(
        InputPolicies const& input_policies,
        Localities const& localities,
        ShapeT<InputPartitions> const& min_shape,
        InputPartitions const& input_partitions) -> InputPartitions
    {
        auto const [nr_partitions0, nr_partitions1] = localities.shape();
        InputPartitions halo_partitions{{nr_partitions0, 2}};

        // Latitudinal sides halo partitions
        //     +---+---+
        //     | W | E |
        //     +---+---+
        //     | W | E |
        //     +---+---+
        //     | W | E |
        //     +---+---+

        using InputPartition = PartitionT<InputPartitions>;
        using Element = ElementT<InputPartitions>;
        using Action = LatitudinalSideHaloPartitionAction<Element>;

        auto const& hp{input_policies.halo_policy()};

        Action action{};

        for (Index rp = 0; rp < nr_partitions0; ++rp)
        {

            for (auto const& [ch, cp, fill_value] :
                 {std::tuple<Index, Index, Element>{0, 0, hp.west_side()},
                  std::tuple<Index, Index, Element>{1, nr_partitions1 - 1, hp.east_side()}})
            {
                halo_partitions(rp, ch) = InputPartition{input_partitions(rp, cp)}.then(

                    [action, locality = localities(rp, cp), min_shape, fill_value = fill_value](
                        InputPartition&& input_partition)
                    { return action(locality, std::move(input_partition), min_shape, fill_value); });
            }
        }

        lue_hpx_assert(all_are_valid(halo_partitions));

        return halo_partitions;
    }


    template<typename InputPolicies, typename InputPartitions, typename Localities>
    auto halo_partitions(
        InputPolicies const& input_policies,
        Localities const& localities,
        ShapeT<InputPartitions> const& min_shape,
        InputPartitions const& input_partitions) -> std::array<InputPartitions, 3>
    {
        lue_hpx_assert(all_are_valid(input_partitions));  // But possibly not ready yet!

        if (input_partitions.nr_elements() == 0)
        {
            return std::array<InputPartitions, 3>{};
        }

        lue_hpx_assert(input_partitions.nr_elements() > 0);

        return std::array<InputPartitions, 3>{
            halo_corner_partitions(input_policies, localities, min_shape, input_partitions),
            halo_longitudinal_side_partitions(input_policies, localities, min_shape, input_partitions),
            halo_latitudinal_side_partitions(input_policies, localities, min_shape, input_partitions)};
    }


    template<std::size_t... idxs, typename InputsPolicies, typename Localities, typename... InputPartitions>
    std::tuple<std::array<InputPartitions, 3>...> halo_partitions(
        std::index_sequence<idxs...>,
        InputsPolicies const& inputs_policies,
        Localities const& localities,
        ShapeT<InputPartitions...> const& min_shape,
        InputPartitions const&... input_partition_collections)
    {
        return std::tuple<std::array<InputPartitions, 3>...>{std::array<InputPartitions, 3>{halo_partitions(
            std::get<idxs>(inputs_policies), localities, min_shape, input_partition_collections)}...};
    }


    /*!
        @brief      Return halo partitions for each collection of input
                    partitions
        @param      inputs_policies For each @a input_partition_collection
                    the input policies
        @param      localities
        @param      input_partition_collection The collection of input
                    partitions for which to create halo partitions
        @return     A tuple with for each collection of input partitions
                    three new collections of partitions: corner partitions,
                    longitudinal side partitions, and latitudinal side
                    partitions
    */
    template<typename InputsPolicies, typename Localities, typename... InputPartitions>
    std::tuple<std::array<InputPartitions, 3>...> halo_partitions(
        InputsPolicies const& inputs_policies,
        Localities const& localities,
        ShapeT<InputPartitions...> const& min_shape,
        InputPartitions const&... input_partition_collection)
    {
        static_assert(std::tuple_size_v<InputsPolicies> == sizeof...(input_partition_collection));

        return halo_partitions(
            std::index_sequence_for<InputPartitions...>(),
            inputs_policies,
            localities,
            min_shape,
            input_partition_collection...);
    }


    template<typename InputPartitions>
    InputPartitions north_west_corner_input_partitions(
        InputPartitions const& input_partitions, std::array<InputPartitions, 3> const& halo_partitions)
    {
        // This block also handles the first partition in case there
        // is only a single row and/or a single column of partitions

        using Shape = ShapeT<InputPartitions>;

        auto const [nr_partitions0, nr_partitions1] = input_partitions.shape();
        auto const& [halo_corner_partitions, halo_longitudinal_side_partitions, halo_latitudinal_side_partitions] =
            halo_partitions;

        InputPartitions result{Shape{{3, 3}}};

        result(0, 0) = halo_corner_partitions(0, 0);
        result(0, 1) = halo_longitudinal_side_partitions(0, 0);
        result(0, 2) =
            nr_partitions1 == 1 ? halo_corner_partitions(0, 1) : halo_longitudinal_side_partitions(0, 1);

        result(1, 0) = halo_latitudinal_side_partitions(0, 0);
        result(1, 1) = input_partitions(0, 0);
        result(1, 2) = nr_partitions1 == 1 ? halo_latitudinal_side_partitions(0, 1) : input_partitions(0, 1);

        result(2, 0) =
            nr_partitions0 == 1 ? halo_corner_partitions(1, 0) : halo_latitudinal_side_partitions(1, 0);
        result(2, 1) = nr_partitions0 == 1 ? halo_longitudinal_side_partitions(1, 0) : input_partitions(1, 0);
        result(2, 2) = nr_partitions0 == 1 ? (nr_partitions1 == 1 ? halo_corner_partitions(1, 1)
                                                                  : halo_longitudinal_side_partitions(1, 1))
                                           : (nr_partitions1 == 1 ? halo_latitudinal_side_partitions(1, 1)
                                                                  : input_partitions(1, 1));

        lue_hpx_assert(all_are_valid(result));

        return result;
    }


    template<std::size_t... idxs, typename... InputPartitionCollection>
    std::tuple<InputPartitionCollection...> north_west_corner_input_partitions(
        std::index_sequence<idxs...>,
        std::tuple<std::array<InputPartitionCollection, 3>...> const& halo_partitions,
        InputPartitionCollection const&... input_partitions)
    {
        return std::tuple<InputPartitionCollection...>{InputPartitionCollection{
            north_west_corner_input_partitions(input_partitions, std::get<idxs>(halo_partitions))}...};
    }


    template<typename InputPartitions>
    InputPartitions north_east_corner_input_partitions(
        InputPartitions const& input_partitions, std::array<InputPartitions, 3> const& halo_partitions)
    {
        // This block also handles the last partition in case there
        // is only a single row of partitions

        using Shape = ShapeT<InputPartitions>;

        auto const [nr_partitions0, nr_partitions1] = input_partitions.shape();
        auto const& [halo_corner_partitions, halo_longitudinal_side_partitions, halo_latitudinal_side_partitions] =
            halo_partitions;

        lue_hpx_assert(nr_partitions1 > 1);

        InputPartitions result{Shape{{3, 3}}};

        result(0, 0) = halo_longitudinal_side_partitions(0, nr_partitions1 - 2);
        result(0, 1) = halo_longitudinal_side_partitions(0, nr_partitions1 - 1);
        result(0, 2) = halo_corner_partitions(0, 1);

        result(1, 0) = input_partitions(0, nr_partitions1 - 2);
        result(1, 1) = input_partitions(0, nr_partitions1 - 1);
        result(1, 2) = halo_latitudinal_side_partitions(0, 1);

        if (nr_partitions0 == 1)
        {
            // Case where nr_partitions1 == 1 is handled by north-west
            // corner logic
            lue_hpx_assert(nr_partitions1 > 1);

            result(2, 0) = halo_longitudinal_side_partitions(1, nr_partitions1 - 2);
            result(2, 1) = halo_longitudinal_side_partitions(1, nr_partitions1 - 1);
            result(2, 2) = halo_corner_partitions(1, 1);
        }
        else
        {
            result(2, 0) = input_partitions(1, nr_partitions1 - 2);
            result(2, 1) = input_partitions(1, nr_partitions1 - 1);
            result(2, 2) = halo_latitudinal_side_partitions(1, 1);
        }

        lue_hpx_assert(all_are_valid(result));

        return result;
    }


    template<std::size_t... idxs, typename... InputPartitionCollection>
    std::tuple<InputPartitionCollection...> north_east_corner_input_partitions(
        std::index_sequence<idxs...>,
        std::tuple<std::array<InputPartitionCollection, 3>...> const& halo_partitions,
        InputPartitionCollection const&... input_partitions)
    {
        return std::tuple<InputPartitionCollection...>{InputPartitionCollection{
            north_east_corner_input_partitions(input_partitions, std::get<idxs>(halo_partitions))}...};
    }


    template<typename InputPartitions>
    InputPartitions south_west_corner_input_partitions(
        InputPartitions const& input_partitions, std::array<InputPartitions, 3> const& halo_partitions)
    {
        // This block also handles the last partition in case there
        // is only a single column of partitions

        using Shape = ShapeT<InputPartitions>;

        auto const [nr_partitions0, nr_partitions1] = input_partitions.shape();
        auto const& [halo_corner_partitions, halo_longitudinal_side_partitions, halo_latitudinal_side_partitions] =
            halo_partitions;

        lue_hpx_assert(nr_partitions0 > 1);

        InputPartitions result{Shape{{3, 3}}};

        result(0, 0) = halo_latitudinal_side_partitions(nr_partitions0 - 2, 0);
        result(0, 1) = input_partitions(nr_partitions0 - 2, 0);
        result(0, 2) = nr_partitions1 == 1 ? halo_latitudinal_side_partitions(nr_partitions0 - 2, 1)
                                           : input_partitions(nr_partitions0 - 2, 1);

        result(1, 0) = halo_latitudinal_side_partitions(nr_partitions0 - 1, 0);
        result(1, 1) = input_partitions(nr_partitions0 - 1, 0);
        result(1, 2) = nr_partitions1 == 1 ? halo_latitudinal_side_partitions(nr_partitions0 - 1, 1)
                                           : input_partitions(nr_partitions0 - 1, 1);

        result(2, 0) = halo_corner_partitions(1, 0);
        result(2, 1) = halo_longitudinal_side_partitions(1, 0);
        result(2, 2) =
            nr_partitions1 == 1 ? halo_corner_partitions(1, 1) : halo_longitudinal_side_partitions(1, 1);

        lue_hpx_assert(all_are_valid(result));

        return result;
    }


    template<std::size_t... idxs, typename... InputPartitionCollection>
    std::tuple<InputPartitionCollection...> south_west_corner_input_partitions(
        std::index_sequence<idxs...>,
        std::tuple<std::array<InputPartitionCollection, 3>...> const& halo_partitions,
        InputPartitionCollection const&... input_partitions)
    {
        return std::tuple<InputPartitionCollection...>{InputPartitionCollection{
            south_west_corner_input_partitions(input_partitions, std::get<idxs>(halo_partitions))}...};
    }


    template<typename InputPartitions>
    InputPartitions south_east_corner_input_partitions(
        InputPartitions const& input_partitions, std::array<InputPartitions, 3> const& halo_partitions)
    {
        using Shape = ShapeT<InputPartitions>;

        auto const [nr_partitions0, nr_partitions1] = input_partitions.shape();
        auto const& [halo_corner_partitions, halo_longitudinal_side_partitions, halo_latitudinal_side_partitions] =
            halo_partitions;

        lue_hpx_assert(nr_partitions0 > 1 && nr_partitions1 > 1);

        InputPartitions result{Shape{{3, 3}}};

        result(0, 0) = input_partitions(nr_partitions0 - 2, nr_partitions1 - 2);
        result(0, 1) = input_partitions(nr_partitions0 - 2, nr_partitions1 - 1);
        result(0, 2) = halo_latitudinal_side_partitions(nr_partitions0 - 2, 1);
        result(1, 0) = input_partitions(nr_partitions0 - 1, nr_partitions1 - 2);
        result(1, 1) = input_partitions(nr_partitions0 - 1, nr_partitions1 - 1);
        result(1, 2) = halo_latitudinal_side_partitions(nr_partitions0 - 1, 1);
        result(2, 0) = halo_longitudinal_side_partitions(1, nr_partitions1 - 2);
        result(2, 1) = halo_longitudinal_side_partitions(1, nr_partitions1 - 1);
        result(2, 2) = halo_corner_partitions(1, 1);

        lue_hpx_assert(all_are_valid(result));

        return result;
    }


    template<std::size_t... idxs, typename... InputPartitionCollection>
    std::tuple<InputPartitionCollection...> south_east_corner_input_partitions(
        std::index_sequence<idxs...>,
        std::tuple<std::array<InputPartitionCollection, 3>...> const& halo_partitions,
        InputPartitionCollection const&... input_partitions)
    {
        return std::tuple<InputPartitionCollection...>{InputPartitionCollection{
            south_east_corner_input_partitions(input_partitions, std::get<idxs>(halo_partitions))}...};
    }


    template<typename InputPartitions>
    InputPartitions north_side_input_partitions(
        Index const c,
        InputPartitions const& input_partitions,
        std::array<InputPartitions, 3> const& halo_partitions)
    {
        // This block also handles the middle partitions in
        // case there is only a single row of partitions

        using Shape = ShapeT<InputPartitions>;

        auto const [nr_partitions0, nr_partitions1] = input_partitions.shape();
        auto const& [halo_corner_partitions, halo_longitudinal_side_partitions, halo_latitudinal_side_partitions] =
            halo_partitions;

        InputPartitions result{Shape{{3, 3}}};

        result(0, 0) = halo_longitudinal_side_partitions(0, c - 1);
        result(0, 1) = halo_longitudinal_side_partitions(0, c);
        result(0, 2) = halo_longitudinal_side_partitions(0, c + 1);

        result(1, 0) = input_partitions(0, c - 1);
        result(1, 1) = input_partitions(0, c);
        result(1, 2) = input_partitions(0, c + 1);

        if (nr_partitions0 == 1)
        {
            // Case where nr_partitions1 == 1 is handled by north-west
            // corner logic
            lue_hpx_assert(nr_partitions1 > 1);

            result(2, 0) = halo_longitudinal_side_partitions(1, c - 1);
            result(2, 1) = halo_longitudinal_side_partitions(1, c);
            result(2, 2) = halo_longitudinal_side_partitions(1, c + 1);
        }
        else
        {
            result(2, 0) = input_partitions(1, c - 1);
            result(2, 1) = input_partitions(1, c);
            result(2, 2) = input_partitions(1, c + 1);
        }

        lue_hpx_assert(all_are_valid(result));

        return result;
    }


    template<std::size_t... idxs, typename... InputPartitionCollection>
    std::tuple<InputPartitionCollection...> north_side_input_partitions(
        Index const c,
        std::index_sequence<idxs...>,
        std::tuple<std::array<InputPartitionCollection, 3>...> const& halo_partitions,
        InputPartitionCollection const&... input_partitions)
    {
        return std::tuple<InputPartitionCollection...>{InputPartitionCollection{
            north_side_input_partitions(c, input_partitions, std::get<idxs>(halo_partitions))}...};
    }


    template<typename InputPartitions>
    InputPartitions south_side_input_partitions(
        Index const c,
        InputPartitions const& input_partitions,
        std::array<InputPartitions, 3> const& halo_partitions)
    {
        using Shape = ShapeT<InputPartitions>;

        auto const [nr_partitions0, nr_partitions1] = input_partitions.shape();
        auto const& [halo_corner_partitions, halo_longitudinal_side_partitions, halo_latitudinal_side_partitions] =
            halo_partitions;

        InputPartitions result{Shape{{3, 3}}};

        result(0, 0) = input_partitions(nr_partitions0 - 2, c - 1);
        result(0, 1) = input_partitions(nr_partitions0 - 2, c);
        result(0, 2) = input_partitions(nr_partitions0 - 2, c + 1);
        result(1, 0) = input_partitions(nr_partitions0 - 1, c - 1);
        result(1, 1) = input_partitions(nr_partitions0 - 1, c);
        result(1, 2) = input_partitions(nr_partitions0 - 1, c + 1);
        result(2, 0) = halo_longitudinal_side_partitions(1, c - 1);
        result(2, 1) = halo_longitudinal_side_partitions(1, c);
        result(2, 2) = halo_longitudinal_side_partitions(1, c + 1);

        lue_hpx_assert(all_are_valid(result));

        return result;
    }


    template<std::size_t... idxs, typename... InputPartitionCollection>
    std::tuple<InputPartitionCollection...> south_side_input_partitions(
        Index const c,
        std::index_sequence<idxs...>,
        std::tuple<std::array<InputPartitionCollection, 3>...> const& halo_partitions,
        InputPartitionCollection const&... input_partitions)
    {
        return std::tuple<InputPartitionCollection...>{InputPartitionCollection{
            south_side_input_partitions(c, input_partitions, std::get<idxs>(halo_partitions))}...};
    }


    template<typename InputPartitions>
    InputPartitions west_side_input_partitions(
        Index const r,
        InputPartitions const& input_partitions,
        std::array<InputPartitions, 3> const& halo_partitions)
    {
        // This block also handles the middle partitions in
        // case there is only a single column of partitions

        using Shape = ShapeT<InputPartitions>;

        auto const [nr_partitions0, nr_partitions1] = input_partitions.shape();
        auto const& [halo_corner_partitions, halo_longitudinal_side_partitions, halo_latitudinal_side_partitions] =
            halo_partitions;

        InputPartitions result{Shape{{3, 3}}};

        result(0, 0) = halo_latitudinal_side_partitions(r - 1, 0);
        result(0, 1) = input_partitions(r - 1, 0);
        result(0, 2) =
            nr_partitions1 == 1 ? halo_latitudinal_side_partitions(r - 1, 1) : input_partitions(r - 1, 1);

        result(1, 0) = halo_latitudinal_side_partitions(r, 0);
        result(1, 1) = input_partitions(r, 0);
        result(1, 2) = nr_partitions1 == 1 ? halo_latitudinal_side_partitions(r, 1) : input_partitions(r, 1);

        result(2, 0) = halo_latitudinal_side_partitions(r + 1, 0);
        result(2, 1) = input_partitions(r + 1, 0);
        result(2, 2) =
            nr_partitions1 == 1 ? halo_latitudinal_side_partitions(r + 1, 1) : input_partitions(r + 1, 1);

        lue_hpx_assert(all_are_valid(result));

        return result;
    }


    template<std::size_t... idxs, typename... InputPartitionCollection>
    std::tuple<InputPartitionCollection...> west_side_input_partitions(
        Index const r,
        std::index_sequence<idxs...>,
        std::tuple<std::array<InputPartitionCollection, 3>...> const& halo_partitions,
        InputPartitionCollection const&... input_partitions)
    {
        return std::tuple<InputPartitionCollection...>{InputPartitionCollection{
            west_side_input_partitions(r, input_partitions, std::get<idxs>(halo_partitions))}...};
    }


    template<typename InputPartitions>
    InputPartitions east_side_input_partitions(
        Index const r,
        InputPartitions const& input_partitions,
        std::array<InputPartitions, 3> const& halo_partitions)
    {
        using Shape = ShapeT<InputPartitions>;

        auto const [nr_partitions0, nr_partitions1] = input_partitions.shape();
        auto const& [halo_corner_partitions, halo_longitudinal_side_partitions, halo_latitudinal_side_partitions] =
            halo_partitions;

        InputPartitions result{Shape{{3, 3}}};

        result(0, 0) = input_partitions(r - 1, nr_partitions1 - 2);
        result(0, 1) = input_partitions(r - 1, nr_partitions1 - 1);
        result(0, 2) = halo_latitudinal_side_partitions(r - 1, 1);
        result(1, 0) = input_partitions(r, nr_partitions1 - 2);
        result(1, 1) = input_partitions(r, nr_partitions1 - 1);
        result(1, 2) = halo_latitudinal_side_partitions(r, 1);
        result(2, 0) = input_partitions(r + 1, nr_partitions1 - 2);
        result(2, 1) = input_partitions(r + 1, nr_partitions1 - 1);
        result(2, 2) = halo_latitudinal_side_partitions(r + 1, 1);

        lue_hpx_assert(all_are_valid(result));

        return result;
    }


    template<std::size_t... idxs, typename... InputPartitionCollection>
    std::tuple<InputPartitionCollection...> east_side_input_partitions(
        Index const r,
        std::index_sequence<idxs...>,
        std::tuple<std::array<InputPartitionCollection, 3>...> const& halo_partitions,
        InputPartitionCollection const&... input_partitions)
    {
        return std::tuple<InputPartitionCollection...>{InputPartitionCollection{
            east_side_input_partitions(r, input_partitions, std::get<idxs>(halo_partitions))}...};
    }


    template<typename InputPartitions>
    InputPartitions inner_input_partitions(
        Index const r, Index const c, InputPartitions const& input_partitions)
    {
        using Shape = ShapeT<InputPartitions>;

        [[maybe_unused]] auto const [nr_partitions0, nr_partitions1] = input_partitions.shape();

        lue_hpx_assert(r < nr_partitions0);
        lue_hpx_assert(c < nr_partitions1);

        // Assume one neighboring partition is enough
        Count const radius{1};

        // Create collection of partitions containing the current
        // partition and its neighboring partitions
        InputPartitions result{Shape{{3, 3}}};

        for (Index i = 0; i < 2 * radius + 1; ++i)
        {
            for (Index j = 0; j < 2 * radius + 1; ++j)
            {
                result(i, j) = input_partitions(r - radius + i, c - radius + j);
            }
        }

        lue_hpx_assert(all_are_valid(result));

        return result;
    }


    template<typename... InputPartitionCollection>
    std::tuple<InputPartitionCollection...> inner_input_partitions(
        Index const r, Index const c, InputPartitionCollection const&... input_partitions)
    {
        return std::tuple<InputPartitionCollection...>{
            InputPartitionCollection{inner_input_partitions(r, c, input_partitions)}...};
    }

}  // namespace lue::detail
