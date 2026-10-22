#define BOOST_TEST_MODULE lue framework algorithm inflow_count
#include "flow_accumulation.hpp"
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/algorithm/value_policies/inflow_count.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"

namespace {

    using lue::FlowDirectionElement;

    // TODO: This small count should be named differently (LUE_SMALL_COUNT, ...)
    using CountElement = lue::BooleanElement;

    std::size_t const rank = 2;

    using FlowDirection = lue::PartitionedArray<FlowDirectionElement, rank>;
    using InflowCount = lue::PartitionedArray<CountElement, rank>;
    using Shape = lue::ShapeT<FlowDirection>;

    Shape const array_shape{{3, 3}};
    Shape const partition_shape{{3, 3}};

    auto const n{lue::north<FlowDirectionElement>};
    auto const ne{lue::north_east<FlowDirectionElement>};
    auto const e{lue::east<FlowDirectionElement>};
    auto const se{lue::south_east<FlowDirectionElement>};
    auto const s{lue::south<FlowDirectionElement>};
    auto const sw{lue::south_west<FlowDirectionElement>};
    auto const w{lue::west<FlowDirectionElement>};
    auto const nw{lue::north_west<FlowDirectionElement>};
    auto const p{lue::sink<FlowDirectionElement>};
    auto const nd{lue::policy::no_data_value<FlowDirectionElement>};


    template<typename FlowDirectionArray, typename InflowCountArray>
    void test_inflow_count(
        FlowDirectionArray const& flow_direction, InflowCountArray const& inflow_count_we_want)
    {
        using CountElement = lue::ElementT<InflowCountArray>;

        InflowCount inflow_count_we_got = lue::value_policies::inflow_count<CountElement>(flow_direction);

        lue::test::check_arrays_are_equal(inflow_count_we_got, inflow_count_we_want);
    }

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(pit)
{
    // +---+---+---+   +---+---+---+
    // | 🡦 | 🡣 | 🡧 |   | 0 | 0 | 0 |
    // +---+---+---+   +---+---+---+
    // | 🡢 | ■ | 🡠 | → | 0 | 8 | 0 |
    // +---+---+---+   +---+---+---+
    // | 🡥 | 🡡 | 🡤 |   | 0 | 0 | 0 |
    // +---+---+---+   +---+---+---+
    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(
            array_shape,
            partition_shape,
            // clang-format off
            {
                {
                    se, s, sw,
                     e, p,  w,
                    ne, n, nw,
                },
            }  // clang-format on
            ),
        lue::test::create_partitioned_array<InflowCount>(
            array_shape,
            partition_shape,
            // clang-format off
            {
                {
                    0, 0, 0,
                    0, 8, 0,
                    0, 0, 0,
                },
            }  // clang-format on
            ));
}


BOOST_AUTO_TEST_CASE(peak)
{
    // +---+---+---+   +---+---+---+
    // | 🡤 | 🡡 | 🡥 |   | 0 | 0 | 0 |
    // +---+---+---+   +---+---+---+
    // | 🡠 | 🡣 | 🡢 | → | 0 | 0 | 0 |
    // +---+---+---+   +---+---+---+
    // | 🡧 | 🡣 | 🡦 |   | 0 | 1 | 0 |
    // +---+---+---+   +---+---+---+
    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(
            array_shape,
            partition_shape,
            // clang-format off
            {
                {
                    nw, n, ne,
                     w, s,  e,
                    sw, s, se,
                },
            }
        ),
        lue::test::create_partitioned_array<InflowCount>(
            array_shape,
            partition_shape,
            // clang-format off
            {
                {
                    0, 0, 0,
                    0, 0, 0,
                    0, 1, 0,
                },
            }  // clang-format on
            ));
}


BOOST_AUTO_TEST_CASE(converge)
{
    // +---+---+---+   +---+---+---+
    // | 🡦 | 🡣 | 🡧 |   | 0 | 0 | 0 |
    // +---+---+---+   +---+---+---+
    // | 🡢 | 🡣 | 🡠 | → | 0 | 5 | 0 |
    // +---+---+---+   +---+---+---+
    // | 🡢 | 🡣 | 🡠 |   | 0 | 3 | 0 |
    // +---+---+---+   +---+---+---+
    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(
            array_shape,
            partition_shape,
            // clang-format off
            {
                {
                    se, s, sw,
                     e, s, w,
                     e, s, w,
                },
            }  // clang-format on
            ),
        lue::test::create_partitioned_array<InflowCount>(
            array_shape,
            partition_shape,
            // clang-format off
            {
                {
                    0, 0, 0,
                    0, 5, 0,
                    0, 3, 0,
                },
            }  // clang-format on
            ));
}


