#define BOOST_TEST_MODULE lue framework algorithm d8_flow_direction
#include "lue/framework/algorithm/value_policies/d8_flow_direction.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


BOOST_AUTO_TEST_CASE(south)
{
    using Elevation = lue::FloatingPointElement<0>;
    using FlowDirection = lue::FlowDirectionElement;
    std::size_t const rank = 2;

    using ElevationArray = lue::PartitionedArray<Elevation, rank>;
    using FlowDirectionArray = lue::PartitionedArray<FlowDirection, rank>;
    using Shape = lue::ShapeT<ElevationArray>;

    Shape const array_shape{{5, 5}};
    Shape const partition_shape{{5, 5}};

    auto const n{lue::north<FlowDirection>};
    auto const e{lue::east<FlowDirection>};
    auto const s{lue::south<FlowDirection>};
    auto const w{lue::west<FlowDirection>};

    auto const elevation = lue::test::create_partitioned_array<ElevationArray>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                9, 9, 9, 9, 9,
                8, 8, 8, 8, 8,
                7, 7, 7, 7, 7,
                6, 6, 6, 6, 6,
                5, 5, 5, 5, 5,
            },
            // clang-format on
            // NOLINTEND
        });
    auto const flow_direction_we_got = lue::value_policies::d8_flow_direction(elevation);
    auto const flow_direction_we_want = lue::test::create_partitioned_array<FlowDirectionArray>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                n, n, n, n, n,
                w, s, s, s, e,
                w, s, s, s, e,
                w, s, s, s, e,
                w, s, s, s, e,
            },
            // clang-format on
            // NOLINTEND
        });

    lue::test::check_arrays_are_equal(flow_direction_we_got, flow_direction_we_want);
}


BOOST_AUTO_TEST_CASE(north)
{
    using Elevation = lue::FloatingPointElement<0>;
    using FlowDirection = lue::FlowDirectionElement;
    std::size_t const rank = 2;

    using ElevationArray = lue::PartitionedArray<Elevation, rank>;
    using FlowDirectionArray = lue::PartitionedArray<FlowDirection, rank>;
    using Shape = lue::ShapeT<ElevationArray>;

    Shape const array_shape{{5, 5}};
    Shape const partition_shape{{5, 5}};

    auto const n{lue::north<FlowDirection>};
    auto const e{lue::east<FlowDirection>};
    auto const s{lue::south<FlowDirection>};
    auto const w{lue::west<FlowDirection>};

    auto const elevation = lue::test::create_partitioned_array<ElevationArray>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                5, 5, 5, 5, 5,
                6, 6, 6, 6, 6,
                7, 7, 7, 7, 7,
                8, 8, 8, 8, 8,
                9, 9, 9, 9, 9,
            },
            // clang-format on
            // NOLINTEND
        });
    auto const flow_direction_we_got = lue::value_policies::d8_flow_direction(elevation);
    auto const flow_direction_we_want = lue::test::create_partitioned_array<FlowDirectionArray>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                n, n, n, n, n,
                w, n, n, n, e,
                w, n, n, n, e,
                w, n, n, n, e,
                w, s, s, s, e,
            },
            // clang-format on
            // NOLINTEND
        });

    lue::test::check_arrays_are_equal(flow_direction_we_got, flow_direction_we_want);
}


BOOST_AUTO_TEST_CASE(east)
{
    using Elevation = lue::FloatingPointElement<0>;
    using FlowDirection = lue::FlowDirectionElement;
    std::size_t const rank = 2;

    using ElevationArray = lue::PartitionedArray<Elevation, rank>;
    using FlowDirectionArray = lue::PartitionedArray<FlowDirection, rank>;
    using Shape = lue::ShapeT<ElevationArray>;

    Shape const array_shape{{5, 5}};
    Shape const partition_shape{{5, 5}};

    auto const n{lue::north<FlowDirection>};
    auto const e{lue::east<FlowDirection>};
    auto const s{lue::south<FlowDirection>};
    auto const w{lue::west<FlowDirection>};

    auto const elevation = lue::test::create_partitioned_array<ElevationArray>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                9, 8, 7, 6, 5,
                9, 8, 7, 6, 5,
                9, 8, 7, 6, 5,
                9, 8, 7, 6, 5,
                9, 8, 7, 6, 5,
            },
            // clang-format on
            // NOLINTEND
        });
    auto const flow_direction_we_got = lue::value_policies::d8_flow_direction(elevation);
    auto const flow_direction_we_want = lue::test::create_partitioned_array<FlowDirectionArray>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                n, n, n, n, n,
                w, e, e, e, e,
                w, e, e, e, e,
                w, e, e, e, e,
                w, s, s, s, e,
            },
            // clang-format on
            // NOLINTEND
        });

    lue::test::check_arrays_are_equal(flow_direction_we_got, flow_direction_we_want);
}


