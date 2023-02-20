#define BOOST_TEST_MODULE lue framework algorithm array_partition_io
#include "lue/framework/algorithm/detail/array_partition_io.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework/test/stream.hpp"


BOOST_AUTO_TEST_CASE(default_construct)
{
    using ArrayPartitionIO = lue::detail::ArrayPartitionIO<lue::Index, 2, float>;

    ArrayPartitionIO partition_io{};

    BOOST_CHECK(partition_io.is_solved());
    BOOST_CHECK(partition_io.is_drained());
}


BOOST_AUTO_TEST_CASE(use_case_01)
{
    using Value = float;
    using ArrayPartitionIO = lue::detail::ArrayPartitionIO<lue::Index, 2, Value>;
    using Shape = typename ArrayPartitionIO::Shape;
    using Indices = ArrayPartitionIO::Indices;
    using CellsIdxs = ArrayPartitionIO::CellsIdxs;
    using Offset = ArrayPartitionIO::Offset;
    using PartitionOffsets = ArrayPartitionIO::PartitionOffsets;

    // Copy some of the arguments, instead of move. We need them for testing.

    Shape const partition_shape{5, 5};
    CellsIdxs input_cell_idxs{{0, 0}, {0, 2}, {2, 0}, {4, 1}, {4, 2}};
    ArrayPartitionIO partition_io{partition_shape, CellsIdxs{input_cell_idxs}};

    Indices output_cell_idxs{{0, 4}};
    Offset offset{-1, 1};
    Value value{5.5};

    partition_io.add_output_cell(output_cell_idxs, Offset{offset}, Value{value});

    // BOOST_CHECK_EQUAL(partition_io.offset(output_cell_idxs), offset);
    // BOOST_CHECK_EQUAL(partition_io.value(output_cell_idxs), value);
    BOOST_CHECK(!partition_io.is_solved());

    partition_io.remove_input_cell({2, 0});
    BOOST_CHECK(!partition_io.is_solved());

    partition_io.remove_input_cell({0, 0});
    BOOST_CHECK(!partition_io.is_solved());
    partition_io.remove_input_cell({0, 2});
    BOOST_CHECK(!partition_io.is_solved());
    partition_io.remove_input_cell({4, 1});
    BOOST_CHECK(!partition_io.is_solved());
    partition_io.remove_input_cell({4, 2});
    BOOST_CHECK(partition_io.is_solved());

    {
        // Given the offset of north-east output cell (0, 4), the target
        // partition is located to the north-east (-1, 1).
        PartitionOffsets partition_offsets_we_want{{-1, 1}};
        PartitionOffsets partition_offsets_we_got{partition_io.partition_offsets()};

        BOOST_CHECK_EQUAL_COLLECTIONS(
            partition_offsets_we_got.begin(),
            partition_offsets_we_got.end(),
            partition_offsets_we_want.begin(),
            partition_offsets_we_want.end());
    }
}