BOOST_AUTO_TEST_CASE(parallel_south)
{
    // +---+---+---+   +---+---+---+
    // | 🡣 | 🡣 | 🡣 |   | 0 | 0 | 0 |
    // +---+---+---+   +---+---+---+
    // | 🡣 | 🡣 | 🡣 | → | 1 | 1 | 1 |
    // +---+---+---+   +---+---+---+
    // | 🡣 | 🡣 | 🡣 |   | 1 | 1 | 1 |
    // +---+---+---+   +---+---+---+

    Shape const array_shape{{9, 9}};
    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(
            array_shape,
            partition_shape,
            // clang-format off
            {
                {
                    s, s, s,
                    s, s, s,
                    s, s, s,
                },
                {
                    s, s, s,
                    s, s, s,
                    s, s, s,
                },
                {
                    s, s, s,
                    s, s, s,
                    s, s, s,
                },
                {
                    s, s, s,
                    s, s, s,
                    s, s, s,
                },
                {
                    s, s, s,
                    s, s, s,
                    s, s, s,
                },
                {
                    s, s, s,
                    s, s, s,
                    s, s, s,
                },
                {
                    s, s, s,
                    s, s, s,
                    s, s, s,
                },
                {
                    s, s, s,
                    s, s, s,
                    s, s, s,
                },
                {
                    s, s, s,
                    s, s, s,
                    s, s, s,
                },
            }  // clang-format on
            ),
        lue::test::create_partitioned_array<InflowCount>(
            array_shape,
            partition_shape,
            // clang-format off
            {
                {
                    0, 0, 0,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    0, 0, 0,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    0, 0, 0,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
            }  // clang-format on
            ));
}


BOOST_AUTO_TEST_CASE(parallel_south_west)
{
    // +---+---+---+   +---+---+---+
    // | 🡧 | 🡧 | 🡧 |   | 0 | 0 | 0 |
    // +---+---+---+   +---+---+---+
    // | 🡧 | 🡧 | 🡧 | → | 1 | 1 | 0 |
    // +---+---+---+   +---+---+---+
    // | 🡧 | 🡧 | 🡧 |   | 1 | 1 | 0 |
    // +---+---+---+   +---+---+---+

    Shape const array_shape{{9, 9}};
    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(
            array_shape,
            partition_shape,
            // clang-format off
            {
                {
                    sw, sw, sw,
                    sw, sw, sw,
                    sw, sw, sw,
                },
                {
                    sw, sw, sw,
                    sw, sw, sw,
                    sw, sw, sw,
                },
                {
                    sw, sw, sw,
                    sw, sw, sw,
                    sw, sw, sw,
                },
                {
                    sw, sw, sw,
                    sw, sw, sw,
                    sw, sw, sw,
                },
                {
                    sw, sw, sw,
                    sw, sw, sw,
                    sw, sw, sw,
                },
                {
                    sw, sw, sw,
                    sw, sw, sw,
                    sw, sw, sw,
                },
                {
                    sw, sw, sw,
                    sw, sw, sw,
                    sw, sw, sw,
                },
                {
                    sw, sw, sw,
                    sw, sw, sw,
                    sw, sw, sw,
                },
                {
                    sw, sw, sw,
                    sw, sw, sw,
                    sw, sw, sw,
                },
            }  // clang-format on
            ),
        lue::test::create_partitioned_array<InflowCount>(
            array_shape,
            partition_shape,
            // clang-format off
            {
                {
                    0, 0, 0,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    0, 0, 0,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    0, 0, 0,
                    1, 1, 0,
                    1, 1, 0,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 0,
                    1, 1, 0,
                    1, 1, 0,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 0,
                    1, 1, 0,
                    1, 1, 0,
                },
            }  // clang-format on
            ));
}