BOOST_AUTO_TEST_CASE(west)
{
    using Elevation = lue::FloatingPointElement<0>;
    using FlowDirection = lue::FlowDirectionElement;
    std::size_t const rank = 2;

    using ElevationArray = lue::PartitionedArray<Elevation, rank>;
    using FlowDirectionArray = lue::PartitionedArray<FlowDirection, rank>;
    using Shape = lue::ShapeT<ElevationArray>;

    Shape const array_shape{{5, 5}};
    Shape const partition_shape{{5, 5}};

    auto const n{lue::north<FlowDirection>};
    auto const e{lue::east<FlowDirection>};
    auto const s{lue::south<FlowDirection>};
    auto const w{lue::west<FlowDirection>};

    auto const elevation = lue::test::create_partitioned_array<ElevationArray>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                5, 6, 7, 8, 9,
                5, 6, 7, 8, 9,
                5, 6, 7, 8, 9,
                5, 6, 7, 8, 9,
                5, 6, 7, 8, 9,
            },
            // clang-format on
            // NOLINTEND
        });
    auto const flow_direction_we_got = lue::value_policies::d8_flow_direction(elevation);
    auto const flow_direction_we_want = lue::test::create_partitioned_array<FlowDirectionArray>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                n, n, n, n, n,
                w, w, w, w, e,
                w, w, w, w, e,
                w, w, w, w, e,
                w, s, s, s, e,
            },
            // clang-format on
            // NOLINTEND
        });

    lue::test::check_arrays_are_equal(flow_direction_we_got, flow_direction_we_want);
}


