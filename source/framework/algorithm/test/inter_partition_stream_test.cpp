#define BOOST_TEST_MODULE lue framework algorithm inter_partition_stream
#include "lue/framework/algorithm/inter_partition_stream.hpp"
#include "lue/framework/algorithm/policy.hpp"
#include "stream.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace {

std::size_t const rank = 2;

using FlowDirectionElement = std::uint32_t;
using FlowDirection = lue::PartitionedArray<FlowDirectionElement, rank>;
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

using CountElement = std::uint64_t;
using InflowCount = lue::PartitionedArray<CountElement, rank>;

using PartitionIO = lue::ArrayPartitionIO<lue::Index, rank, CountElement>;
using PartitionIOArray = lue::PartitionedArray<PartitionIO, rank>;

using BooleanElement = std::uint32_t;
using StreamCells = lue::PartitionedArray<BooleanElement, rank>;
BooleanElement const t{1};
BooleanElement const f{0};

using Shape = lue::ShapeT<FlowDirection>;
Shape const array_shape{{9, 9}};
Shape const partition_shape{{3, 3}};


template<
    typename Policies,
    typename CountElement,
    typename FlowDirectionArray,
    typename StreamCells,
    typename PartitionIOArray>
void test_inter_partition_stream(
    Policies const& policies,
    FlowDirectionArray const& flow_direction,
    StreamCells const& stream_cells_we_want,
    PartitionIOArray const& partition_io_we_want)
{
    using BooleanElement = lue::ElementT<StreamCells>;

    auto const [stream_cells_we_got, partition_io_we_got] =
        lue::inter_partition_stream<Policies, BooleanElement, CountElement>(policies, flow_direction);

    // Verify all cells marked as part of the inter-partition stream match
    lue::test::check_arrays_are_equal(stream_cells_we_got, stream_cells_we_want);

    // Verify the input/output cells found match, and the number of
    // cells contributing to each output cell
    lue::test::check_arrays_are_equal(partition_io_we_got, partition_io_we_want);
}


template<
    typename CountElement,
    typename FlowDirectionArray,
    typename StreamCells,
    typename PartitionIOArray>
void test_inter_partition_stream(
    FlowDirectionArray const& flow_direction,
    StreamCells const& stream_cells_we_want,
    PartitionIOArray const& partition_io_we_want)
{
    using BooleanElement = lue::ElementT<StreamCells>;
    using FlowDirectionElement = lue::ElementT<FlowDirectionArray>;
    using Policies = lue::policy::inter_partition_stream::DefaultPolicies<
        BooleanElement, FlowDirectionElement>;

    test_inter_partition_stream<Policies, CountElement>(
        Policies{}, flow_direction, stream_cells_we_want, partition_io_we_want);
}

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(parallel_east)
{
    PartitionIO io0, io1, io2, io3, io4, io5, io6, io7, io8;
    io0.add({0, 2}, {      }, 3);
    io0.add({1, 2}, {      }, 3);
    io0.add({2, 2}, {      }, 3);

    io1.add({0, 2}, {{0, 0}}, 3);
    io1.add({1, 2}, {{1, 0}}, 3);
    io1.add({2, 2}, {{2, 0}}, 3);

    io2.add({0, 2}, {{0, 0}}, 3);
    io2.add({1, 2}, {{1, 0}}, 3);
    io2.add({2, 2}, {{2, 0}}, 3);

    io3.add({0, 2}, {      }, 3);
    io3.add({1, 2}, {      }, 3);
    io3.add({2, 2}, {      }, 3);

    io4.add({0, 2}, {{0, 0}}, 3);
    io4.add({1, 2}, {{1, 0}}, 3);
    io4.add({2, 2}, {{2, 0}}, 3);

    io5.add({0, 2}, {{0, 0}}, 3);
    io5.add({1, 2}, {{1, 0}}, 3);
    io5.add({2, 2}, {{2, 0}}, 3);

    io6.add({0, 2}, {      }, 3);
    io6.add({1, 2}, {      }, 3);
    io6.add({2, 2}, {      }, 3);

    io7.add({0, 2}, {{0, 0}}, 3);
    io7.add({1, 2}, {{1, 0}}, 3);
    io7.add({2, 2}, {{2, 0}}, 3);

    io8.add({0, 2}, {{0, 0}}, 3);
    io8.add({1, 2}, {{1, 0}}, 3);
    io8.add({2, 2}, {{2, 0}}, 3);

    test_inter_partition_stream<CountElement>(
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
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
            }),
        lue::test::create_partitioned_array<StreamCells>(array_shape, partition_shape,
            {
                {
                    f, f, f,
                    f, f, f,
                    f, f, f,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    f, f, f,
                    f, f, f,
                    f, f, f,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    f, f, f,
                    f, f, f,
                    f, f, f,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
            }),
        lue::test::create_partitioned_array<PartitionIOArray>(partition_shape, {1, 1},
            {
                { io0 }, { io1 }, { io2 },
                { io3 }, { io4 }, { io5 },
                { io6 }, { io7 }, { io8 },
            }));
}