BOOST_AUTO_TEST_CASE(parallel_west)
{
    // +---+---+---+   +---+---+---+
    // | 🡠 | 🡠 | 🡠 |   | 1 | 1 | 0 |
    // +---+---+---+   +---+---+---+
    // | 🡠 | 🡠 | 🡠 | → | 1 | 1 | 0 |
    // +---+---+---+   +---+---+---+
    // | 🡠 | 🡠 | 🡠 |   | 1 | 1 | 0 |
    // +---+---+---+   +---+---+---+

    Shape const array_shape{{9, 9}};
    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(
            array_shape,
            partition_shape,
            // clang-format off
            {
                {
                    w, w, w,
                    w, w, w,
                    w, w, w,
                },
                {
                    w, w, w,
                    w, w, w,
                    w, w, w,
                },
                {
                    w, w, w,
                    w, w, w,
                    w, w, w,
                },
                {
                    w, w, w,
                    w, w, w,
                    w, w, w,
                },
                {
                    w, w, w,
                    w, w, w,
                    w, w, w,
                },
                {
                    w, w, w,
                    w, w, w,
                    w, w, w,
                },
                {
                    w, w, w,
                    w, w, w,
                    w, w, w,
                },
                {
                    w, w, w,
                    w, w, w,
                    w, w, w,
                },
                {
                    w, w, w,
                    w, w, w,
                    w, w, w,
                },
            }  // clang-format on
            ),
        lue::test::create_partitioned_array<InflowCount>(
            array_shape,
            partition_shape,
            // clang-format off
            {
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 0,
                    1, 1, 0,
                    1, 1, 0,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 0,
                    1, 1, 0,
                    1, 1, 0,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 0,
                    1, 1, 0,
                    1, 1, 0,
                },
            }  // clang-format on
            ));
}


BOOST_AUTO_TEST_CASE(parallel_north_west)
{
    // +---+---+---+   +---+---+---+
    // | 🡤 | 🡤 | 🡤 |   | 1 | 1 | 0 |
    // +---+---+---+   +---+---+---+
    // | 🡤 | 🡤 | 🡤 | → | 1 | 1 | 0 |
    // +---+---+---+   +---+---+---+
    // | 🡤 | 🡤 | 🡤 |   | 0 | 0 | 0 |
    // +---+---+---+   +---+---+---+

    Shape const array_shape{{9, 9}};
    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(
            array_shape,
            partition_shape,
            // clang-format off
            {
                {
                    nw, nw, nw,
                    nw, nw, nw,
                    nw, nw, nw,
                },
                {
                    nw, nw, nw,
                    nw, nw, nw,
                    nw, nw, nw,
                },
                {
                    nw, nw, nw,
                    nw, nw, nw,
                    nw, nw, nw,
                },
                {
                    nw, nw, nw,
                    nw, nw, nw,
                    nw, nw, nw,
                },
                {
                    nw, nw, nw,
                    nw, nw, nw,
                    nw, nw, nw,
                },
                {
                    nw, nw, nw,
                    nw, nw, nw,
                    nw, nw, nw,
                },
                {
                    nw, nw, nw,
                    nw, nw, nw,
                    nw, nw, nw,
                },
                {
                    nw, nw, nw,
                    nw, nw, nw,
                    nw, nw, nw,
                },
                {
                    nw, nw, nw,
                    nw, nw, nw,
                    nw, nw, nw,
                },
            }  // clang-format on
            ),
        lue::test::create_partitioned_array<InflowCount>(
            array_shape,
            partition_shape,
            // clang-format off
            {
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 0,
                    1, 1, 0,
                    1, 1, 0,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 0,
                    1, 1, 0,
                    1, 1, 0,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    0, 0, 0,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    0, 0, 0,
                },
                {
                    1, 1, 0,
                    1, 1, 0,
                    0, 0, 0,
                },
            }  // clang-format on
            ));
}


