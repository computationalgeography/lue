#define BOOST_TEST_MODULE lue framework algorithm accu
#include "flow_accumulation.hpp"
#include "lue/framework/algorithm/value_policies/accu.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


BOOST_AUTO_TEST_CASE(overloads)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    lue::PartitionedArray<FlowDirectionElement, 2> const flow_direction{};
    lue::PartitionedArray<MaterialElement, 2> outflow{};

    lue::PartitionedArray<MaterialElement, 2> const inflow_raster{};
    lue::Scalar<MaterialElement> const inflow_scalar{};
    MaterialElement const inflow_value{};

    // raster
    outflow = lue::value_policies::accu(flow_direction, inflow_raster);

    // scalar
    outflow = lue::value_policies::accu(flow_direction, inflow_scalar);

    // value
    outflow = lue::value_policies::accu(flow_direction, inflow_value);
}


BOOST_AUTO_TEST_CASE(parallel_east)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    auto const flow_direction = lue::create_partitioned_array<FlowDirectionElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, lue::test::e);
    auto const inflow = lue::create_partitioned_array<MaterialElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, 1);

    auto const outflow_we_got = lue::value_policies::accu(flow_direction, inflow);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            lue::test::array_shape,
            lue::test::partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    1, 2, 3,
                    1, 2, 3,
                    1, 2, 3,
                },
                {
                    4, 5, 6,
                    4, 5, 6,
                    4, 5, 6,
                },
                {
                    7, 8, 9,
                    7, 8, 9,
                    7, 8, 9,
                },
                {
                    1, 2, 3,
                    1, 2, 3,
                    1, 2, 3,
                },
                {
                    4, 5, 6,
                    4, 5, 6,
                    4, 5, 6,
                },
                {
                    7, 8, 9,
                    7, 8, 9,
                    7, 8, 9,
                },
                {
                    1, 2, 3,
                    1, 2, 3,
                    1, 2, 3,
                },
                {
                    4, 5, 6,
                    4, 5, 6,
                    4, 5, 6,
                },
                {
                    7, 8, 9,
                    7, 8, 9,
                    7, 8, 9,
                },
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(parallel_south_east)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    auto const flow_direction = lue::create_partitioned_array<FlowDirectionElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, lue::test::se);
    auto const inflow = lue::create_partitioned_array<MaterialElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, 1);

    auto const outflow_we_got = lue::value_policies::accu(flow_direction, inflow);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            lue::test::array_shape,
            lue::test::partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    1, 1, 1,
                    1, 2, 2,
                    1, 2, 3,
                },
                {
                    1, 1, 1,
                    2, 2, 2,
                    3, 3, 3,
                },
                {
                    1, 1, 1,
                    2, 2, 2,
                    3, 3, 3,
                },
                {
                    1, 2, 3,
                    1, 2, 3,
                    1, 2, 3,
                },
                {
                    4, 4, 4,
                    4, 5, 5,
                    4, 5, 6,
                },
                {
                    4, 4, 4,
                    5, 5, 5,
                    6, 6, 6,
                },
                {
                    1, 2, 3,
                    1, 2, 3,
                    1, 2, 3,
                },
                {
                    4, 5, 6,
                    4, 5, 6,
                    4, 5, 6,
                },
                {
                    7, 7, 7,
                    7, 8, 8,
                    7, 8, 9,
                },
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(parallel_south)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    auto const flow_direction = lue::create_partitioned_array<FlowDirectionElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, lue::test::s);
    auto const inflow = lue::create_partitioned_array<MaterialElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, 1);

    auto const outflow_we_got = lue::value_policies::accu(flow_direction, inflow);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            lue::test::array_shape,
            lue::test::partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    1, 1, 1,
                    2, 2, 2,
                    3, 3, 3,
                },
                {
                    1, 1, 1,
                    2, 2, 2,
                    3, 3, 3,
                },
                {
                    1, 1, 1,
                    2, 2, 2,
                    3, 3, 3,
                },
                {
                    4, 4, 4,
                    5, 5, 5,
                    6, 6, 6,
                },
                {
                    4, 4, 4,
                    5, 5, 5,
                    6, 6, 6,
                },
                {
                    4, 4, 4,
                    5, 5, 5,
                    6, 6, 6,
                },
                {
                    7, 7, 7,
                    8, 8, 8,
                    9, 9, 9,
                },
                {
                    7, 7, 7,
                    8, 8, 8,
                    9, 9, 9,
                },
                {
                    7, 7, 7,
                    8, 8, 8,
                    9, 9, 9,
                },
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(parallel_south_west)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    auto const flow_direction = lue::create_partitioned_array<FlowDirectionElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, lue::test::sw);
    auto const inflow = lue::create_partitioned_array<MaterialElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, 1);

    auto const outflow_we_got = lue::value_policies::accu(flow_direction, inflow);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            lue::test::array_shape,
            lue::test::partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    1, 1, 1,
                    2, 2, 2,
                    3, 3, 3,
                },
                {
                    1, 1, 1,
                    2, 2, 2,
                    3, 3, 3,
                },
                {
                    1, 1, 1,
                    2, 2, 1,
                    3, 2, 1,
                },
                {
                    4, 4, 4,
                    5, 5, 5,
                    6, 6, 6,
                },
                {
                    4, 4, 4,
                    5, 5, 4,
                    6, 5, 4,
                },
                {
                    3, 2, 1,
                    3, 2, 1,
                    3, 2, 1,
                },
                {
                    7, 7, 7,
                    8, 8, 7,
                    9, 8, 7,
                },
                {
                    6, 5, 4,
                    6, 5, 4,
                    6, 5, 4,
                },
                {
                    3, 2, 1,
                    3, 2, 1,
                    3, 2, 1,
                },
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(parallel_west)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    auto const flow_direction = lue::create_partitioned_array<FlowDirectionElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, lue::test::w);
    auto const inflow = lue::create_partitioned_array<MaterialElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, 1);

    auto const outflow_we_got = lue::value_policies::accu(flow_direction, inflow);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            lue::test::array_shape,
            lue::test::partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    9, 8, 7,
                    9, 8, 7,
                    9, 8, 7,
                },
                {
                    6, 5, 4,
                    6, 5, 4,
                    6, 5, 4,
                },
                {
                    3, 2, 1,
                    3, 2, 1,
                    3, 2, 1,
                },
                {
                    9, 8, 7,
                    9, 8, 7,
                    9, 8, 7,
                },
                {
                    6, 5, 4,
                    6, 5, 4,
                    6, 5, 4,
                },
                {
                    3, 2, 1,
                    3, 2, 1,
                    3, 2, 1,
                },
                {
                    9, 8, 7,
                    9, 8, 7,
                    9, 8, 7,
                },
                {
                    6, 5, 4,
                    6, 5, 4,
                    6, 5, 4,
                },
                {
                    3, 2, 1,
                    3, 2, 1,
                    3, 2, 1,
                },
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(parallel_north_west)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    auto const flow_direction = lue::create_partitioned_array<FlowDirectionElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, lue::test::nw);
    auto const inflow = lue::create_partitioned_array<MaterialElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, 1);

    auto const outflow_we_got = lue::value_policies::accu(flow_direction, inflow);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            lue::test::array_shape,
            lue::test::partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    9, 8, 7,
                    8, 8, 7,
                    7, 7, 7,
                },
                {
                    6, 5, 4,
                    6, 5, 4,
                    6, 5, 4,
                },
                {
                    3, 2, 1,
                    3, 2, 1,
                    3, 2, 1,
                },
                {
                    6, 6, 6,
                    5, 5, 5,
                    4, 4, 4,
                },
                {
                    6, 5, 4,
                    5, 5, 4,
                    4, 4, 4,
                },
                {
                    3, 2, 1,
                    3, 2, 1,
                    3, 2, 1,
                },
                {
                    3, 3, 3,
                    2, 2, 2,
                    1, 1, 1,
                },
                {
                    3, 3, 3,
                    2, 2, 2,
                    1, 1, 1,
                },
                {
                    3, 2, 1,
                    2, 2, 1,
                    1, 1, 1,
                },
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(parallel_north)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    auto const flow_direction = lue::create_partitioned_array<FlowDirectionElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, lue::test::n);
    auto const inflow = lue::create_partitioned_array<MaterialElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, 1);

    auto const outflow_we_got = lue::value_policies::accu(flow_direction, inflow);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            lue::test::array_shape,
            lue::test::partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    9, 9, 9,
                    8, 8, 8,
                    7, 7, 7,
                },
                {
                    9, 9, 9,
                    8, 8, 8,
                    7, 7, 7,
                },
                {
                    9, 9, 9,
                    8, 8, 8,
                    7, 7, 7,
                },
                {
                    6, 6, 6,
                    5, 5, 5,
                    4, 4, 4,
                },
                {
                    6, 6, 6,
                    5, 5, 5,
                    4, 4, 4,
                },
                {
                    6, 6, 6,
                    5, 5, 5,
                    4, 4, 4,
                },
                {
                    3, 3, 3,
                    2, 2, 2,
                    1, 1, 1,
                },
                {
                    3, 3, 3,
                    2, 2, 2,
                    1, 1, 1,
                },
                {
                    3, 3, 3,
                    2, 2, 2,
                    1, 1, 1,
                },
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(parallel_north_east)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    auto const flow_direction = lue::create_partitioned_array<FlowDirectionElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, lue::test::ne);
    auto const inflow = lue::create_partitioned_array<MaterialElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, 1);

    auto const outflow_we_got = lue::value_policies::accu(flow_direction, inflow);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            lue::test::array_shape,
            lue::test::partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    1, 2, 3,
                    1, 2, 3,
                    1, 2, 3,
                },
                {
                    4, 5, 6,
                    4, 5, 6,
                    4, 5, 6,
                },
                {
                    7, 8, 9,
                    7, 8, 8,
                    7, 7, 7,
                },
                {
                    1, 2, 3,
                    1, 2, 3,
                    1, 2, 3,
                },
                {
                    4, 5, 6,
                    4, 5, 5,
                    4, 4, 4,
                },
                {
                    6, 6, 6,
                    5, 5, 5,
                    4, 4, 4,
                },
                {
                    1, 2, 3,
                    1, 2, 2,
                    1, 1, 1,
                },
                {
                    3, 3, 3,
                    2, 2, 2,
                    1, 1, 1,
                },
                {
                    3, 3, 3,
                    2, 2, 2,
                    1, 1, 1,
                },
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(converge)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    auto const n = lue::test::n;
    auto const ne = lue::test::ne;
    auto const e = lue::test::e;
    auto const se = lue::test::se;
    auto const s = lue::test::s;
    auto const sw = lue::test::sw;
    auto const w = lue::test::w;
    auto const nw = lue::test::nw;
    auto const p = lue::test::p;

    auto const flow_direction =
        lue::test::create_partitioned_array<lue::PartitionedArray<FlowDirectionElement, 2>>(
            lue::test::array_shape,
            lue::test::partition_shape,
            {
                // clang-format off
                {
                    se, se, se,
                    se, se, se,
                    se, se, se,
                },
                {
                    s, s, s,
                    s, s, s,
                    s, s, s,
                },
                {
                    sw, sw, sw,
                    sw, sw, sw,
                    sw, sw, sw,
                },
                {
                    e, e, e,
                    e, e, e,
                    e, e, e,
                },
                {
                    se, s, sw,
                    e, p, w,
                    ne, n, nw,
                },
                {
                    w, w, w,
                    w, w, w,
                    w, w, w,
                },
                {
                    ne, ne, ne,
                    ne, ne, ne,
                    ne, ne, ne,
                },
                {
                    n, n, n,
                    n, n, n,
                    n, n, n,
                },
                {
                    nw, nw, nw,
                    nw, nw, nw,
                    nw, nw, nw,
                },
                // clang-format on
            });
    auto const inflow = lue::create_partitioned_array<MaterialElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, 1);

    auto const outflow_we_got = lue::value_policies::accu(flow_direction, inflow);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            lue::test::array_shape,
            lue::test::partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    1, 1, 1,
                    1, 2, 2,
                    1, 2, 3,
                },
                {
                    1, 1, 1,
                    3, 2, 3,
                    6, 3, 6,
                },
                {
                    1, 1, 1,
                    2, 2, 1,
                    3, 2, 1,
                },
                {
                    1, 3, 6,
                    1, 2, 3,
                    1, 3, 6,
                },
                {
                    16, 4, 16,
                    4, 81, 4,
                    16, 4, 16,
                },
                {
                    6, 3, 1,
                    3, 2, 1,
                    6, 3, 1,
                },
                {
                    1, 2, 3,
                    1, 2, 2,
                    1, 1, 1,
                },
                {
                    6, 3, 6,
                    3, 2, 3,
                    1, 1, 1,
                },
                {
                    3, 2, 1,
                    2, 2, 1,
                    1, 1, 1,
                },
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(diverge)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    auto const n = lue::test::n;
    auto const ne = lue::test::ne;
    auto const e = lue::test::e;
    auto const se = lue::test::se;
    auto const s = lue::test::s;
    auto const sw = lue::test::sw;
    auto const w = lue::test::w;
    auto const nw = lue::test::nw;

    auto const flow_direction =
        lue::test::create_partitioned_array<lue::PartitionedArray<FlowDirectionElement, 2>>(
            lue::test::array_shape,
            lue::test::partition_shape,
            {
                // clang-format off
                {
                    nw, nw, nw,
                    nw, nw, nw,
                    nw, nw, nw,
                },
                {
                    n, n, n,
                    n, n, n,
                    n, n, n,
                },
                {
                    ne, ne, ne,
                    ne, ne, ne,
                    ne, ne, ne,
                },
                {
                    w, w, w,
                    w, w, w,
                    w, w, w,
                },
                {
                    nw, n, ne,
                    w, s, e,
                    sw, s, se,
                },
                {
                    e, e, e,
                    e, e, e,
                    e, e, e,
                },
                {
                    sw, sw, sw,
                    sw, sw, sw,
                    sw, sw, sw,
                },
                {
                    s, s, s,
                    s, s, s,
                    s, s, s,
                },
                {
                    se, se, se,
                    se, se, se,
                    se, se, se,
                },
                // clang-format on
            });
    auto const inflow = lue::create_partitioned_array<MaterialElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, 1);

    auto const outflow_we_got = lue::value_policies::accu(flow_direction, inflow);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            lue::test::array_shape,
            lue::test::partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    4, 2, 1,
                    2, 3, 1,
                    1, 1, 2,
                },
                {
                    3, 4, 3,
                    2, 3, 2,
                    1, 2, 1,
                },
                {
                    1, 2, 4,
                    1, 3, 2,
                    2, 1, 1,
                },
                {
                    3, 2, 1,
                    4, 3, 2,
                    3, 2, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 2, 1,
                },
                {
                    1, 2, 3,
                    2, 3, 4,
                    1, 2, 3,
                },
                {
                    1, 1, 2,
                    2, 3, 1,
                    4, 2, 1,
                },
                {
                    1, 3, 1,
                    2, 4, 2,
                    3, 5, 3,
                },
                {
                    2, 1, 1,
                    1, 3, 2,
                    1, 2, 4,
                },
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(spiral_in_case)
{
    using MaterialElement = lue::FloatingPointElement<0>;

    auto const flow_direction = lue::test::spiral_in();
    auto const inflow = lue::test::ones<MaterialElement>();

    auto const outflow_we_got = lue::value_policies::accu(flow_direction, inflow);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            lue::test::array_shape,
            lue::test::partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    1, 2, 3,
                    32, 33, 34,
                    31, 56, 57,
                },
                {
                    4, 5, 6,
                    35, 36, 37,
                    58, 59, 60,
                },
                {
                    7, 8, 9,
                    38, 39, 10,
                    61, 40, 11,
                },
                {
                    30, 55, 72,
                    29, 54, 71,
                    28, 53, 70,
                },
                {
                    73, 74, 75,
                    80, 81, 76,
                    79, 78, 77,
                },
                {
                    62, 41, 12,
                    63, 42, 13,
                    64, 43, 14,
                },
                {
                    27, 52, 69,
                    26, 51, 50,
                    25, 24, 23,
                },
                {
                    68, 67, 66,
                    49, 48, 47,
                    22, 21, 20,
                },
                {
                    65, 44, 15,
                    46, 45, 16,
                    19, 18, 17,
                },
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(spiral_out)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    auto const n = lue::test::n;
    auto const e = lue::test::e;
    auto const s = lue::test::s;
    auto const w = lue::test::w;

    auto const flow_direction =
        lue::test::create_partitioned_array<lue::PartitionedArray<FlowDirectionElement, 2>>(
            lue::test::array_shape,
            lue::test::partition_shape,
            {
                // clang-format off
                {
                    w, w, w,
                    s, w, w,
                    s, s, w,
                },
                {
                    w, w, w,
                    w, w, w,
                    w, w, w,
                },
                {
                    w, w, w,
                    w, w, n,
                    w, n, n,
                },
                {
                    s, s, s,
                    s, s, s,
                    s, s, s,
                },
                {
                    w, w, w,
                    s, w, n,
                    e, e, n,
                },
                {
                    n, n, n,
                    n, n, n,
                    n, n, n,
                },
                {
                    s, s, e,
                    s, e, e,
                    e, e, e,
                },
                {
                    e, e, e,
                    e, e, e,
                    e, e, e,
                },
                {
                    n, n, n,
                    e, n, n,
                    e, e, n,
                },
                // clang-format on
            });
    auto const inflow = lue::create_partitioned_array<MaterialElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, 1);

    auto const outflow_we_got = lue::value_policies::accu(flow_direction, inflow);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            lue::test::array_shape,
            lue::test::partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    81, 80, 79,
                    50, 49, 48,
                    51, 26, 25,
                },
                {
                    78, 77, 76,
                    47, 46, 45,
                    24, 23, 22,
                },
                {
                    75, 74, 73,
                    44, 43, 72,
                    21, 42, 71,
                },
                {
                    52, 27, 10,
                    53, 28, 11,
                    54, 29, 12,
                },
                {
                    9, 8, 7,
                    2, 1, 6,
                    3, 4, 5,
                },
                {
                    20, 41, 70,
                    19, 40, 69,
                    18, 39, 68,
                },
                {
                    55, 30, 13,
                    56, 31, 32,
                    57, 58, 59,
                },
                {
                    14, 15, 16,
                    33, 34, 35,
                    60, 61, 62,
                },
                {
                    17, 38, 67,
                    36, 37, 66,
                    63, 64, 65,
                },
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(all_no_data_flow_direction)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    auto const flow_direction = lue::test::no_data<FlowDirectionElement>();
    auto const inflow = lue::test::ones<MaterialElement>();

    auto const outflow_we_got = lue::value_policies::accu(flow_direction, inflow);

    auto const outflow_we_want = lue::test::filled(lue::policy::no_data_value<MaterialElement>);

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(all_no_data_material)
{
    using MaterialElement = lue::FloatingPointElement<0>;

    auto const e = lue::test::e;

    auto const flow_direction = lue::test::filled(e);
    auto const inflow = lue::test::no_data<MaterialElement>();

    auto const outflow_we_got = lue::value_policies::accu(flow_direction, inflow);

    auto const outflow_we_want = lue::test::filled(lue::policy::no_data_value<MaterialElement>);

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(merging_streams_case_01)
{
    // No-data material in ridge cell. From there on, it must propagate
    // down to all downstream cells, in all downstream partitions.

    using MaterialElement = lue::FloatingPointElement<0>;

    auto const flow_direction = lue::test::merging_streams();

    auto const x{lue::policy::no_data_value<MaterialElement>};

    auto const inflow = lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
        lue::test::array_shape,
        lue::test::partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                // 0, 0
                1, 1, 1,
                1, 1, 1,
                1, 1, 1,
            },
            {
                // 0, 1
                1, 1, 1,
                1, 1, 1,
                1, 1, 1,
            },
            {
                // 0, 2
                1, x, 1,
                1, 1, 1,
                1, 1, 1,
            },
            {
                // 1, 0
                1, 1, 1,
                1, 1, 1,
                1, 1, 1,
            },
            {
                // 1, 1
                1, 1, 1,
                1, 1, 1,
                1, 1, 1,
            },
            {
                // 1, 2
                1, 1, 1,
                1, 1, 1,
                1, 1, 1,
            },
            {
                // 2, 0
                1, 1, 1,
                1, 1, 1,
                1, 1, 1,
            },
            {
                // 2, 1
                1, 1, 1,
                1, 1, 1,
                1, 1, 1,
            },
            {
                // 2, 2
                1, 1, 1,
                1, 1, 1,
                1, 1, 1,
            },
            // clang-format on
            // NOLINTEND
        });

    auto const outflow_we_got = lue::value_policies::accu(flow_direction, inflow);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            lue::test::array_shape,
            lue::test::partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    // 0, 0
                    x, x, x,
                    x, x, x,
                    x, x, 1,
                },
                {
                    // 0, 1
                    x, 1, 1,
                    x, 1, 1,
                    x, 2, x,
                },
                {
                    // 0, 2
                    x, x, 1,
                    3, x, 1,
                    x, x, 1,
                },
                {
                    // 1, 0
                    x, x, 1,
                    x, x, 1,
                    x, x, 1,
                },
                {
                    // 1, 1
                    4, x, x,
                    x, x, 2,
                    2, x, 3,
                },
                {
                    // 1, 2
                    x, x, x,
                    1, x, x,
                    1, x, x,
                },
                {
                    // 2, 0
                    x, x, x,
                    x, x, x,
                    x, x, x,
                },
                {
                    // 2, 1
                    x, x, x,
                    x, x, 2,
                    4, 3, 2,
                },
                {
                    // 2, 2
                    1, x, x,
                    1, x, x,
                    1, x, x,
                },
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(back_and_forth)
{
    // Stream which runs from one partition to another and back and back
    // and back ...

    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    lue::ShapeT<lue::test::FlowDirectionArray> const array_shape{{6, 5}};
    lue::ShapeT<lue::test::FlowDirectionArray> const partition_shape{{3, 5}};

    auto const n = lue::test::n;
    auto const e = lue::test::e;
    auto const s = lue::test::s;
    auto const p = lue::test::p;

    auto const flow_direction =
        lue::test::create_partitioned_array<lue::PartitionedArray<FlowDirectionElement, 2>>(
            array_shape,
            partition_shape,
            {
                // clang-format off
                {
                    s, e, s, e, s,
                    s, n, s, n, s,
                    s, n, s, n, s,
                },
                {
                    s, n, s, n, s,
                    s, n, s, n, s,
                    e, n, e, n, p,
                },
                // clang-format on
            });
    MaterialElement const inflow = 1;

    auto outflow_we_got = lue::value_policies::accu(flow_direction, inflow);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            array_shape,
            partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    1, 12, 13, 24, 25,
                    2, 11, 14, 23, 26,
                    3, 10, 15, 22, 27,
                },
                {
                    4, 9, 16, 21, 28,
                    5, 8, 17, 20, 29,
                    6, 7, 18, 19, 30,
                },
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
}


// TODO: Out of domain values: negative material is out of domain!
