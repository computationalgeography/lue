#define BOOST_TEST_MODULE lue framework algorithm inflow_count
#include "lue/framework/algorithm/inflow_count.hpp"
#include "lue/framework/algorithm/policy.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace {

using FlowDirectionElement = std::uint32_t;
using CountElement = std::int32_t;
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
auto const nd{lue::no_data<FlowDirectionElement>};


template<
    typename Policies,
    typename FlowDirectionArray,
    typename InflowCountArray>
void test_inflow_count(
    Policies const& policies,
    FlowDirectionArray const& flow_direction,
    InflowCountArray const& inflow_count_we_want)
{
    using CountElement = lue::ElementT<InflowCountArray>;

    InflowCount inflow_count_we_got = lue::inflow_count<Policies, CountElement>(policies, flow_direction);

    lue::test::check_arrays_are_equal(inflow_count_we_got, inflow_count_we_want);
}


template<
    typename FlowDirectionArray,
    typename InflowCountArray>
void test_inflow_count(
    FlowDirectionArray const& flow_direction,
    InflowCountArray const& inflow_count_we_want)
{
    using CountElement = lue::ElementT<InflowCountArray>;
    using FlowDirectionElement = lue::ElementT<FlowDirectionArray>;
    using Policies = lue::policy::inflow_count::DefaultPolicies<CountElement, FlowDirectionElement>;

    test_inflow_count(Policies{}, flow_direction, inflow_count_we_want);
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
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
            {{
                se,  s, sw,
                 e,  p,  w,
                ne,  n, nw,
            }}),
        lue::test::create_partitioned_array<InflowCount>(array_shape, partition_shape,
            {{
                0, 0, 0,
                0, 8, 0,
                0, 0, 0,
            }}));
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
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
            {{
                nw,  n, ne,
                 w,  s,  e,
                sw,  s, se,
            }}),
        lue::test::create_partitioned_array<InflowCount>(array_shape, partition_shape,
            {{
                0, 0, 0,
                0, 0, 0,
                0, 1, 0,
            }}));
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
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
            {{
                se,  s, sw,
                 e,  s,  w,
                 e,  s,  w,
            }}),
        lue::test::create_partitioned_array<InflowCount>(array_shape, partition_shape,
            {{
                0, 0, 0,
                0, 5, 0,
                0, 3, 0,
            }}));
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
    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
            {{
                s, s, s,
                s, s, s,
                s, s, s,
            }}),
        lue::test::create_partitioned_array<InflowCount>(array_shape, partition_shape,
            {{
                0, 0, 0,
                1, 1, 1,
                1, 1, 1,
            }}));
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
    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
            {{
                sw, sw, sw,
                sw, sw, sw,
                sw, sw, sw,
            }}),
        lue::test::create_partitioned_array<InflowCount>(array_shape, partition_shape,
            {{
                0, 0, 0,
                1, 1, 0,
                1, 1, 0,
            }}));
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
    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
            {{
                w, w, w,
                w, w, w,
                w, w, w,
            }}),
        lue::test::create_partitioned_array<InflowCount>(array_shape, partition_shape,
            {{
                1, 1, 0,
                1, 1, 0,
                1, 1, 0,
            }}));
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
    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
            {{
                nw, nw, nw,
                nw, nw, nw,
                nw, nw, nw,
            }}),
        lue::test::create_partitioned_array<InflowCount>(array_shape, partition_shape,
            {{
                1, 1, 0,
                1, 1, 0,
                0, 0, 0,
            }}));
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
    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
            {{
                n, n, n,
                n, n, n,
                n, n, n,
            }}),
        lue::test::create_partitioned_array<InflowCount>(array_shape, partition_shape,
            {{
                1, 1, 1,
                1, 1, 1,
                0, 0, 0,
            }}));
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
    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
            {{
                ne, ne, ne,
                ne, ne, ne,
                ne, ne, ne,
            }}),
        lue::test::create_partitioned_array<InflowCount>(array_shape, partition_shape,
            {{
                0, 1, 1,
                0, 1, 1,
                0, 0, 0,
            }}));
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
    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
            {{
                e, e, e,
                e, e, e,
                e, e, e,
            }}),
        lue::test::create_partitioned_array<InflowCount>(array_shape, partition_shape,
            {{
                0, 1, 1,
                0, 1, 1,
                0, 1, 1,
            }}));
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
    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
            {{
                se, se, se,
                se, se, se,
                se, se, se,
            }}),
        lue::test::create_partitioned_array<InflowCount>(array_shape, partition_shape,
            {{
                0, 0, 0,
                0, 1, 1,
                0, 1, 1,
            }}));
}