BOOST_AUTO_TEST_CASE(parallel_south_east)
{
    PartitionIO io0, io1, io2, io3, io4, io5, io6, io7, io8;
    io0.add({0, 2}, {      }, 1);
    io0.add({1, 2}, {      }, 2);
    io0.add({2, 0}, {      }, 1);
    io0.add({2, 1}, {      }, 2);
    io0.add({2, 2}, {      }, 3);

    io1.add({0, 2}, {      }, 1);
    io1.add({1, 2}, {      }, 2);
    io1.add({2, 0}, {{2, 0}}, 1);
    io1.add({2, 1}, {{1, 0}}, 2);
    io1.add({2, 2}, {      }, 3);

    io2.add({0, 2}, {      }, 1);
    io2.add({1, 2}, {      }, 2);
    io2.add({2, 0}, {{2, 0}}, 1);
    io2.add({2, 1}, {{1, 0}}, 2);
    io2.add({2, 2}, {      }, 3);

    io3.add({0, 2}, {{0, 2}}, 1);
    io3.add({1, 2}, {{0, 1}}, 2);
    io3.add({2, 0}, {      }, 1);
    io3.add({2, 1}, {      }, 2);
    io3.add({2, 2}, {      }, 3);

    io4.add({0, 2}, {{0, 2}}, 1);
    io4.add({1, 2}, {{0, 1}}, 2);
    io4.add({2, 0}, {{2, 0}}, 1);
    io4.add({2, 1}, {{1, 0}}, 2);
    io4.add({2, 2}, {{0, 0}}, 3);

    io5.add({0, 2}, {{0, 2}}, 1);
    io5.add({1, 2}, {{0, 1}}, 2);
    io5.add({2, 0}, {{2, 0}}, 1);
    io5.add({2, 1}, {{1, 0}}, 2);
    io5.add({2, 2}, {{0, 0}}, 3);

    io6.add({0, 2}, {{0, 2}}, 1);
    io6.add({1, 2}, {{0, 1}}, 2);
    io6.add({2, 0}, {      }, 1);
    io6.add({2, 1}, {      }, 2);
    io6.add({2, 2}, {      }, 3);

    io7.add({0, 2}, {{0, 2}}, 1);
    io7.add({1, 2}, {{0, 1}}, 2);
    io7.add({2, 0}, {{2, 0}}, 1);
    io7.add({2, 1}, {{1, 0}}, 2);
    io7.add({2, 2}, {{0, 0}}, 3);

    io8.add({0, 2}, {{0, 2}}, 1);
    io8.add({1, 2}, {{0, 1}}, 2);
    io8.add({2, 0}, {{2, 0}}, 1);
    io8.add({2, 1}, {{1, 0}}, 2);
    io8.add({2, 2}, {{0, 0}}, 3);

    test_inter_partition_stream<CountElement>(
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
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
            }),
        lue::test::create_partitioned_array<StreamCells>(array_shape, partition_shape,
            {
                {
                    f, f, f,
                    f, f, f,
                    f, f, f,
                },
                {
                    f, f, f,
                    t, f, f,
                    t, t, f,
                },
                {
                    f, f, f,
                    t, f, f,
                    t, t, f,
                },
                {
                    f, t, t,
                    f, f, t,
                    f, f, f,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    f, t, t,
                    f, f, t,
                    f, f, f,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
            }),
        lue::test::create_partitioned_array<PartitionIOArray>(partition_shape, {1, 1},
            {
                { io0 }, { io1 }, { io2 },
                { io3 }, { io4 }, { io5 },
                { io6 }, { io7 }, { io8 },
            }));
}


