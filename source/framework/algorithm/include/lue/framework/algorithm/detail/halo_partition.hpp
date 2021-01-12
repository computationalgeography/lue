#pragma once
#include "lue/framework/core/component/partitioned_array.hpp"


namespace lue {
namespace detail {

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