BOOST_AUTO_TEST_CASE(parallel_north)
{
    // +---+---+---+   +---+---+---+
    // | 🡡 | 🡡 | 🡡 |   | 1 | 1 | 1 |
    // +---+---+---+   +---+---+---+
    // | 🡡 | 🡡 | 🡡 | → | 1 | 1 | 1 |
    // +---+---+---+   +---+---+---+
    // | 🡡 | 🡡 | 🡡 |   | 0 | 0 | 0 |
    // +---+---+---+   +---+---+---+

    Shape const array_shape{{9, 9}};
    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(
            array_shape,
            partition_shape,
            // clang-format off
            {
                {
                    n, n, n,
                    n, n, n,
                    n, n, n,
                },
                {
                    n, n, n,
                    n, n, n,
                    n, n, n,
                },
                {
                    n, n, n,
                    n, n, n,
                    n, n, n,
                },
                {
                    n, n, n,
                    n, n, n,
                    n, n, n,
                },
                {
                    n, n, n,
                    n, n, n,
                    n, n, n,
                },
                {
                    n, n, n,
                    n, n, n,
                    n, n, n,
                },
                {
                    n, n, n,
                    n, n, n,
                    n, n, n,
                },
                {
                    n, n, n,
                    n, n, n,
                    n, n, n,
                },
                {
                    n, n, n,
                    n, n, n,
                    n, n, n,
                },
            }  // clang-format on
            ),
        lue::test::create_partitioned_array<InflowCount>(
            array_shape,
            partition_shape,
            // clang-format off
            {
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    0, 0, 0,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    0, 0, 0,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    0, 0, 0,
                },
            }  // clang-format on
            ));
}


BOOST_AUTO_TEST_CASE(parallel_north_east)
{
    // +---+---+---+   +---+---+---+
    // | 🡥 | 🡥 | 🡥 |   | 0 | 1 | 1 |
    // +---+---+---+   +---+---+---+
    // | 🡥 | 🡥 | 🡥 | → | 0 | 1 | 1 |
    // +---+---+---+   +---+---+---+
    // | 🡥 | 🡥 | 🡥 |   | 0 | 0 | 0 |
    // +---+---+---+   +---+---+---+

    Shape const array_shape{{9, 9}};
    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(
            array_shape,
            partition_shape,
            // clang-format off
            {
                {
                    ne, ne, ne,
                    ne, ne, ne,
                    ne, ne, ne,
                },
                {
                    ne, ne, ne,
                    ne, ne, ne,
                    ne, ne, ne,
                },
                {
                    ne, ne, ne,
                    ne, ne, ne,
                    ne, ne, ne,
                },
                {
                    ne, ne, ne,
                    ne, ne, ne,
                    ne, ne, ne,
                },
                {
                    ne, ne, ne,
                    ne, ne, ne,
                    ne, ne, ne,
                },
                {
                    ne, ne, ne,
                    ne, ne, ne,
                    ne, ne, ne,
                },
                {
                    ne, ne, ne,
                    ne, ne, ne,
                    ne, ne, ne,
                },
                {
                    ne, ne, ne,
                    ne, ne, ne,
                    ne, ne, ne,
                },
                {
                    ne, ne, ne,
                    ne, ne, ne,
                    ne, ne, ne,
                },
            }  // clang-format on
            ),
        lue::test::create_partitioned_array<InflowCount>(
            array_shape,
            partition_shape,
            // clang-format off
            {
                {
                    0, 1, 1,
                    0, 1, 1,
                    0, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    0, 1, 1,
                    0, 1, 1,
                    0, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    0, 1, 1,
                    0, 1, 1,
                    0, 0, 0,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    0, 0, 0,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    0, 0, 0,
                },
            }  // clang-format on
            ));
}


BOOST_AUTO_TEST_CASE(parallel_east)
{
    // +---+---+---+   +---+---+---+
    // | 🡢 | 🡢 | 🡢 |   | 0 | 1 | 1 |
    // +---+---+---+   +---+---+---+
    // | 🡢 | 🡢 | 🡢 | → | 0 | 1 | 1 |
    // +---+---+---+   +---+---+---+
    // | 🡢 | 🡢 | 🡢 |   | 0 | 1 | 1 |
    // +---+---+---+   +---+---+---+

    Shape const array_shape{{9, 9}};
    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(
            array_shape,
            partition_shape,
            // clang-format off
            {
                {
                    e, e, e,
                    e, e, e,
                    e, e, e,
                },
                {
                    e, e, e,
                    e, e, e,
                    e, e, e,
                },
                {
                    e, e, e,
                    e, e, e,
                    e, e, e,
                },
                {
                    e, e, e,
                    e, e, e,
                    e, e, e,
                },
                {
                    e, e, e,
                    e, e, e,
                    e, e, e,
                },
                {
                    e, e, e,
                    e, e, e,
                    e, e, e,
                },
                {
                    e, e, e,
                    e, e, e,
                    e, e, e,
                },
                {
                    e, e, e,
                    e, e, e,
                    e, e, e,
                },
                {
                    e, e, e,
                    e, e, e,
                    e, e, e,
                },
            }  // clang-format on
            ),
        lue::test::create_partitioned_array<InflowCount>(
            array_shape,
            partition_shape,
            // clang-format off
            {
                {
                    0, 1, 1,
                    0, 1, 1,
                    0, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    0, 1, 1,
                    0, 1, 1,
                    0, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    0, 1, 1,
                    0, 1, 1,
                    0, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
            }  // clang-format on
            ));
}