BOOST_AUTO_TEST_CASE(parallel_south)
{
    PartitionIO io0, io1, io2, io3, io4, io5, io6, io7, io8;
    io0.add({2, 0}, {      }, 3);
    io0.add({2, 1}, {      }, 3);
    io0.add({2, 2}, {      }, 3);

    io1.add({2, 0}, {      }, 3);
    io1.add({2, 1}, {      }, 3);
    io1.add({2, 2}, {      }, 3);

    io2.add({2, 0}, {      }, 3);
    io2.add({2, 1}, {      }, 3);
    io2.add({2, 2}, {      }, 3);

    io3.add({2, 0}, {{0, 0}}, 3);
    io3.add({2, 1}, {{0, 1}}, 3);
    io3.add({2, 2}, {{0, 2}}, 3);

    io4.add({2, 0}, {{0, 0}}, 3);
    io4.add({2, 1}, {{0, 1}}, 3);
    io4.add({2, 2}, {{0, 2}}, 3);

    io5.add({2, 0}, {{0, 0}}, 3);
    io5.add({2, 1}, {{0, 1}}, 3);
    io5.add({2, 2}, {{0, 2}}, 3);

    io6.add({2, 0}, {{0, 0}}, 3);
    io6.add({2, 1}, {{0, 1}}, 3);
    io6.add({2, 2}, {{0, 2}}, 3);

    io7.add({2, 0}, {{0, 0}}, 3);
    io7.add({2, 1}, {{0, 1}}, 3);
    io7.add({2, 2}, {{0, 2}}, 3);

    io8.add({2, 0}, {{0, 0}}, 3);
    io8.add({2, 1}, {{0, 1}}, 3);
    io8.add({2, 2}, {{0, 2}}, 3);

    test_inter_partition_stream<CountElement>(
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
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
            }),
        lue::test::create_partitioned_array<StreamCells>(array_shape, partition_shape,
            {
                {
                    f, f, f,
                    f, f, f,
                    f, f, f,
                },
                {
                    f, f, f,
                    f, f, f,
                    f, f, f,
                },
                {
                    f, f, f,
                    f, f, f,
                    f, f, f,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
            }),
        lue::test::create_partitioned_array<PartitionIOArray>(partition_shape, {1, 1},
            {
                { io0 }, { io1 }, { io2 },
                { io3 }, { io4 }, { io5 },
                { io6 }, { io7 }, { io8 },
            }));
}


BOOST_AUTO_TEST_CASE(parallel_south_west)
{
    PartitionIO io0, io1, io2, io3, io4, io5, io6, io7, io8;
    io0.add({0, 0}, {      }, 1);
    io0.add({1, 0}, {      }, 2);
    io0.add({2, 0}, {      }, 3);
    io0.add({2, 1}, {{1, 2}}, 2);
    io0.add({2, 2}, {{2, 2}}, 1);

    io1.add({0, 0}, {      }, 1);
    io1.add({1, 0}, {      }, 2);
    io1.add({2, 0}, {      }, 3);
    io1.add({2, 1}, {{1, 2}}, 2);
    io1.add({2, 2}, {{2, 2}}, 1);

    io2.add({0, 0}, {      }, 1);
    io2.add({1, 0}, {      }, 2);
    io2.add({2, 0}, {      }, 3);
    io2.add({2, 1}, {      }, 2);
    io2.add({2, 2}, {      }, 1);

    io3.add({0, 0}, {{0, 0}}, 1);
    io3.add({1, 0}, {{0, 1}}, 2);
    io3.add({2, 0}, {{0, 2}}, 3);
    io3.add({2, 1}, {{1, 2}}, 2);
    io3.add({2, 2}, {{2, 2}}, 1);

    io4.add({0, 0}, {{0, 0}}, 1);
    io4.add({1, 0}, {{0, 1}}, 2);
    io4.add({2, 0}, {{0, 2}}, 3);
    io4.add({2, 1}, {{1, 2}}, 2);
    io4.add({2, 2}, {{2, 2}}, 1);

    io5.add({0, 0}, {{0, 0}}, 1);
    io5.add({1, 0}, {{0, 1}}, 2);
    io5.add({2, 0}, {      }, 3);
    io5.add({2, 1}, {      }, 2);
    io5.add({2, 2}, {      }, 1);

    io6.add({0, 0}, {{0, 0}}, 1);
    io6.add({1, 0}, {{0, 1}}, 2);
    io6.add({2, 0}, {{0, 2}}, 3);
    io6.add({2, 1}, {{1, 2}}, 2);
    io6.add({2, 2}, {{2, 2}}, 1);

    io7.add({0, 0}, {{0, 0}}, 1);
    io7.add({1, 0}, {{0, 1}}, 2);
    io7.add({2, 0}, {{0, 2}}, 3);
    io7.add({2, 1}, {{1, 2}}, 2);
    io7.add({2, 2}, {{2, 2}}, 1);

    io8.add({0, 0}, {{0, 0}}, 1);
    io8.add({1, 0}, {{0, 1}}, 2);
    io8.add({2, 0}, {      }, 3);
    io8.add({2, 1}, {      }, 2);
    io8.add({2, 2}, {      }, 1);

    test_inter_partition_stream<CountElement>(
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
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
            }),
        lue::test::create_partitioned_array<StreamCells>(array_shape, partition_shape,
            {
                {
                    f, f, f,
                    f, f, t,
                    f, t, t,
                },
                {
                    f, f, f,
                    f, f, t,
                    f, t, t,
                },
                {
                    f, f, f,
                    f, f, f,
                    f, f, f,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    t, t, f,
                    t, f, f,
                    f, f, f,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    t, t, f,
                    t, f, f,
                    f, f, f,
                },
            }),
        lue::test::create_partitioned_array<PartitionIOArray>(partition_shape, {1, 1},
            {
                { io0 }, { io1 }, { io2 },
                { io3 }, { io4 }, { io5 },
                { io6 }, { io7 }, { io8 },
            }));
}