BOOST_AUTO_TEST_CASE(drain_full_coverage)
{
    // Check whether the calculations for input cells in neighbouring
    // partitions are correct

    // Given an 5x5 partition, put output cells at various locations
    // along the border

    using Value = std::int32_t;
    using ArrayPartitionIO = lue::detail::ArrayPartitionIO<lue::Index, 2, Value>;
    using Shape = typename ArrayPartitionIO::Shape;
    using Indices = ArrayPartitionIO::Indices;
    using PartitionOffsets = ArrayPartitionIO::PartitionOffsets;

    using Tuples = std::vector<std::tuple<Indices, Value>>;

    Shape const partition_shape{5, 5};
    Value const value{55};

    {
        // downstream is north
        Shape const north_partition_shape{{3, 5}};
        PartitionOffsets const partition_offsets_we_want{{-1, 0}};

        {
            Indices output_cell{0, 0};

            {
                ArrayPartitionIO partition_io{partition_shape, {}};
                partition_io.add_output_cell(output_cell, {-1, 0}, value);

                Indices const input_cell_we_want{2, 0};

                PartitionOffsets const partition_offsets_we_got{partition_io.partition_offsets()};
                BOOST_REQUIRE_EQUAL_COLLECTIONS(
                    partition_offsets_we_got.begin(),
                    partition_offsets_we_got.end(),
                    partition_offsets_we_want.begin(),
                    partition_offsets_we_want.end());

                Tuples const tuples_we_want{{input_cell_we_want, value}};
                Tuples const tuples_we_got{
                    partition_io.drain(*partition_offsets_we_want.begin(), north_partition_shape)};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    tuples_we_got.begin(), tuples_we_got.end(), tuples_we_want.begin(), tuples_we_want.end());
            }

            {
                ArrayPartitionIO partition_io{partition_shape, {}};
                partition_io.add_output_cell(output_cell, {-1, 1}, value);

                Indices const input_cell_we_want{2, 1};

                PartitionOffsets const partition_offsets_we_got{partition_io.partition_offsets()};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    partition_offsets_we_got.begin(),
                    partition_offsets_we_got.end(),
                    partition_offsets_we_want.begin(),
                    partition_offsets_we_want.end());

                Tuples const tuples_we_want{{input_cell_we_want, value}};
                Tuples const tuples_we_got{
                    partition_io.drain(*partition_offsets_we_want.begin(), north_partition_shape)};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    tuples_we_got.begin(), tuples_we_got.end(), tuples_we_want.begin(), tuples_we_want.end());
            }
        }

        {
            Indices output_cell{0, 2};

            {
                ArrayPartitionIO partition_io{partition_shape, {}};
                partition_io.add_output_cell(output_cell, {-1, -1}, value);

                Indices const input_cell_we_want{2, 1};

                PartitionOffsets const partition_offsets_we_got{partition_io.partition_offsets()};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    partition_offsets_we_got.begin(),
                    partition_offsets_we_got.end(),
                    partition_offsets_we_want.begin(),
                    partition_offsets_we_want.end());

                Tuples const tuples_we_want{{input_cell_we_want, value}};
                Tuples const tuples_we_got{
                    partition_io.drain(*partition_offsets_we_want.begin(), north_partition_shape)};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    tuples_we_got.begin(), tuples_we_got.end(), tuples_we_want.begin(), tuples_we_want.end());
            }

            {
                ArrayPartitionIO partition_io{partition_shape, {}};
                partition_io.add_output_cell(output_cell, {-1, 0}, value);

                Indices const input_cell_we_want{2, 2};

                PartitionOffsets const partition_offsets_we_got{partition_io.partition_offsets()};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    partition_offsets_we_got.begin(),
                    partition_offsets_we_got.end(),
                    partition_offsets_we_want.begin(),
                    partition_offsets_we_want.end());

                Tuples const tuples_we_want{{input_cell_we_want, value}};
                Tuples const tuples_we_got{
                    partition_io.drain(*partition_offsets_we_want.begin(), north_partition_shape)};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    tuples_we_got.begin(), tuples_we_got.end(), tuples_we_want.begin(), tuples_we_want.end());
            }

            {
                ArrayPartitionIO partition_io{partition_shape, {}};
                partition_io.add_output_cell(output_cell, {-1, 1}, value);

                Indices const input_cell_we_want{2, 3};

                PartitionOffsets const partition_offsets_we_got{partition_io.partition_offsets()};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    partition_offsets_we_got.begin(),
                    partition_offsets_we_got.end(),
                    partition_offsets_we_want.begin(),
                    partition_offsets_we_want.end());

                Tuples const tuples_we_want{{input_cell_we_want, value}};
                Tuples const tuples_we_got{
                    partition_io.drain(*partition_offsets_we_want.begin(), north_partition_shape)};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    tuples_we_got.begin(), tuples_we_got.end(), tuples_we_want.begin(), tuples_we_want.end());
            }
        }

        {
            Indices output_cell{0, 4};

            {
                ArrayPartitionIO partition_io{partition_shape, {}};
                partition_io.add_output_cell(output_cell, {-1, -1}, value);

                Indices const input_cell_we_want{2, 3};

                PartitionOffsets const partition_offsets_we_got{partition_io.partition_offsets()};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    partition_offsets_we_got.begin(),
                    partition_offsets_we_got.end(),
                    partition_offsets_we_want.begin(),
                    partition_offsets_we_want.end());

                Tuples const tuples_we_want{{input_cell_we_want, value}};
                Tuples const tuples_we_got{
                    partition_io.drain(*partition_offsets_we_want.begin(), north_partition_shape)};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    tuples_we_got.begin(), tuples_we_got.end(), tuples_we_want.begin(), tuples_we_want.end());
            }

            {
                ArrayPartitionIO partition_io{partition_shape, {}};
                partition_io.add_output_cell(output_cell, {-1, 0}, value);

                Indices const input_cell_we_want{2, 4};

                PartitionOffsets const partition_offsets_we_got{partition_io.partition_offsets()};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    partition_offsets_we_got.begin(),
                    partition_offsets_we_got.end(),
                    partition_offsets_we_want.begin(),
                    partition_offsets_we_want.end());

                Tuples const tuples_we_want{{input_cell_we_want, value}};
                Tuples const tuples_we_got{
                    partition_io.drain(*partition_offsets_we_want.begin(), north_partition_shape)};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    tuples_we_got.begin(), tuples_we_got.end(), tuples_we_want.begin(), tuples_we_want.end());
            }
        }
    }

    {
        // downstream is east
        Shape const east_partition_shape{{5, 3}};
        PartitionOffsets const partition_offsets_we_want{{0, 1}};

        {
            Indices output_cell{0, 4};

            {
                ArrayPartitionIO partition_io{partition_shape, {}};
                partition_io.add_output_cell(output_cell, {0, 1}, value);

                Indices const input_cell_we_want{0, 0};

                PartitionOffsets const partition_offsets_we_got{partition_io.partition_offsets()};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    partition_offsets_we_got.begin(),
                    partition_offsets_we_got.end(),
                    partition_offsets_we_want.begin(),
                    partition_offsets_we_want.end());

                Tuples const tuples_we_want{{input_cell_we_want, value}};
                Tuples const tuples_we_got{
                    partition_io.drain(*partition_offsets_we_want.begin(), east_partition_shape)};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    tuples_we_got.begin(), tuples_we_got.end(), tuples_we_want.begin(), tuples_we_want.end());
            }

            {
                ArrayPartitionIO partition_io{partition_shape, {}};
                partition_io.add_output_cell(output_cell, {1, 1}, value);

                Indices const input_cell_we_want{1, 0};

                PartitionOffsets const partition_offsets_we_got{partition_io.partition_offsets()};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    partition_offsets_we_got.begin(),
                    partition_offsets_we_got.end(),
                    partition_offsets_we_want.begin(),
                    partition_offsets_we_want.end());

                Tuples const tuples_we_want{{input_cell_we_want, value}};
                Tuples const tuples_we_got{
                    partition_io.drain(*partition_offsets_we_want.begin(), east_partition_shape)};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    tuples_we_got.begin(), tuples_we_got.end(), tuples_we_want.begin(), tuples_we_want.end());
            }
        }

        {
            Indices output_cell{2, 4};

            {
                ArrayPartitionIO partition_io{partition_shape, {}};
                partition_io.add_output_cell(output_cell, {-1, 1}, value);

                Indices const input_cell_we_want{1, 0};

                PartitionOffsets const partition_offsets_we_got{partition_io.partition_offsets()};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    partition_offsets_we_got.begin(),
                    partition_offsets_we_got.end(),
                    partition_offsets_we_want.begin(),
                    partition_offsets_we_want.end());

                Tuples const tuples_we_want{{input_cell_we_want, value}};
                Tuples const tuples_we_got{
                    partition_io.drain(*partition_offsets_we_want.begin(), east_partition_shape)};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    tuples_we_got.begin(), tuples_we_got.end(), tuples_we_want.begin(), tuples_we_want.end());
            }

            {
                ArrayPartitionIO partition_io{partition_shape, {}};
                partition_io.add_output_cell(output_cell, {0, 1}, value);

                Indices const input_cell_we_want{2, 0};

                PartitionOffsets const partition_offsets_we_got{partition_io.partition_offsets()};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    partition_offsets_we_got.begin(),
                    partition_offsets_we_got.end(),
                    partition_offsets_we_want.begin(),
                    partition_offsets_we_want.end());

                Tuples const tuples_we_want{{input_cell_we_want, value}};
                Tuples const tuples_we_got{
                    partition_io.drain(*partition_offsets_we_want.begin(), east_partition_shape)};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    tuples_we_got.begin(), tuples_we_got.end(), tuples_we_want.begin(), tuples_we_want.end());
            }

            {
                ArrayPartitionIO partition_io{partition_shape, {}};
                partition_io.add_output_cell(output_cell, {1, 1}, value);

                Indices const input_cell_we_want{3, 0};

                PartitionOffsets const partition_offsets_we_got{partition_io.partition_offsets()};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    partition_offsets_we_got.begin(),
                    partition_offsets_we_got.end(),
                    partition_offsets_we_want.begin(),
                    partition_offsets_we_want.end());

                Tuples const tuples_we_want{{input_cell_we_want, value}};
                Tuples const tuples_we_got{
                    partition_io.drain(*partition_offsets_we_want.begin(), east_partition_shape)};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    tuples_we_got.begin(), tuples_we_got.end(), tuples_we_want.begin(), tuples_we_want.end());
            }
        }

        {
            Indices output_cell{4, 4};

            {
                ArrayPartitionIO partition_io{partition_shape, {}};
                partition_io.add_output_cell(output_cell, {-1, 1}, value);

                Indices const input_cell_we_want{3, 0};

                PartitionOffsets const partition_offsets_we_got{partition_io.partition_offsets()};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    partition_offsets_we_got.begin(),
                    partition_offsets_we_got.end(),
                    partition_offsets_we_want.begin(),
                    partition_offsets_we_want.end());

                Tuples const tuples_we_want{{input_cell_we_want, value}};
                Tuples const tuples_we_got{
                    partition_io.drain(*partition_offsets_we_want.begin(), east_partition_shape)};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    tuples_we_got.begin(), tuples_we_got.end(), tuples_we_want.begin(), tuples_we_want.end());
            }

            {
                ArrayPartitionIO partition_io{partition_shape, {}};
                partition_io.add_output_cell(output_cell, {0, 1}, value);

                Indices const input_cell_we_want{4, 0};

                PartitionOffsets const partition_offsets_we_got{partition_io.partition_offsets()};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    partition_offsets_we_got.begin(),
                    partition_offsets_we_got.end(),
                    partition_offsets_we_want.begin(),
                    partition_offsets_we_want.end());

                Tuples const tuples_we_want{{input_cell_we_want, value}};
                Tuples const tuples_we_got{
                    partition_io.drain(*partition_offsets_we_want.begin(), east_partition_shape)};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    tuples_we_got.begin(), tuples_we_got.end(), tuples_we_want.begin(), tuples_we_want.end());
            }
        }
    }

    {
        // downstream is south
        Shape const south_partition_shape{{3, 5}};
        PartitionOffsets const partition_offsets_we_want{{1, 0}};

        {
            Indices output_cell{4, 0};

            {
                ArrayPartitionIO partition_io{partition_shape, {}};
                partition_io.add_output_cell(output_cell, {1, 0}, value);

                Indices const input_cell_we_want{0, 0};

                PartitionOffsets const partition_offsets_we_got{partition_io.partition_offsets()};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    partition_offsets_we_got.begin(),
                    partition_offsets_we_got.end(),
                    partition_offsets_we_want.begin(),
                    partition_offsets_we_want.end());

                Tuples const tuples_we_want{{input_cell_we_want, value}};
                Tuples const tuples_we_got{
                    partition_io.drain(*partition_offsets_we_want.begin(), south_partition_shape)};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    tuples_we_got.begin(), tuples_we_got.end(), tuples_we_want.begin(), tuples_we_want.end());
            }

            {
                ArrayPartitionIO partition_io{partition_shape, {}};
                partition_io.add_output_cell(output_cell, {1, 1}, value);

                Indices const input_cell_we_want{0, 1};

                PartitionOffsets const partition_offsets_we_got{partition_io.partition_offsets()};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    partition_offsets_we_got.begin(),
                    partition_offsets_we_got.end(),
                    partition_offsets_we_want.begin(),
                    partition_offsets_we_want.end());

                Tuples const tuples_we_want{{input_cell_we_want, value}};
                Tuples const tuples_we_got{
                    partition_io.drain(*partition_offsets_we_want.begin(), south_partition_shape)};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    tuples_we_got.begin(), tuples_we_got.end(), tuples_we_want.begin(), tuples_we_want.end());
            }
        }

        {
            Indices output_cell{4, 2};

            {
                ArrayPartitionIO partition_io{partition_shape, {}};
                partition_io.add_output_cell(output_cell, {1, -1}, value);

                Indices const input_cell_we_want{0, 1};

                PartitionOffsets const partition_offsets_we_got{partition_io.partition_offsets()};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    partition_offsets_we_got.begin(),
                    partition_offsets_we_got.end(),
                    partition_offsets_we_want.begin(),
                    partition_offsets_we_want.end());

                Tuples const tuples_we_want{{input_cell_we_want, value}};
                Tuples const tuples_we_got{
                    partition_io.drain(*partition_offsets_we_want.begin(), south_partition_shape)};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    tuples_we_got.begin(), tuples_we_got.end(), tuples_we_want.begin(), tuples_we_want.end());
            }

            {
                ArrayPartitionIO partition_io{partition_shape, {}};
                partition_io.add_output_cell(output_cell, {1, 0}, value);

                Indices const input_cell_we_want{0, 2};

                PartitionOffsets const partition_offsets_we_got{partition_io.partition_offsets()};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    partition_offsets_we_got.begin(),
                    partition_offsets_we_got.end(),
                    partition_offsets_we_want.begin(),
                    partition_offsets_we_want.end());

                Tuples const tuples_we_want{{input_cell_we_want, value}};
                Tuples const tuples_we_got{
                    partition_io.drain(*partition_offsets_we_want.begin(), south_partition_shape)};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    tuples_we_got.begin(), tuples_we_got.end(), tuples_we_want.begin(), tuples_we_want.end());
            }

            {
                ArrayPartitionIO partition_io{partition_shape, {}};
                partition_io.add_output_cell(output_cell, {1, 1}, value);

                Indices const input_cell_we_want{0, 3};

                PartitionOffsets const partition_offsets_we_got{partition_io.partition_offsets()};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    partition_offsets_we_got.begin(),
                    partition_offsets_we_got.end(),
                    partition_offsets_we_want.begin(),
                    partition_offsets_we_want.end());

                Tuples const tuples_we_want{{input_cell_we_want, value}};
                Tuples const tuples_we_got{
                    partition_io.drain(*partition_offsets_we_want.begin(), south_partition_shape)};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    tuples_we_got.begin(), tuples_we_got.end(), tuples_we_want.begin(), tuples_we_want.end());
            }
        }

        {
            Indices output_cell{4, 4};

            {
                ArrayPartitionIO partition_io{partition_shape, {}};
                partition_io.add_output_cell(output_cell, {1, -1}, value);

                Indices const input_cell_we_want{0, 3};

                PartitionOffsets const partition_offsets_we_got{partition_io.partition_offsets()};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    partition_offsets_we_got.begin(),
                    partition_offsets_we_got.end(),
                    partition_offsets_we_want.begin(),
                    partition_offsets_we_want.end());

                Tuples const tuples_we_want{{input_cell_we_want, value}};
                Tuples const tuples_we_got{
                    partition_io.drain(*partition_offsets_we_want.begin(), south_partition_shape)};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    tuples_we_got.begin(), tuples_we_got.end(), tuples_we_want.begin(), tuples_we_want.end());
            }

            {
                ArrayPartitionIO partition_io{partition_shape, {}};
                partition_io.add_output_cell(output_cell, {1, 0}, value);

                Indices const input_cell_we_want{0, 4};

                PartitionOffsets const partition_offsets_we_got{partition_io.partition_offsets()};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    partition_offsets_we_got.begin(),
                    partition_offsets_we_got.end(),
                    partition_offsets_we_want.begin(),
                    partition_offsets_we_want.end());

                Tuples const tuples_we_want{{input_cell_we_want, value}};
                Tuples const tuples_we_got{
                    partition_io.drain(*partition_offsets_we_want.begin(), south_partition_shape)};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    tuples_we_got.begin(), tuples_we_got.end(), tuples_we_want.begin(), tuples_we_want.end());
            }
        }
    }

    {
        // downstream is west
        Shape const west_partition_shape{{5, 3}};
        PartitionOffsets const partition_offsets_we_want{{0, -1}};

        {
            Indices output_cell{0, 0};

            {
                ArrayPartitionIO partition_io{partition_shape, {}};
                partition_io.add_output_cell(output_cell, {0, -1}, value);

                Indices const input_cell_we_want{0, 2};

                PartitionOffsets const partition_offsets_we_got{partition_io.partition_offsets()};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    partition_offsets_we_got.begin(),
                    partition_offsets_we_got.end(),
                    partition_offsets_we_want.begin(),
                    partition_offsets_we_want.end());

                Tuples const tuples_we_want{{input_cell_we_want, value}};
                Tuples const tuples_we_got{
                    partition_io.drain(*partition_offsets_we_want.begin(), west_partition_shape)};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    tuples_we_got.begin(), tuples_we_got.end(), tuples_we_want.begin(), tuples_we_want.end());
            }

            {
                ArrayPartitionIO partition_io{partition_shape, {}};
                partition_io.add_output_cell(output_cell, {1, -1}, value);

                Indices const input_cell_we_want{1, 2};

                PartitionOffsets const partition_offsets_we_got{partition_io.partition_offsets()};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    partition_offsets_we_got.begin(),
                    partition_offsets_we_got.end(),
                    partition_offsets_we_want.begin(),
                    partition_offsets_we_want.end());

                Tuples const tuples_we_want{{input_cell_we_want, value}};
                Tuples const tuples_we_got{
                    partition_io.drain(*partition_offsets_we_want.begin(), west_partition_shape)};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    tuples_we_got.begin(), tuples_we_got.end(), tuples_we_want.begin(), tuples_we_want.end());
            }
        }

        {
            Indices output_cell{2, 0};

            {
                ArrayPartitionIO partition_io{partition_shape, {}};
                partition_io.add_output_cell(output_cell, {-1, -1}, value);

                Indices const input_cell_we_want{1, 2};

                PartitionOffsets const partition_offsets_we_got{partition_io.partition_offsets()};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    partition_offsets_we_got.begin(),
                    partition_offsets_we_got.end(),
                    partition_offsets_we_want.begin(),
                    partition_offsets_we_want.end());

                Tuples const tuples_we_want{{input_cell_we_want, value}};
                Tuples const tuples_we_got{
                    partition_io.drain(*partition_offsets_we_want.begin(), west_partition_shape)};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    tuples_we_got.begin(), tuples_we_got.end(), tuples_we_want.begin(), tuples_we_want.end());
            }

            {
                ArrayPartitionIO partition_io{partition_shape, {}};
                partition_io.add_output_cell(output_cell, {0, -1}, value);

                Indices const input_cell_we_want{2, 2};

                PartitionOffsets const partition_offsets_we_got{partition_io.partition_offsets()};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    partition_offsets_we_got.begin(),
                    partition_offsets_we_got.end(),
                    partition_offsets_we_want.begin(),
                    partition_offsets_we_want.end());

                Tuples const tuples_we_want{{input_cell_we_want, value}};
                Tuples const tuples_we_got{
                    partition_io.drain(*partition_offsets_we_want.begin(), west_partition_shape)};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    tuples_we_got.begin(), tuples_we_got.end(), tuples_we_want.begin(), tuples_we_want.end());
            }

            {
                ArrayPartitionIO partition_io{partition_shape, {}};
                partition_io.add_output_cell(output_cell, {1, -1}, value);

                Indices const input_cell_we_want{3, 2};

                PartitionOffsets const partition_offsets_we_got{partition_io.partition_offsets()};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    partition_offsets_we_got.begin(),
                    partition_offsets_we_got.end(),
                    partition_offsets_we_want.begin(),
                    partition_offsets_we_want.end());

                Tuples const tuples_we_want{{input_cell_we_want, value}};
                Tuples const tuples_we_got{
                    partition_io.drain(*partition_offsets_we_want.begin(), west_partition_shape)};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    tuples_we_got.begin(), tuples_we_got.end(), tuples_we_want.begin(), tuples_we_want.end());
            }
        }

        {
            Indices output_cell{4, 0};

            {
                ArrayPartitionIO partition_io{partition_shape, {}};
                partition_io.add_output_cell(output_cell, {-1, -1}, value);

                Indices const input_cell_we_want{3, 2};

                PartitionOffsets const partition_offsets_we_got{partition_io.partition_offsets()};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    partition_offsets_we_got.begin(),
                    partition_offsets_we_got.end(),
                    partition_offsets_we_want.begin(),
                    partition_offsets_we_want.end());

                Tuples const tuples_we_want{{input_cell_we_want, value}};
                Tuples const tuples_we_got{
                    partition_io.drain(*partition_offsets_we_want.begin(), west_partition_shape)};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    tuples_we_got.begin(), tuples_we_got.end(), tuples_we_want.begin(), tuples_we_want.end());
            }

            {
                ArrayPartitionIO partition_io{partition_shape, {}};
                partition_io.add_output_cell(output_cell, {0, -1}, value);

                Indices const input_cell_we_want{4, 2};

                PartitionOffsets const partition_offsets_we_got{partition_io.partition_offsets()};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    partition_offsets_we_got.begin(),
                    partition_offsets_we_got.end(),
                    partition_offsets_we_want.begin(),
                    partition_offsets_we_want.end());

                Tuples const tuples_we_want{{input_cell_we_want, value}};
                Tuples const tuples_we_got{
                    partition_io.drain(*partition_offsets_we_want.begin(), west_partition_shape)};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    tuples_we_got.begin(), tuples_we_got.end(), tuples_we_want.begin(), tuples_we_want.end());
            }
        }
    }

    {
        // downstream is north-east
        Shape const north_east_partition_shape{{2, 2}};
        PartitionOffsets const partition_offsets_we_want{{-1, 1}};

        {
            Indices output_cell{0, 4};

            {
                ArrayPartitionIO partition_io{partition_shape, {}};
                partition_io.add_output_cell(output_cell, {-1, 1}, value);

                Indices const input_cell_we_want{1, 0};

                PartitionOffsets const partition_offsets_we_got{partition_io.partition_offsets()};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    partition_offsets_we_got.begin(),
                    partition_offsets_we_got.end(),
                    partition_offsets_we_want.begin(),
                    partition_offsets_we_want.end());

                Tuples const tuples_we_want{{input_cell_we_want, value}};
                Tuples const tuples_we_got{
                    partition_io.drain(*partition_offsets_we_want.begin(), north_east_partition_shape)};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    tuples_we_got.begin(), tuples_we_got.end(), tuples_we_want.begin(), tuples_we_want.end());
            }
        }
    }

    {
        // downstream is south-east
        Shape const south_east_partition_shape{{2, 2}};
        PartitionOffsets const partition_offsets_we_want{{1, 1}};

        {
            Indices output_cell{4, 4};

            {
                ArrayPartitionIO partition_io{partition_shape, {}};
                partition_io.add_output_cell(output_cell, {1, 1}, value);

                Indices const input_cell_we_want{0, 0};

                PartitionOffsets const partition_offsets_we_got{partition_io.partition_offsets()};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    partition_offsets_we_got.begin(),
                    partition_offsets_we_got.end(),
                    partition_offsets_we_want.begin(),
                    partition_offsets_we_want.end());

                Tuples const tuples_we_want{{input_cell_we_want, value}};
                Tuples const tuples_we_got{
                    partition_io.drain(*partition_offsets_we_want.begin(), south_east_partition_shape)};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    tuples_we_got.begin(), tuples_we_got.end(), tuples_we_want.begin(), tuples_we_want.end());
            }
        }
    }

    {
        // downstream is south-west
        Shape const south_west_partition_shape{{2, 2}};
        PartitionOffsets const partition_offsets_we_want{{1, -1}};

        {
            Indices output_cell{4, 0};

            {
                ArrayPartitionIO partition_io{partition_shape, {}};
                partition_io.add_output_cell(output_cell, {1, -1}, value);

                Indices const input_cell_we_want{0, 1};

                PartitionOffsets const partition_offsets_we_got{partition_io.partition_offsets()};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    partition_offsets_we_got.begin(),
                    partition_offsets_we_got.end(),
                    partition_offsets_we_want.begin(),
                    partition_offsets_we_want.end());

                Tuples const tuples_we_want{{input_cell_we_want, value}};
                Tuples const tuples_we_got{
                    partition_io.drain(*partition_offsets_we_want.begin(), south_west_partition_shape)};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    tuples_we_got.begin(), tuples_we_got.end(), tuples_we_want.begin(), tuples_we_want.end());
            }
        }
    }

    {
        // downstream is north-west
        Shape const north_west_partition_shape{{2, 2}};
        PartitionOffsets const partition_offsets_we_want{{-1, -1}};

        {
            Indices output_cell{0, 0};

            {
                ArrayPartitionIO partition_io{partition_shape, {}};
                partition_io.add_output_cell(output_cell, {-1, -1}, value);

                Indices const input_cell_we_want{1, 1};

                PartitionOffsets const partition_offsets_we_got{partition_io.partition_offsets()};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    partition_offsets_we_got.begin(),
                    partition_offsets_we_got.end(),
                    partition_offsets_we_want.begin(),
                    partition_offsets_we_want.end());

                Tuples const tuples_we_want{{input_cell_we_want, value}};
                Tuples const tuples_we_got{
                    partition_io.drain(*partition_offsets_we_want.begin(), north_west_partition_shape)};
                BOOST_CHECK_EQUAL_COLLECTIONS(
                    tuples_we_got.begin(), tuples_we_got.end(), tuples_we_want.begin(), tuples_we_want.end());
            }
        }
    }
}