BOOST_AUTO_TEST_CASE(parallel_south_east)
{
    // +---+---+---+   +---+---+---+
    // | 🡦 | 🡦 | 🡦 |   | 0 | 0 | 0 |
    // +---+---+---+   +---+---+---+
    // | 🡦 | 🡦 | 🡦 | → | 0 | 1 | 1 |
    // +---+---+---+   +---+---+---+
    // | 🡦 | 🡦 | 🡦 |   | 0 | 1 | 1 |
    // +---+---+---+   +---+---+---+

    Shape const array_shape{{9, 9}};
    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(
            array_shape,
            partition_shape,
            // clang-format off
            {
                {
                    se, se, se,
                    se, se, se,
                    se, se, se,
                },
                {
                    se, se, se,
                    se, se, se,
                    se, se, se,
                },
                {
                    se, se, se,
                    se, se, se,
                    se, se, se,
                },
                {
                    se, se, se,
                    se, se, se,
                    se, se, se,
                },
                {
                    se, se, se,
                    se, se, se,
                    se, se, se,
                },
                {
                    se, se, se,
                    se, se, se,
                    se, se, se,
                },
                {
                    se, se, se,
                    se, se, se,
                    se, se, se,
                },
                {
                    se, se, se,
                    se, se, se,
                    se, se, se,
                },
                {
                    se, se, se,
                    se, se, se,
                    se, se, se,
                },
            }  // clang-format on
            ),
        lue::test::create_partitioned_array<InflowCount>(
            array_shape,
            partition_shape,
            // clang-format off
            {
                {
                    0, 0, 0,
                    0, 1, 1,
                    0, 1, 1,
                },
                {
                    0, 0, 0,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    0, 0, 0,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    0, 1, 1,
                    0, 1, 1,
                    0, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    0, 1, 1,
                    0, 1, 1,
                    0, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
            }  // clang-format on
            ));
}


BOOST_AUTO_TEST_CASE(partitioned)
{
    Shape const array_shape{{9, 9}};

    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(
            array_shape,
            partition_shape,
            // clang-format off
            {
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
                    p, p, p,
                    p, p, p,
                    p, p, p,
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
            }  // clang-format on
            ),
        lue::test::create_partitioned_array<InflowCount>(
            array_shape,
            partition_shape,
            // clang-format off
            {
                {
                    1, 1, 0,
                    1, 1, 0,
                    0, 0, 0,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    0, 0, 0,
                },
                {
                    0, 1, 1,
                    0, 1, 1,
                    0, 0, 0,
                },
                {
                    1, 1, 0,
                    1, 1, 0,
                    1, 1, 0,
                },
                {
                    0, 0, 0,
                    0, 0, 0,
                    0, 0, 0,
                },
                {
                    0, 1, 1,
                    0, 1, 1,
                    0, 1, 1,
                },
                {
                    0, 0, 0,
                    1, 1, 0,
                    1, 1, 0,
                },
                {
                    0, 0, 0,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    0, 0, 0,
                    0, 1, 1,
                    0, 1, 1,
                },
            }  // clang-format on
            ));
}


BOOST_AUTO_TEST_CASE(all_no_data)
{
    // +---+---+---+   +---+---+---+
    // | X | X | X |   | X | X | X |
    // +---+---+---+   +---+---+---+
    // | X | X | X |   | X | X | X |
    // +---+---+---+   +---+---+---+
    // | X | X | X |   | X | X | X |
    // +---+---+---+   +---+---+---+

    auto const flow_direction = lue::test::create_partitioned_array<FlowDirection>(
        array_shape,
        partition_shape,
        // clang-format off
        {
            {
                nd, nd, nd,
                nd, nd, nd,
                nd, nd, nd,
            },
        }  // clang-format on
    );
    auto const x{lue::policy::no_data_value<CountElement>};
    auto const inflow_count_we_want = lue::test::create_partitioned_array<InflowCount>(
        array_shape,
        partition_shape,
        // clang-format off
        {
            {
                x, x, x,
                x, x, x,
                x, x, x,
            },
        }  // clang-format on
    );

    test_inflow_count(flow_direction, inflow_count_we_want);
}