BOOST_AUTO_TEST_CASE(parallel_west)
{
    PartitionIO io0, io1, io2, io3, io4, io5, io6, io7, io8;
    io0.add({0, 0}, {{0, 2}}, 3);
    io0.add({1, 0}, {{1, 2}}, 3);
    io0.add({2, 0}, {{2, 2}}, 3);

    io1.add({0, 0}, {{0, 2}}, 3);
    io1.add({1, 0}, {{1, 2}}, 3);
    io1.add({2, 0}, {{2, 2}}, 3);

    io2.add({0, 0}, {      }, 3);
    io2.add({1, 0}, {      }, 3);
    io2.add({2, 0}, {      }, 3);

    io3.add({0, 0}, {{0, 2}}, 3);
    io3.add({1, 0}, {{1, 2}}, 3);
    io3.add({2, 0}, {{2, 2}}, 3);

    io4.add({0, 0}, {{0, 2}}, 3);
    io4.add({1, 0}, {{1, 2}}, 3);
    io4.add({2, 0}, {{2, 2}}, 3);

    io5.add({0, 0}, {      }, 3);
    io5.add({1, 0}, {      }, 3);
    io5.add({2, 0}, {      }, 3);

    io6.add({0, 0}, {{0, 2}}, 3);
    io6.add({1, 0}, {{1, 2}}, 3);
    io6.add({2, 0}, {{2, 2}}, 3);

    io7.add({0, 0}, {{0, 2}}, 3);
    io7.add({1, 0}, {{1, 2}}, 3);
    io7.add({2, 0}, {{2, 2}}, 3);

    io8.add({0, 0}, {      }, 3);
    io8.add({1, 0}, {      }, 3);
    io8.add({2, 0}, {      }, 3);

    test_inter_partition_stream<CountElement>(
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
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
            }),
        lue::test::create_partitioned_array<StreamCells>(array_shape, partition_shape,
            {
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    f, f, f,
                    f, f, f,
                    f, f, f,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    f, f, f,
                    f, f, f,
                    f, f, f,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    f, f, f,
                    f, f, f,
                    f, f, f,
                },
            }),
        lue::test::create_partitioned_array<PartitionIOArray>(partition_shape, {1, 1},
            {
                { io0 }, { io1 }, { io2 },
                { io3 }, { io4 }, { io5 },
                { io6 }, { io7 }, { io8 },
            }));
}


BOOST_AUTO_TEST_CASE(parallel_north_west)
{
    PartitionIO io0, io1, io2, io3, io4, io5, io6, io7, io8;
    io0.add({0, 0}, {{2, 2}}, 3);
    io0.add({0, 1}, {{1, 2}}, 2);
    io0.add({0, 2}, {{0, 2}}, 1);
    io0.add({1, 0}, {{2, 1}}, 2);
    io0.add({2, 0}, {{2, 0}}, 1);

    io1.add({0, 0}, {{2, 2}}, 3);
    io1.add({0, 1}, {{1, 2}}, 2);
    io1.add({0, 2}, {{0, 2}}, 1);
    io1.add({1, 0}, {{2, 1}}, 2);
    io1.add({2, 0}, {{2, 0}}, 1);

    io2.add({0, 0}, {      }, 3);
    io2.add({0, 1}, {      }, 2);
    io2.add({0, 2}, {      }, 1);
    io2.add({1, 0}, {{2, 1}}, 2);
    io2.add({2, 0}, {{2, 0}}, 1);

    io3.add({0, 0}, {{2, 2}}, 3);
    io3.add({0, 1}, {{1, 2}}, 2);
    io3.add({0, 2}, {{0, 2}}, 1);
    io3.add({1, 0}, {{2, 1}}, 2);
    io3.add({2, 0}, {{2, 0}}, 1);

    io4.add({0, 0}, {{2, 2}}, 3);
    io4.add({0, 1}, {{1, 2}}, 2);
    io4.add({0, 2}, {{0, 2}}, 1);
    io4.add({1, 0}, {{2, 1}}, 2);
    io4.add({2, 0}, {{2, 0}}, 1);

    io5.add({0, 0}, {      }, 3);
    io5.add({0, 1}, {      }, 2);
    io5.add({0, 2}, {      }, 1);
    io5.add({1, 0}, {{2, 1}}, 2);
    io5.add({2, 0}, {{2, 0}}, 1);

    io6.add({0, 0}, {      }, 3);
    io6.add({0, 1}, {{1, 2}}, 2);
    io6.add({0, 2}, {{0, 2}}, 1);
    io6.add({1, 0}, {      }, 2);
    io6.add({2, 0}, {      }, 1);

    io7.add({0, 0}, {      }, 3);
    io7.add({0, 1}, {{1, 2}}, 2);
    io7.add({0, 2}, {{0, 2}}, 1);
    io7.add({1, 0}, {      }, 2);
    io7.add({2, 0}, {      }, 1);

    io8.add({0, 0}, {      }, 3);
    io8.add({0, 1}, {      }, 2);
    io8.add({0, 2}, {      }, 1);
    io8.add({1, 0}, {      }, 2);
    io8.add({2, 0}, {      }, 1);

    test_inter_partition_stream<CountElement>(
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
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
            }),
        lue::test::create_partitioned_array<StreamCells>(array_shape, partition_shape,
            {
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    f, f, f,
                    t, f, f,
                    t, t, f,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    f, f, f,
                    t, f, f,
                    t, t, f,
                },
                {
                    f, t, t,
                    f, f, t,
                    f, f, f,
                },
                {
                    f, t, t,
                    f, f, t,
                    f, f, f,
                },
                {
                    f, f, f,
                    f, f, f,
                    f, f, f,
                },
            }),
        lue::test::create_partitioned_array<PartitionIOArray>(partition_shape, {1, 1},
            {
                { io0 }, { io1 }, { io2 },
                { io3 }, { io4 }, { io5 },
                { io6 }, { io7 }, { io8 },
            }));
}