BOOST_AUTO_TEST_CASE(partition_offsets)
{
    // Check whether calculations for offsets of neighbouring partitions
    // containing destination input cells are correct

    // Given an 5x5 partition, put an output cell at various locations
    // along the border

    using Value = std::int32_t;
    using ArrayPartitionIO = lue::detail::ArrayPartitionIO<lue::Index, 2, Value>;
    using Shape = typename ArrayPartitionIO::Shape;
    using PartitionOffsets = ArrayPartitionIO::PartitionOffsets;

    Shape const partition_shape{5, 5};
    ArrayPartitionIO partition_io{partition_shape, {}};

    partition_io.add_output_cell({0, 0}, {-1, 0}, Value{});
    partition_io.add_output_cell({0, 2}, {-1, 1}, Value{});
    partition_io.add_output_cell({0, 4}, {-1, 1}, Value{});
    partition_io.add_output_cell({1, 4}, {-1, 1}, Value{});
    partition_io.add_output_cell({4, 4}, {1, 1}, Value{});
    partition_io.add_output_cell({4, 1}, {1, 0}, Value{});
    partition_io.add_output_cell({4, 0}, {1, -1}, Value{});
    partition_io.add_output_cell({2, 0}, {1, -1}, Value{});

    PartitionOffsets partition_offsets_we_want{{-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
    PartitionOffsets partition_offsets_we_got{partition_io.partition_offsets()};

    BOOST_CHECK_EQUAL_COLLECTIONS(
        partition_offsets_we_got.begin(),
        partition_offsets_we_got.end(),
        partition_offsets_we_want.begin(),
        partition_offsets_we_want.end());
}