BOOST_AUTO_TEST_CASE(partitioned)
{
    Shape const array_shape{{9, 9}};

    test_inflow_count(
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
            {
                {
                    nw, nw, nw,
                    nw, nw, nw,
                    nw, nw, nw,
                },
                {
                     n,  n,  n,
                     n,  n,  n,
                     n,  n,  n,
                },
                {
                    ne, ne, ne,
                    ne, ne, ne,
                    ne, ne, ne,
                },
                {
                     w,  w,  w,
                     w,  w,  w,
                     w,  w,  w,
                },
                {
                     p,  p,  p,
                     p,  p,  p,
                     p,  p,  p,
                },
                {
                     e,  e,  e,
                     e,  e,  e,
                     e,  e,  e,
                },
                {
                    sw, sw, sw,
                    sw, sw, sw,
                    sw, sw, sw,
                },
                {
                     s,  s,  s,
                     s,  s,  s,
                     s,  s,  s,
                },
                {
                    se, se, se,
                    se, se, se,
                    se, se, se,
                },
            }),
        lue::test::create_partitioned_array<InflowCount>(array_shape, partition_shape,
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
            }));
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

    using InflowCountPolicies =
        lue::policy::OutputPolicies<lue::policy::MarkNoDataByValue<CountElement>>;
    using FlowDirectionPolicies =
        lue::policy::InputPolicies<lue::policy::DetectNoDataByValue<FlowDirectionElement>>;
    using Policies = lue::policy::Policies<
        lue::policy::OutputsPolicies<InflowCountPolicies>,
        lue::policy::InputsPolicies<FlowDirectionPolicies>>;

    InflowCountPolicies inflow_count_policies{
        lue::policy::MarkNoDataByValue<CountElement>{99}};
    FlowDirectionPolicies flow_direction_policies{
        lue::policy::DetectNoDataByValue<FlowDirectionElement>{nd}};

    Policies policies{inflow_count_policies, flow_direction_policies};

    test_inflow_count(
        policies,
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
            {{
                nd, nd, nd,
                nd, nd, nd,
                nd, nd, nd,
            }}),
        lue::test::create_partitioned_array<InflowCount>(array_shape, partition_shape,
            {{
                99, 99, 99,
                99, 99, 99,
                99, 99, 99,
            }}));
}


BOOST_AUTO_TEST_CASE(no_data)
{
    using InflowCountPolicies =
        lue::policy::OutputPolicies<lue::policy::MarkNoDataByValue<CountElement>>;
    using FlowDirectionPolicies =
        lue::policy::InputPolicies<lue::policy::DetectNoDataByValue<FlowDirectionElement>>;
    using Policies = lue::policy::Policies<
        lue::policy::OutputsPolicies<InflowCountPolicies>,
        lue::policy::InputsPolicies<FlowDirectionPolicies>>;

    InflowCountPolicies inflow_count_policies{
        lue::policy::MarkNoDataByValue<CountElement>{99}};
    FlowDirectionPolicies flow_direction_policies{
        lue::policy::DetectNoDataByValue<FlowDirectionElement>{nd}};

    Policies policies{inflow_count_policies, flow_direction_policies};

    Shape const array_shape{{3, 6}};

    test_inflow_count(
        policies,
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
            {
                {
                     e,  s, nd,
                     e,  s, nd,
                     e,  s, nd,
                },
                {
                     e,  e,  e,
                     e,  e,  e,
                     e,  e,  e,
                },
            }),
        lue::test::create_partitioned_array<InflowCount>(array_shape, partition_shape,
            {
                {
                    0, 1, 99,
                    0, 2, 99,
                    0, 2, 99,
                },
                {
                    0, 1, 1,
                    0, 1, 1,
                    0, 1, 1,
                },
            }));
}