BOOST_AUTO_TEST_CASE(parallel_north)
{
    PartitionIO io0, io1, io2, io3, io4, io5, io6, io7, io8;
    io0.add({0, 0}, {{2, 0}}, 3);
    io0.add({0, 1}, {{2, 1}}, 3);
    io0.add({0, 2}, {{2, 2}}, 3);

    io1.add({0, 0}, {{2, 0}}, 3);
    io1.add({0, 1}, {{2, 1}}, 3);
    io1.add({0, 2}, {{2, 2}}, 3);

    io2.add({0, 0}, {{2, 0}}, 3);
    io2.add({0, 1}, {{2, 1}}, 3);
    io2.add({0, 2}, {{2, 2}}, 3);

    io3.add({0, 0}, {{2, 0}}, 3);
    io3.add({0, 1}, {{2, 1}}, 3);
    io3.add({0, 2}, {{2, 2}}, 3);

    io4.add({0, 0}, {{2, 0}}, 3);
    io4.add({0, 1}, {{2, 1}}, 3);
    io4.add({0, 2}, {{2, 2}}, 3);

    io5.add({0, 0}, {{2, 0}}, 3);
    io5.add({0, 1}, {{2, 1}}, 3);
    io5.add({0, 2}, {{2, 2}}, 3);

    io6.add({0, 0}, {      }, 3);
    io6.add({0, 1}, {      }, 3);
    io6.add({0, 2}, {      }, 3);

    io7.add({0, 0}, {      }, 3);
    io7.add({0, 1}, {      }, 3);
    io7.add({0, 2}, {      }, 3);

    io8.add({0, 0}, {      }, 3);
    io8.add({0, 1}, {      }, 3);
    io8.add({0, 2}, {      }, 3);

    test_inter_partition_stream<CountElement>(
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
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
            }),
        lue::test::create_partitioned_array<StreamCells>(array_shape, partition_shape,
            {
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    f, f, f,
                    f, f, f,
                    f, f, f,
                },
                {
                    f, f, f,
                    f, f, f,
                    f, f, f,
                },
                {
                    f, f, f,
                    f, f, f,
                    f, f, f,
                },
            }),
        lue::test::create_partitioned_array<PartitionIOArray>(partition_shape, {1, 1},
            {
                { io0 }, { io1 }, { io2 },
                { io3 }, { io4 }, { io5 },
                { io6 }, { io7 }, { io8 },
            }));
}