BOOST_AUTO_TEST_CASE(no_data)
{
    Shape const array_shape{{3, 6}};

    auto const flow_direction = lue::test::create_partitioned_array<FlowDirection>(
        array_shape,
        partition_shape,
        // clang-format off
        {
            {
                e, s, nd,
                e, s, nd,
                e, s, nd,
            },
            {
                e, e, e,
                e, e, e,
                e, e, e,
            },
        }  // clang-format on
    );
    auto const x{lue::policy::no_data_value<CountElement>};
    auto const inflow_count_we_want = lue::test::create_partitioned_array<InflowCount>(
        array_shape,
        partition_shape,
        // clang-format off
        {
            {
                0, 1, x,
                0, 2, x,
                0, 2, x,
            },
            {
                0, 1, 1,
                0, 1, 1,
                0, 1, 1,
            },
        }  // clang-format on
    );

    test_inflow_count(flow_direction, inflow_count_we_want);
}


BOOST_AUTO_TEST_CASE(merging_inter_partition_streams)
{
    Shape const array_shape{{9, 9}};

    auto const flow_direction = lue::test::create_partitioned_array<FlowDirection>(
        array_shape,
        partition_shape,
        // clang-format off
        {
            {
                // 0, 0
                nd, nd, nd,
                nd, nd, nd,
                nd, nd, se,
            },
            {
                // 0, 1
                nd, nw, se,
                nd, s,  e,
                nd, s,  nd,
            },
            {
                // 0, 2
                nd, s, sw,
                se, s, w,
                nd, s, sw,
            },
            {
                // 1, 0
                nd, nd, e,
                nd, nd, ne,
                nd, nd, e,
            },
            {
                // 1, 1
                e,  s, w,
                nd, s, w,
                e,  s, w,
            },
            {
                // 1, 2
                w, w,  nd,
                w, nd, nd,
                w, nd, nd,
            },
            {
                // 2, 0
                nd, nd, nd,
                nd, nd, nd,
                nd, nd, s,
            },
            {
                // 2, 1
                nd, s, nd,
                sw, w, w,
                w,  w, w,
            },
            {
                // 2, 2
                nw, nd, nd,
                w,  nd, nd,
                w,  nd, nd,
            },
        }  // clang-format on
    );

    auto const x{lue::policy::no_data_value<CountElement>};
    auto const inflow_count_we_want = lue::test::create_partitioned_array<InflowCount>(
        array_shape,
        partition_shape,
        // clang-format off
        {
            {
                x, x, x,
                x, x, x,
                x, x, 0,
            },
            {
                x, 0, 0,
                x, 0, 0,
                x, 1, x,
            },
            {
                x, 0, 0,
                2, 3, 0,
                x, 2, 0,
            },
            {
                x, x, 0,
                x, x, 0,
                x, x, 0,
            },
            {
                3, 3, 1,
                x, 2, 1,
                1, 3, 2,
            },
            {
                1, 2, x,
                0, x, x,
                0, x, x,
            },
            {
                x, x, x,
                x, x, x,
                x, x, 2,
            },
            {
                x, 1, x,
                1, 2, 1,
                1, 1, 1,
            },
            {
                0, x, x,
                0, x, x,
                0, x, x,
            },
        }  // clang-format on
    );

    test_inflow_count(flow_direction, inflow_count_we_want);
}


BOOST_AUTO_TEST_CASE(spiral_in_case)
{
    Shape const array_shape{{9, 9}};
    Shape const partition_shape{{3, 3}};

    test_inflow_count(
        lue::test::spiral_in(),
        lue::test::create_partitioned_array<InflowCount>(
            array_shape,
            partition_shape,
            // clang-format off
            {
                {
                    0, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
                {
                    1, 1, 1,
                    1, 1, 1,
                    1, 1, 1,
                },
            }  // clang-format on
            ));
}
