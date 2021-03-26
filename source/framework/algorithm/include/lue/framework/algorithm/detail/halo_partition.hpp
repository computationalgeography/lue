#pragma once
#include "lue/framework/algorithm/flow_direction.hpp"
#include "lue/framework/core/component/partitioned_array.hpp"


namespace lue {
namespace detail {

template<
    typename Element>
ArrayPartition<Element, 2> corner_halo_partition(
    Element const fill_value)
{
    // Asynchronously create a new partition:
    // - With a shape based on the partition passsed in
    // - Filled with the fill_value passed in

    using Partition = ArrayPartition<Element, 2>;
    using Shape = ShapeT<Partition>;
    using Offset = OffsetT<Partition>;

    return Partition{hpx::find_here(), Offset{}, Shape{{1, 1}}, fill_value};
}


template<
    typename Element>
ArrayPartition<Element, 2> longitudinal_side_halo_partition(
    ArrayPartition<Element, 2> const& partition,
    Element const fill_value)
{
    // Asynchronously create a new partition:
    // - With a shape based on the partition passsed in
    // - Filled with the fill_value passed in

    using Partition = ArrayPartition<Element, 2>;
    using Shape = ShapeT<Partition>;
    using Offset = OffsetT<Partition>;

    return hpx::dataflow(
        hpx::launch::async,
        hpx::util::unwrapping(

                [fill_value](
                    Shape const& shape)
                {
                    return Partition{hpx::find_here(), Offset{}, Shape{{1, shape[1]}}, fill_value};
                }

            ),
        partition.shape());
}


template<
    typename Element>
ArrayPartition<Element, 2> latitudinal_side_halo_partition(
    ArrayPartition<Element, 2> const& partition,
    Element const fill_value)
{
    // Asynchronously create a new partition:
    // - With a shape based on the partition passsed in
    // - Filled with the fill_value passed in

    using Partition = ArrayPartition<Element, 2>;
    using Shape = ShapeT<Partition>;
    using Offset = OffsetT<Partition>;

    return hpx::dataflow(
        hpx::launch::async,
        hpx::util::unwrapping(

                [fill_value](
                    Shape const& shape)
                {
                    return Partition{hpx::find_here(), Offset{}, Shape{{shape[0], 1}}, fill_value};
                }

            ),
        partition.shape());
}


template<
    typename Element>
struct CornerHaloPartitionAction:
    hpx::actions::make_action<
        decltype(&corner_halo_partition<Element>),
        &corner_halo_partition<Element>,
        CornerHaloPartitionAction<Element>>::type
{};


template<
    typename Element>
struct LongitudinalSideHaloPartitionAction:
    hpx::actions::make_action<
        decltype(&longitudinal_side_halo_partition<Element>),
        &longitudinal_side_halo_partition<Element>,
        LongitudinalSideHaloPartitionAction<Element>>::type
{};


template<
    typename Element>
struct LatitudinalSideHaloPartitionAction:
    hpx::actions::make_action<
        decltype(&latitudinal_side_halo_partition<Element>),
        &latitudinal_side_halo_partition<Element>,
        LatitudinalSideHaloPartitionAction<Element>>::type
{};


template<
    typename FlowDirectionPartitions,
    typename Localities>
FlowDirectionPartitions halo_corner_partitions(
    Localities const& localities)
{
    auto const [nr_partitions0, nr_partitions1] = localities.shape();
    FlowDirectionPartitions halo_partitions{{2, 2}};

    // Corner halo partitions
    //     +----+----+
    //     | NW | NE |
    //     +----+----+
    //     | SW | SE |
    //     +----+----+

    using FlowDirectionElement = ElementT<FlowDirectionPartitions>;
    using Action = CornerHaloPartitionAction<FlowDirectionElement>;

    Action action{};

    // North-west corner halo partition
    halo_partitions(0, 0) =
        hpx::async(action, localities(0, 0), north_west<FlowDirectionElement>);

    // North-east corner halo partition
    halo_partitions(0, 1) =
        hpx::async(action, localities(0, nr_partitions1 - 1), north_east<FlowDirectionElement>);

    // South-west corner halo partition
    halo_partitions(1, 0) =
        hpx::async(action, localities(nr_partitions0 - 1, 0), south_west<FlowDirectionElement>);

    // South-east corner halo partition
    halo_partitions(1, 1) =
        hpx::async(action, localities(nr_partitions0 - 1, nr_partitions1 - 1),
            south_east<FlowDirectionElement>);

    return halo_partitions;
}


template<
    typename FlowDirectionPartitions,
    typename Localities>
FlowDirectionPartitions halo_longitudinal_side_partitions(
    Localities const& localities,
    FlowDirectionPartitions const& flow_direction_partitions)
{
    auto const [nr_partitions0, nr_partitions1] = localities.shape();
    FlowDirectionPartitions halo_partitions{{2, nr_partitions1}};

    // Longitudinal side halo partitions
    //     +---+---+---+
    //     | N | N | N |
    //     +---+---+---+
    //     | S | S | S |
    //     +---+---+---+

    using FlowDirectionElement = ElementT<FlowDirectionPartitions>;
    using Action = LongitudinalSideHaloPartitionAction<FlowDirectionElement>;

    Action action{};

    for(auto const [rh, rp, halo_flow_direction]: {
            std::tuple<Index, Index, FlowDirectionElement>{
                0, 0, north<FlowDirectionElement>},
            std::tuple<Index, Index, FlowDirectionElement>{
                1, nr_partitions0 - 1, south<FlowDirectionElement>}
        })
    {
        for(Index cp = 0; cp < nr_partitions1; ++cp)
        {
            halo_partitions(rh, cp) =
                hpx::async(action, localities(rp, cp),
                    flow_direction_partitions(rp, cp), halo_flow_direction);
        }
    }

    return halo_partitions;
}


template<
    typename FlowDirectionPartitions,
    typename Localities>
FlowDirectionPartitions halo_latitudinal_side_partitions(
    Localities const& localities,
    FlowDirectionPartitions const& flow_direction_partitions)
{
    auto const [nr_partitions0, nr_partitions1] = localities.shape();
    FlowDirectionPartitions halo_partitions{{nr_partitions0, 2}};

    // Latitudinal sides halo partitions
    //     +---+---+
    //     | W | E |
    //     +---+---+
    //     | W | E |
    //     +---+---+
    //     | W | E |
    //     +---+---+

    using FlowDirectionElement = ElementT<FlowDirectionPartitions>;
    using Action = LatitudinalSideHaloPartitionAction<FlowDirectionElement>;

    Action action{};

    for(Index rp = 0; rp < nr_partitions0; ++rp) {

        for(auto const [ch, cp, halo_flow_direction]: {
                std::tuple<Index, Index, FlowDirectionElement>{
                    0, 0, west<FlowDirectionElement>},
                std::tuple<Index, Index, FlowDirectionElement>{
                    1, nr_partitions1 - 1, east<FlowDirectionElement>}
            })
        {
            halo_partitions(rp, ch) =
                hpx::async(action, localities(rp, cp),
                    flow_direction_partitions(rp, cp), halo_flow_direction);
        }
    }

    return halo_partitions;
}


template<
    typename InputPartitions>
InputPartitions north_west_corner_input_partitions(
    InputPartitions const& input_partitions,
    InputPartitions const& halo_corner_partitions,
    InputPartitions const& halo_longitudinal_side_partitions,
    InputPartitions const& halo_latitudinal_side_partitions)
{
    // This block also handles the first partition in case there
    // is only a single row and/or a single column of partitions

    using Shape = ShapeT<InputPartitions>;

    auto const [nr_partitions0, nr_partitions1] = input_partitions.shape();

    InputPartitions result{Shape{{3, 3}}};

    result(0, 0) = halo_corner_partitions(0, 0);
    result(0, 1) = halo_longitudinal_side_partitions(0, 0);
    result(0, 2) = nr_partitions1 == 1
        ? halo_corner_partitions(0, 1)
        : halo_longitudinal_side_partitions(0, 1);

    result(1, 0) = halo_latitudinal_side_partitions(0, 0);
    result(1, 1) = input_partitions(0, 0);
    result(1, 2) = nr_partitions1 == 1
        ? halo_latitudinal_side_partitions(0, 1)
        : input_partitions(0, 1);

    result(2, 0) = nr_partitions0 == 1
        ? halo_corner_partitions(1, 0)
        : halo_latitudinal_side_partitions(1, 0);
    result(2, 1) = nr_partitions0 == 1
        ? halo_longitudinal_side_partitions(1, 0)
        : input_partitions(1, 0);
    result(2, 2) = nr_partitions0 == 1
        ? (nr_partitions1 == 1
            ? halo_corner_partitions(1, 1)
            : halo_longitudinal_side_partitions(1, 1))
        : (nr_partitions1 == 1
            ? halo_latitudinal_side_partitions(1, 1)
            : input_partitions(1, 1));

    lue_assert(all_are_valid(result));

    return result;
}


template<
    typename InputPartitions>
InputPartitions north_east_corner_input_partitions(
    InputPartitions const& input_partitions,
    InputPartitions const& halo_corner_partitions,
    InputPartitions const& halo_longitudinal_side_partitions,
    InputPartitions const& halo_latitudinal_side_partitions)
{
    // This block also handles the last partition in case there
    // is only a single row of partitions

    using Shape = ShapeT<InputPartitions>;

    auto const [nr_partitions0, nr_partitions1] = input_partitions.shape();

    lue_assert(nr_partitions1 > 1);

    InputPartitions result{Shape{{3, 3}}};

    result(0, 0) = halo_longitudinal_side_partitions(0, nr_partitions1 - 2);
    result(0, 1) = halo_longitudinal_side_partitions(0, nr_partitions1 - 1);
    result(0, 2) = halo_corner_partitions(0, 1);

    result(1, 0) = input_partitions(0, nr_partitions1 - 2);
    result(1, 1) = input_partitions(0, nr_partitions1 - 1);
    result(1, 2) = halo_latitudinal_side_partitions(0, 1);

    if(nr_partitions0 == 1)
    {
        // Case where nr_partitions1 == 1 is handled by north-west
        // corner logic
        lue_assert(nr_partitions1 > 1);

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

    lue_assert(all_are_valid(result));

    return result;
}


template<
    typename InputPartitions>
InputPartitions south_west_corner_input_partitions(
    InputPartitions const& input_partitions,
    InputPartitions const& halo_corner_partitions,
    InputPartitions const& halo_longitudinal_side_partitions,
    InputPartitions const& halo_latitudinal_side_partitions)
{
    // This block also handles the last partition in case there
    // is only a single column of partitions

    using Shape = ShapeT<InputPartitions>;

    auto const [nr_partitions0, nr_partitions1] = input_partitions.shape();

    lue_assert(nr_partitions0 > 1);

    InputPartitions result{Shape{{3, 3}}};

    result(0, 0) = halo_latitudinal_side_partitions(nr_partitions0 - 2, 0);
    result(0, 1) = input_partitions(nr_partitions0 - 2, 0);
    result(0, 2) = nr_partitions1 == 1
        ? halo_latitudinal_side_partitions(nr_partitions0 - 2, 1)
        : input_partitions(nr_partitions0 - 2, 1);

    result(1, 0) = halo_latitudinal_side_partitions(nr_partitions0 - 1, 0);
    result(1, 1) = input_partitions(nr_partitions0 - 1, 0);
    result(1, 2) = nr_partitions1 == 1
        ? halo_latitudinal_side_partitions(nr_partitions0 - 1, 1)
        : input_partitions(nr_partitions0 - 1, 1);

    result(2, 0) = halo_corner_partitions(1, 0);
    result(2, 1) = halo_longitudinal_side_partitions(1, 0);
    result(2, 2) = nr_partitions1 == 1
        ? halo_corner_partitions(1, 1)
        : halo_longitudinal_side_partitions(1, 1);

    lue_assert(all_are_valid(result));

    return result;
}


template<
    typename InputPartitions>
InputPartitions south_east_corner_input_partitions(
    InputPartitions const& input_partitions,
    InputPartitions const& halo_corner_partitions,
    InputPartitions const& halo_longitudinal_side_partitions,
    InputPartitions const& halo_latitudinal_side_partitions)
{
    using Shape = ShapeT<InputPartitions>;

    auto const [nr_partitions0, nr_partitions1] = input_partitions.shape();

    lue_assert(nr_partitions0 > 1 && nr_partitions1 > 1);

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

    lue_assert(all_are_valid(result));

    return result;
}


template<
    typename InputPartitions>
InputPartitions north_side_input_partitions(
    Index const c,
    InputPartitions const& input_partitions,
    InputPartitions const& halo_longitudinal_side_partitions)
{
    // This block also handles the middle partitions in
    // case there is only a single row of partitions

    using Shape = ShapeT<InputPartitions>;

    auto const [nr_partitions0, nr_partitions1] = input_partitions.shape();

    InputPartitions result{Shape{{3, 3}}};

    result(0, 0) = halo_longitudinal_side_partitions(0, c - 1);
    result(0, 1) = halo_longitudinal_side_partitions(0, c    );
    result(0, 2) = halo_longitudinal_side_partitions(0, c + 1);

    result(1, 0) = input_partitions(0, c - 1);
    result(1, 1) = input_partitions(0, c    );
    result(1, 2) = input_partitions(0, c + 1);

    if(nr_partitions0 == 1)
    {
        // Case where nr_partitions1 == 1 is handled by north-west
        // corner logic
        lue_assert(nr_partitions1 > 1);

        result(2, 0) = halo_longitudinal_side_partitions(1, c - 1);
        result(2, 1) = halo_longitudinal_side_partitions(1, c    );
        result(2, 2) = halo_longitudinal_side_partitions(1, c + 1);
    }
    else
    {
        result(2, 0) = input_partitions(1, c - 1);
        result(2, 1) = input_partitions(1, c    );
        result(2, 2) = input_partitions(1, c + 1);
    }

    lue_assert(all_are_valid(result));

    return result;
}


template<
    typename InputPartitions>
InputPartitions south_side_input_partitions(
    Index const c,
    InputPartitions const& input_partitions,
    InputPartitions const& halo_longitudinal_side_partitions)
{
    using Shape = ShapeT<InputPartitions>;

    auto const [nr_partitions0, nr_partitions1] = input_partitions.shape();

    InputPartitions result{Shape{{3, 3}}};

    result(0, 0) = input_partitions(nr_partitions0 - 2, c - 1);
    result(0, 1) = input_partitions(nr_partitions0 - 2, c    );
    result(0, 2) = input_partitions(nr_partitions0 - 2, c + 1);
    result(1, 0) = input_partitions(nr_partitions0 - 1, c - 1);
    result(1, 1) = input_partitions(nr_partitions0 - 1, c    );
    result(1, 2) = input_partitions(nr_partitions0 - 1, c + 1);
    result(2, 0) = halo_longitudinal_side_partitions(1, c - 1);
    result(2, 1) = halo_longitudinal_side_partitions(1, c    );
    result(2, 2) = halo_longitudinal_side_partitions(1, c + 1);

    lue_assert(all_are_valid(result));

    return result;
}


template<
    typename InputPartitions>
InputPartitions west_side_input_partitions(
    Index const r,
    InputPartitions const& input_partitions,
    InputPartitions const& halo_latitudinal_side_partitions)
{
    // This block also handles the middle partitions in
    // case there is only a single column of partitions

    using Shape = ShapeT<InputPartitions>;

    auto const [nr_partitions0, nr_partitions1] = input_partitions.shape();

    InputPartitions result{Shape{{3, 3}}};

    result(0, 0) = halo_latitudinal_side_partitions(r - 1, 0);
    result(0, 1) = input_partitions(r - 1, 0);
    result(0, 2) = nr_partitions1 == 1
        ? halo_latitudinal_side_partitions(r - 1, 1)
        : input_partitions(r - 1, 1);

    result(1, 0) = halo_latitudinal_side_partitions(r, 0);
    result(1, 1) = input_partitions(r, 0);
    result(1, 2) = nr_partitions1 == 1
        ? halo_latitudinal_side_partitions(r, 1)
        : input_partitions(r, 1);

    result(2, 0) = halo_latitudinal_side_partitions(r + 1, 0);
    result(2, 1) = input_partitions(r + 1, 0);
    result(2, 2) = nr_partitions1 == 1
        ? halo_latitudinal_side_partitions(r + 1, 1)
        : input_partitions(r + 1, 1);

    lue_assert(all_are_valid(result));

    return result;
}


template<
    typename InputPartitions>
InputPartitions east_side_input_partitions(
    Index const r,
    InputPartitions const& input_partitions,
    InputPartitions const& halo_latitudinal_side_partitions)
{
    using Shape = ShapeT<InputPartitions>;

    auto const [nr_partitions0, nr_partitions1] = input_partitions.shape();

    InputPartitions result{Shape{{3, 3}}};

    result(0, 0) = input_partitions(r - 1, nr_partitions1 - 2);
    result(0, 1) = input_partitions(r - 1, nr_partitions1 - 1);
    result(0, 2) = halo_latitudinal_side_partitions(r - 1, 1);
    result(1, 0) = input_partitions(r    , nr_partitions1 - 2);
    result(1, 1) = input_partitions(r    , nr_partitions1 - 1);
    result(1, 2) = halo_latitudinal_side_partitions(r    , 1);
    result(2, 0) = input_partitions(r + 1, nr_partitions1 - 2);
    result(2, 1) = input_partitions(r + 1, nr_partitions1 - 1);
    result(2, 2) = halo_latitudinal_side_partitions(r + 1, 1);

    lue_assert(all_are_valid(result));

    return result;
}


template<
    typename InputPartitions>
InputPartitions inner_input_partitions(
    Index const r,
    Index const c,
    InputPartitions const& input_partitions)
{
    using Shape = ShapeT<InputPartitions>;

    auto const [nr_partitions0, nr_partitions1] = input_partitions.shape();

    lue_assert(r < nr_partitions0);
    lue_assert(c < nr_partitions1);

    // Assume one neighboring partition is enough
    Count const radius{1};

    // Create collection of partitions containing the current
    // partition and its neighboring partitions
    InputPartitions result{Shape{{3, 3}}};

    for(Index i = 0; i < 2 * radius + 1; ++i) {
        for(Index j = 0; j < 2 * radius + 1; ++j)
        {
            result(i, j) = input_partitions(r - radius + i, c - radius + j);
        }
    }

    lue_assert(all_are_valid(result));

    return result;
}

}  // namespace detail
}  // namespace lue