BOOST_AUTO_TEST_CASE(parallel_north_east)
{
    PartitionIO io0, io1, io2, io3, io4, io5, io6, io7, io8;
    io0.add({0, 0}, {      }, 1);
    io0.add({0, 1}, {      }, 2);
    io0.add({0, 2}, {      }, 3);
    io0.add({1, 2}, {{2, 1}}, 2);
    io0.add({2, 2}, {{2, 2}}, 1);

    io1.add({0, 0}, {{0, 0}}, 1);
    io1.add({0, 1}, {{1, 0}}, 2);
    io1.add({0, 2}, {{2, 0}}, 3);
    io1.add({1, 2}, {{2, 1}}, 2);
    io1.add({2, 2}, {{2, 2}}, 1);

    io2.add({0, 0}, {{0, 0}}, 1);
    io2.add({0, 1}, {{1, 0}}, 2);
    io2.add({0, 2}, {{2, 0}}, 3);
    io2.add({1, 2}, {{2, 1}}, 2);
    io2.add({2, 2}, {{2, 2}}, 1);

    io3.add({0, 0}, {      }, 1);
    io3.add({0, 1}, {      }, 2);
    io3.add({0, 2}, {      }, 3);
    io3.add({1, 2}, {{2, 1}}, 2);
    io3.add({2, 2}, {{2, 2}}, 1);

    io4.add({0, 0}, {{0, 0}}, 1);
    io4.add({0, 1}, {{1, 0}}, 2);
    io4.add({0, 2}, {{2, 0}}, 3);
    io4.add({1, 2}, {{2, 1}}, 2);
    io4.add({2, 2}, {{2, 2}}, 1);

    io5.add({0, 0}, {{0, 0}}, 1);
    io5.add({0, 1}, {{1, 0}}, 2);
    io5.add({0, 2}, {{2, 0}}, 3);
    io5.add({1, 2}, {{2, 1}}, 2);
    io5.add({2, 2}, {{2, 2}}, 1);

    io6.add({0, 0}, {      }, 1);
    io6.add({0, 1}, {      }, 2);
    io6.add({0, 2}, {      }, 3);
    io6.add({1, 2}, {      }, 2);
    io6.add({2, 2}, {      }, 1);

    io7.add({0, 0}, {{0, 0}}, 1);
    io7.add({0, 1}, {{1, 0}}, 2);
    io7.add({0, 2}, {      }, 3);
    io7.add({1, 2}, {      }, 2);
    io7.add({2, 2}, {      }, 1);

    io8.add({0, 0}, {{0, 0}}, 1);
    io8.add({0, 1}, {{1, 0}}, 2);
    io8.add({0, 2}, {      }, 3);
    io8.add({1, 2}, {      }, 2);
    io8.add({2, 2}, {      }, 1);

    test_inter_partition_stream<CountElement>(
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
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
            }),
        lue::test::create_partitioned_array<StreamCells>(array_shape, partition_shape,
            {
                {
                    f, f, f,
                    f, f, t,
                    f, t, t,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    f, f, f,
                    f, f, t,
                    f, t, t,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    t, t, t,
                    t, t, t,
                    t, t, t,
                },
                {
                    f, f, f,
                    f, f, f,
                    f, f, f,
                },
                {
                    t, t, f,
                    t, f, f,
                    f, f, f,
                },
                {
                    t, t, f,
                    t, f, f,
                    f, f, f,
                },
            }),
        lue::test::create_partitioned_array<PartitionIOArray>(partition_shape, {1, 1},
            {
                { io0 }, { io1 }, { io2 },
                { io3 }, { io4 }, { io5 },
                { io6 }, { io7 }, { io8 },
            }));
}


BOOST_AUTO_TEST_CASE(pit)
{
    // +---+---+---+
    // | 🡦 | 🡣 | 🡧 |
    // +---+---+---+
    // | 🡢 | ■ | 🡠 |
    // +---+---+---+
    // | 🡥 | 🡡 | 🡤 |
    // +---+---+---+
    PartitionIO io0;
    // No partition output cells...

    test_inter_partition_stream<CountElement>(
        lue::test::create_partitioned_array<FlowDirection>({3, 3}, partition_shape,
            {{
                se,  s, sw,
                 e,  p,  w,
                ne,  n, nw,
            }}),
        lue::test::create_partitioned_array<StreamCells>({3, 3}, partition_shape,
            {{
                f, f, f,
                f, f, f,
                f, f, f,
            }}),
        lue::test::create_partitioned_array<PartitionIOArray>(Shape{{1, 1}}, {1, 1},
            {{
                io0,
            }}));
}


BOOST_AUTO_TEST_CASE(peak)
{
    // +---+---+---+
    // | 🡤 | 🡡 | 🡥 |
    // +---+---+---+
    // | 🡠 | 🡣 | 🡢 |
    // +---+---+---+
    // | 🡧 | 🡣 | 🡦 |
    // +---+---+---+
    PartitionIO io0;
    io0.add({0, 0}, {      }, 1);
    io0.add({0, 1}, {      }, 1);
    io0.add({0, 2}, {      }, 1);
    io0.add({1, 0}, {      }, 1);
    io0.add({1, 2}, {      }, 1);
    io0.add({2, 0}, {      }, 1);
    io0.add({2, 1}, {      }, 2);
    io0.add({2, 2}, {      }, 1);

    test_inter_partition_stream<CountElement>(
        lue::test::create_partitioned_array<FlowDirection>({3, 3}, partition_shape,
            {{
                nw,  n, ne,
                 w,  s,  e,
                sw,  s, se,
            }}),
        lue::test::create_partitioned_array<StreamCells>({3, 3}, partition_shape,
            {{
                f, f, f,
                f, f, f,
                f, f, f,
            }}),
        lue::test::create_partitioned_array<PartitionIOArray>(Shape{{1, 1}}, {1, 1},
            {{
                io0,
            }}));
}