BOOST_AUTO_TEST_CASE(barnes_2017)
{
    // ElevationArray and network from paper:
    //     Parallel non-divergent flow accumulation for trillion cell
    //     digital elevation models on desktops or clusters

    using Elevation = lue::FloatingPointElement<0>;
    using FlowDirection = lue::FlowDirectionElement;
    std::size_t const rank = 2;

    using ElevationArray = lue::PartitionedArray<Elevation, rank>;
    using FlowDirectionArray = lue::PartitionedArray<FlowDirection, rank>;
    using Shape = lue::ShapeT<ElevationArray>;

    Shape const array_shape{{21, 21}};
    Shape const partition_shape{{7, 7}};

    auto const n{lue::north<FlowDirection>};
    auto const ne{lue::north_east<FlowDirection>};
    auto const e{lue::east<FlowDirection>};
    auto const se{lue::south_east<FlowDirection>};
    auto const s{lue::south<FlowDirection>};
    auto const sw{lue::south_west<FlowDirection>};
    auto const w{lue::west<FlowDirection>};
    auto const nw{lue::north_west<FlowDirection>};

    auto const elevation = lue::test::create_partitioned_array<ElevationArray>(
        array_shape,
        partition_shape,
        {
            {9, 9, 7, 6, 7, 6, 4, 6, 7, 6, 5, 5, 4, 4, 3, 5, 5, 4, 4, 4, 4, 1, 3, 4, 4,
             4, 4, 4, 5, 4, 4, 4, 4, 4, 4, 6, 4, 4, 4, 4, 5, 6, 7, 4, 4, 4, 4, 5, 7},
            {3, 2, 3, 4, 2, 1, 2, 4, 4, 4, 5, 3, 3, 5, 4, 4, 4, 5, 4, 5, 6, 4, 4, 4, 5,
             6, 6, 6, 5, 6, 6, 7, 5, 5, 6, 7, 8, 8, 6, 5, 5, 6, 9, 9, 8, 5, 5, 5, 6},
            {3, 4, 4, 5, 5, 6, 7, 6, 6, 5, 4, 4, 6, 8, 6, 6, 5, 4, 4, 5, 6, 6, 5, 4, 4,
             4, 4, 3, 6, 5, 4, 4, 4, 4, 4, 7, 6, 4, 4, 4, 4, 4, 8, 7, 4, 4, 4, 4, 4},
            {8, 6, 5, 5, 7, 6, 6, 6, 7, 8, 7, 8, 7, 6, 5, 7, 8, 8, 7, 7, 6, 6, 6, 6, 6,
             6, 5, 5, 4, 4, 4, 4, 6, 7, 8, 4, 4, 4, 5, 6, 7, 7, 7, 5, 5, 7, 7, 6, 6},
            {8, 7, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 7, 5, 5, 5, 5,
             5, 5, 7, 7, 6, 5, 5, 5, 5, 7, 8, 7, 5, 5, 5, 5, 7, 7, 7, 6, 6, 5, 5, 6},
            {8, 8, 6, 4, 4, 4, 6, 7, 7, 6, 4, 4, 5, 6, 7, 7, 6, 4, 5, 6, 7, 6, 6, 6, 6,
             5, 6, 8, 6, 7, 8, 7, 7, 6, 6, 6, 7, 7, 7, 6, 5, 4, 6, 5, 5, 5, 4, 4, 3},
            {8, 8, 8, 7, 6, 6, 6, 8, 8, 7, 7, 6, 6, 6, 8, 7, 6, 6, 6, 6, 6, 9, 7, 5, 4,
             6, 7, 7, 8, 6, 5, 4, 6, 7, 7, 4, 4, 4, 5, 5, 6, 6, 0, 2, 3, 5, 5, 6, 6},
            {7, 8, 8, 8, 7, 5, 4, 7, 8, 8, 8, 7, 5, 4, 7, 7, 7, 7, 6, 6, 6, 6, 5, 4, 4,
             5, 6, 6, 6, 5, 5, 6, 4, 5, 6, 6, 6, 5, 6, 4, 4, 6, 4, 6, 6, 3, 3, 4, 5},
            {4, 4, 3, 3, 3, 3, 4, 5, 4, 3, 3, 3, 3, 4, 5, 4, 3, 3, 4, 5, 5, 6, 5, 5, 3,
             5, 7, 8, 6, 5, 5, 6, 6, 6, 6, 5, 5, 5, 8, 7, 5, 3, 3, 3, 5, 9, 7, 4, 1},
        });

    auto const flow_direction_we_want = lue::test::create_partitioned_array<FlowDirectionArray>(
        array_shape,
        partition_shape,
        {
            {nw, n,  n, n, n,  n,  n, w, sw, se, s, e, e, ne, w, sw, sw, sw, w,  ne, n, w,  w, w, w,
             sw, ne, n, w, nw, nw, w, w, n,  nw, w, n, n, nw, w, w,  n,  w,  ne, n,  n, nw, w, w},
            {n, n,  n,  n, n, n, n,  ne, n, nw, ne, ne, n,  nw, n, n,  n, ne, n,  n, nw, n, n, nw, w,
             n, nw, nw, w, n, n, nw, nw, w, w,  nw, nw, se, e,  n, nw, w, nw, se, e, ne, n, n, w},
            {n, n, n, n, n, n, ne, nw, nw, n, nw, w, w, e, nw, se, e, n, se, se, e,  e, e, se, se,
             e, e, e, e, e, e, e,  e,  ne, e, ne, e, e, e, ne, n,  e, w, e,  ne, ne, n, e, e},
            {w,  n, n, n, n, nw, nw, w, ne, n, n, nw, n, e, w,  w, s, s, se, s, e,  w,  s,  s, s,
             sw, e, e, w, w, w,  w,  w, n,  n, w, w,  w, w, nw, w, s, w, n,  n, nw, nw, nw, w},
            {s, e,  ne, n, n, n, w,  se, s, s, s,  n, nw, w, e,  se, se, s, s,  sw, w, ne, e,  se, s,
             s, sw, w,  n, n, e, se, s,  s, w, ne, e, ne, e, se, s,  w,  w, ne, n,  n, e,  se, s},
            {w,  ne, n, ne, e, ne, e,  w, e,  e, e,  ne, w, e,  s, e,  e, ne, w,  nw, e, e, e, ne, n,
             nw, w,  e, s,  w, n,  ne, n, se, e, se, s,  s, se, s, se, e, s,  se, s,  s, s, e, e},
            {w, n, n, nw, se, s, sw, w, se, s, e, s,  sw, w,  w,  se, se, s,  sw, w, se, w, e, e, s,
             w, w, n, w,  s,  e, sw, w, sw, e, w, sw, sw, sw, nw, w,  se, sw, s,  s, s,  s, s, s},
            {w, ne, n,  ne, n,  e, e, w, w, s,  se, e,  e,  ne, se, se, s,  s, sw, ne, n, e, e, e, se,
             w, sw, ne, e,  ne, n, n, s, w, sw, s,  sw, se, s,  s,  sw, se, s, s,  s,  s, s, s, s},
            {e, e,  e, e, e, ne, e, w,  e,  e, e, ne, n,  e, e,  e,  e, ne, w, n, e, ne, ne, n, n,
             w, nw, e, e, n, ne, n, nw, se, e, s, s,  sw, w, se, se, e, s,  s, s, s, s,  s,  se},
        });

    FlowDirectionArray const flow_direction_we_got = lue::value_policies::d8_flow_direction(elevation);

    // TOO bad, the DEM from the paper contains flat areas. Our operation
    // assumes this is not the case. The results are therefore not the
    // same. This can be fixed in the future, once we are able to handle
    // flat areas.

    BOOST_WARN(false);
    // lue::test::check_arrays_are_equal(flow_direction_we_got, flow_direction_we_want);
}