BOOST_AUTO_TEST_CASE(converge)
{
    // +---+---+---+
    // | 🡦 | 🡣 | 🡧 |
    // +---+---+---+
    // | 🡢 | 🡣 | 🡠 |
    // +---+---+---+
    // | 🡢 | 🡣 | 🡠 |
    // +---+---+---+
    PartitionIO io0;
    // All cells contribute
    io0.add({2, 1}, {      }, 9);

    test_inter_partition_stream<CountElement>(
        lue::test::create_partitioned_array<FlowDirection>({3, 3}, partition_shape,
            {{
                se,  s, sw,
                 e,  s,  w,
                 e,  s,  w,
            }}),
        lue::test::create_partitioned_array<StreamCells>({3, 3}, partition_shape,
            {{
                f, f, f,
                f, f, f,
                f, f, f,
            }}),
        lue::test::create_partitioned_array<PartitionIOArray>(Shape{{1, 1}}, {1, 1},
            {{
                io0,
            }}));
}


BOOST_AUTO_TEST_CASE(all_no_data)
{
    // +---+---+---+
    // | X | X | X |
    // +---+---+---+
    // | X | X | X |
    // +---+---+---+
    // | X | X | X |
    // +---+---+---+

    // Outputs
    using StreamPolicies =
        lue::policy::OutputPolicies<lue::policy::MarkNoDataByValue<BooleanElement>>;

    // Inputs
    using FlowDirectionPolicies =
        lue::policy::InputPolicies<lue::policy::DetectNoDataByValue<FlowDirectionElement>>;

    using Policies = lue::policy::Policies<
        lue::policy::OutputsPolicies<StreamPolicies>,
        lue::policy::InputsPolicies<FlowDirectionPolicies>>;

    StreamPolicies stream_policies{
        lue::policy::MarkNoDataByValue<BooleanElement>{99}};
    FlowDirectionPolicies flow_direction_policies{
        lue::policy::DetectNoDataByValue<FlowDirectionElement>{nd}};

    Policies policies{stream_policies, flow_direction_policies};

    PartitionIO io0;

    test_inter_partition_stream<Policies, CountElement>(
        policies,
        lue::test::create_partitioned_array<FlowDirection>({3, 3}, partition_shape,
            {{
                nd, nd, nd,
                nd, nd, nd,
                nd, nd, nd,
            }}),
        lue::test::create_partitioned_array<StreamCells>({3, 3}, partition_shape,
            {{
                99, 99, 99,
                99, 99, 99,
                99, 99, 99,
            }}),
        lue::test::create_partitioned_array<PartitionIOArray>(Shape{{1, 1}}, {1, 1},
            {{
                io0,
            }}));
}


BOOST_AUTO_TEST_CASE(no_data)
{
    // Outputs
    using StreamPolicies =
        lue::policy::OutputPolicies<lue::policy::MarkNoDataByValue<BooleanElement>>;

    // Inputs
    using FlowDirectionPolicies =
        lue::policy::InputPolicies<lue::policy::DetectNoDataByValue<FlowDirectionElement>>;

    using Policies = lue::policy::Policies<
        lue::policy::OutputsPolicies<StreamPolicies>,
        lue::policy::InputsPolicies<FlowDirectionPolicies>>;

    StreamPolicies stream_policies{
        lue::policy::MarkNoDataByValue<BooleanElement>{99}};
    FlowDirectionPolicies flow_direction_policies{
        lue::policy::DetectNoDataByValue<FlowDirectionElement>{nd}};

    Policies policies{stream_policies, flow_direction_policies};

    PartitionIO io0, io1;
    io0.add({1, 2}, {      }, 1);
    io0.add({2, 1}, {      }, 6);

    io1.add({0, 2}, {      }, 3);
    io1.add({1, 2}, {{1, 0}}, 3);
    io1.add({2, 2}, {      }, 3);

    test_inter_partition_stream<Policies, CountElement>(
        policies,
        lue::test::create_partitioned_array<FlowDirection>({3, 6}, partition_shape,
            {
                {
                     e,  s, nd,
                     e,  s,  e,
                     e,  s, nd,
                },
                {
                     e,  e,  e,
                     e,  e,  e,
                     e,  e,  e,
                },
            }),
        lue::test::create_partitioned_array<StreamCells>({3, 6}, partition_shape,
            {
                {
                    f, f, 99,
                    f, f,  f,
                    f, f, 99,
                },
                {
                    f, f, f,
                    t, t, t,
                    f, f, f,
                },
            }),
        lue::test::create_partitioned_array<PartitionIOArray>(Shape{{1, 2}}, {1, 1},
            {
                {
                    io0,
                },
                {
                    io1,
                },
            }));
}