BOOST_AUTO_TEST_CASE(no_data)
{
    using Elevation = lue::FloatingPointElement<0>;
    using FlowDirection = lue::FlowDirectionElement;
    using Policies = lue::policy::d8_flow_direction::DefaultValuePolicies<FlowDirection, Elevation>;
    std::size_t const rank = 2;

    using ElevationArray = lue::PartitionedArray<Elevation, rank>;
    using FlowDirectionArray = lue::PartitionedArray<FlowDirection, rank>;
    using Shape = lue::ShapeT<ElevationArray>;

    Shape const array_shape{{3, 3}};
    Shape const partition_shape{{3, 3}};

    auto const y{lue::sink<FlowDirection>};

    Policies policies{};
    auto const x{lue::policy::no_data_value<Elevation>};
    auto const u{lue::policy::no_data_value<FlowDirection>};

    {
        auto const elevation = lue::test::create_partitioned_array<ElevationArray>(
            array_shape,
            partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
            {
                x, x, x,
                x, x, x,
                x, x, x,
            },
                // clang-format on
                // NOLINTEND
            });
        auto const flow_direction_we_got = lue::value_policies::d8_flow_direction(elevation);
        auto const flow_direction_we_want = lue::test::create_partitioned_array<FlowDirectionArray>(
            array_shape,
            partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
            {
                u, u, u,
                u, u, u,
                u, u, u,
            },
                // clang-format on
                // NOLINTEND
            });

        lue::test::check_arrays_are_equal(flow_direction_we_got, flow_direction_we_want);
    }

    {
        auto const elevation = lue::test::create_partitioned_array<ElevationArray>(
            array_shape,
            partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    x, x, x,
                    x, 5, x,
                    x, x, x,
                },
                // clang-format on
                // NOLINTEND
            });
        auto const flow_direction_we_got = lue::value_policies::d8_flow_direction(elevation);
        auto const flow_direction_we_want = lue::test::create_partitioned_array<FlowDirectionArray>(
            array_shape,
            partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    u, u, u,
                    u, y, u,
                    u, u, u,
                },
                // clang-format on
                // NOLINTEND
            });

        lue::test::check_arrays_are_equal(flow_direction_we_got, flow_direction_we_want);
    }
}