BOOST_AUTO_TEST_CASE(merging_inter_partition_streams)
{
    // Outputs
    using StreamPolicies =
        lue::policy::OutputPolicies<lue::policy::MarkNoDataByValue<BooleanElement>>;

    // Inputs
    using FlowDirectionPolicies =
        lue::policy::InputPolicies<lue::policy::DetectNoDataByValue<FlowDirectionElement>>;

    using Policies = lue::policy::Policies<
        lue::policy::OutputsPolicies<StreamPolicies>,
        lue::policy::InputsPolicies<FlowDirectionPolicies>>;

    StreamPolicies stream_policies{
        lue::policy::MarkNoDataByValue<BooleanElement>{99}};
    FlowDirectionPolicies flow_direction_policies{
        lue::policy::DetectNoDataByValue<FlowDirectionElement>{nd}};

    Policies policies{stream_policies, flow_direction_policies};

    PartitionIO io0, io1, io2, io3, io4, io5, io6, io7, io8;
    io0.add({2, 2}, {      }, 1);

    io1.add({0, 1}, {      }, 1);
    io1.add({0, 2}, {      }, 1);
    io1.add({1, 2}, {      }, 1);
    io1.add({2, 1}, {      }, 2);

    io2.add({2, 1}, {{1, 0}, {1, 0}}, 6);
    io2.add({2, 2}, {      }, 1);

    io3.add({0, 2}, {      }, 1);
    io3.add({1, 2}, {      }, 1);
    io3.add({2, 2}, {      }, 1);

    io4.add({2, 1}, {{0, 0}, {0, 0}, {0, 0}, {0, 1}, {0, 2}, {1, 2}, {2, 0}, {2, 2}, {2, 2}}, 8);

    io5.add({0, 0}, {{0, 1}, {0, 1}}, 2);
    io5.add({1, 0}, {      }, 1);
    io5.add({2, 0}, {      }, 1);

    io6.add({2, 2}, {{2, 2}, {2, 2}}, 1);

    io7.add({1, 0}, {{0, 1}, {1, 2}}, 4);
    io7.add({2, 0}, {{2, 2}}, 3);

    io8.add({0, 0}, {      }, 1);
    io8.add({1, 0}, {      }, 1);
    io8.add({2, 0}, {      }, 1);

    test_inter_partition_stream<Policies, CountElement>(
        policies,
        lue::test::create_partitioned_array<FlowDirection>(array_shape, partition_shape,
            {
                { // 0, 0
                    nd, nd, nd,
                    nd, nd, nd,
                    nd, nd, se,
                },
                { // 0, 1
                    nd, nw, se,
                    nd,  s,  e,
                    nd,  s, nd,
                },
                { // 0, 2
                    nd,  s, sw,
                    se,  s,  w,
                    nd,  s, sw,
                },
                { // 1, 0
                    nd, nd,  e,
                    nd, nd, ne,
                    nd, nd,  e,
                },
                { // 1, 1
                     e,  s,  w,
                    nd,  s,  w,
                     e,  s,  w,
                },
                { // 1, 2
                     w,  w, nd,
                     w, nd, nd,
                     w, nd, nd,
                },
                { // 2, 0
                    nd, nd, nd,
                    nd, nd, nd,
                    nd, nd,  s,
                },
                { // 2, 1
                    nd,  s, nd,
                    sw,  w,  w,
                     w,  w,  w,
                },
                { // 2, 2
                    nw, nd, nd,
                     w, nd, nd,
                     w, nd, nd,
                },
            }),
        lue::test::create_partitioned_array<StreamCells>(array_shape, partition_shape,
            {
                {
                    99, 99, 99,
                    99, 99, 99,
                    99, 99,  f,
                },
                {
                    99,  f,  f,
                    99,  f,  f,
                    99,  f, 99,
                },
                {
                    99,  f,  f,
                     t,  f,  f,
                    99,  t,  f,
                },
                {
                    99, 99,  f,
                    99, 99,  f,
                    99, 99,  f,
                },
                {
                     t,  t,  t,
                    99,  t,  t,
                     t,  t,  t,
                },
                {
                     t,  t, 99,
                     f, 99, 99,
                     f, 99, 99,
                },
                {
                    99, 99, 99,
                    99, 99, 99,
                    99, 99,  t,
                },
                {
                    99,  t, 99,
                     t,  t,  t,
                     t,  t,  t,
                },
                {
                     f, 99, 99,
                     f, 99, 99,
                     f, 99, 99,
                },
            }),
        lue::test::create_partitioned_array<PartitionIOArray>(partition_shape, {1, 1},
            {
                { io0 }, { io1 }, { io2 },
                { io3 }, { io4 }, { io5 },
                { io6 }, { io7 }, { io8 },
            }));
}
