#define BOOST_TEST_MODULE lue framework algorithm highest_n
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/value_policies/decreasing_order.hpp"
#include "lue/framework/algorithm/value_policies/highest_n.hpp"
#include "lue/framework/core/component.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/stream.hpp"


namespace {

    lue::Rank const rank{2};

    template<typename Element>
    using Array = lue::PartitionedArray<Element, rank>;

    using Offset = lue::Offset<lue::Index, rank>;

    using Shape = lue::Shape<lue::Count, rank>;  // = typename Array<std::int32_t>::Shape;

}  // Anonymous namespace


// BOOST_AUTO_TEST_CASE(empty_array)
// {
//     // Create empty array. Verify this works and that the result is also an empty array.
//     using namespace lue::value_policies;
//
//     using InputElement = float;
//     using OutputElement = std::uint8_t;
//
//     Shape const array_shape{};
//     lue::Count const nr_elements{0};
//
//     Array<InputElement> input_array = lue::create_partitioned_array<InputElement>(array_shape);
//     Array<OutputElement> output_array = highest_n<OutputElement>(input_array, nr_elements);
//
//     BOOST_CHECK_EQUAL(output_array.shape(), array_shape);
// }


BOOST_AUTO_TEST_CASE(use_case_01)
{
    // Partitions:
    // - 3x3 cells

    // Value array:
    // - 9x9 cells
    // - Values increase in size, going from left to right (cols) and top to bottom (rows)
    // - A few no-data cells

    // +----+----+----+  +----+----+----+  +----+----+----+
    // | ND |  2 |  3 |  |  4 |  5 |  6 |  |  7 |  8 |  9 |
    // +----+----+----+  +----+----+----+  +----+----+----+
    // | 10 | 11 | 12 |  | 13 | 14 | 15 |  | 16 | 17 | 18 |
    // +----+----+----+  +----+----+----+  +----+----+----+
    // | 19 | 20 | 21 |  | 22 | 23 | 24 |  | 25 | 26 | 27 |
    // +----+----+----+  +----+----+----+  +----+----+----+
    //
    // +----+----+----+  +----+----+----+  +----+----+----+
    // | 28 | 29 | 30 |  | ND | 32 | 33 |  | 34 | 35 | 36 |
    // +----+----+----+  +----+----+----+  +----+----+----+
    // | 37 | 38 | 39 |  | 40 | 41 | 42 |  | 43 | ND | 45 |
    // +----+----+----+  +----+----+----+  +----+----+----+
    // | 46 | 47 | 48 |  | 49 | 50 | 51 |  | 52 | 53 | 54 |
    // +----+----+----+  +----+----+----+  +----+----+----+
    //
    // +----+----+----+  +----+----+----+  +----+----+----+
    // | 55 | 56 | ND |  | 58 | 59 | 60 |  | 61 | 62 | 63 |
    // +----+----+----+  +----+----+----+  +----+----+----+
    // | 64 | 65 | 66 |  | 67 | 68 | 69 |  | 70 | 71 | 72 |
    // +----+----+----+  +----+----+----+  +----+----+----+
    // | 73 | 74 | 75 |  | 76 | 77 | 78 |  | 79 | 80 | ND |
    // +----+----+----+  +----+----+----+  +----+----+----+

    // Zones:
    // - 3 zones:
    //     - Zone 1: top left 4x4 cells
    //     - Zone 2: top right 3x5 cells
    //     - Zone 3: all other cells
    // - A few no-data cells

    // +----+----+----+  +----+----+----+  +----+----+----+
    // |  1 |  1 |  1 |  |  1 |  2 |  2 |  |  2 |  2 | ND |
    // +----+----+----+  +----+----+----+  +----+----+----+
    // |  1 |  1 |  1 |  |  1 | ND |  2 |  |  2 |  2 |  2 |
    // +----+----+----+  +----+----+----+  +----+----+----+
    // |  1 |  1 | ND |  |  1 |  2 |  2 |  |  2 |  2 |  2 |
    // +----+----+----+  +----+----+----+  +----+----+----+
    //
    // +----+----+----+  +----+----+----+  +----+----+----+
    // |  1 |  1 |  1 |  |  1 |  2 |  2 |  |  2 |  2 |  2 |
    // +----+----+----+  +----+----+----+  +----+----+----+
    // | ND |  3 |  3 |  |  3 |  2 |  2 |  |  2 |  2 |  2 |
    // +----+----+----+  +----+----+----+  +----+----+----+
    // |  3 |  3 |  3 |  |  3 |  3 |  3 |  |  3 |  3 |  3 |
    // +----+----+----+  +----+----+----+  +----+----+----+
    //
    // +----+----+----+  +----+----+----+  +----+----+----+
    // |  3 |  3 | ND |  |  3 |  3 |  3 |  |  3 |  3 |  3 |
    // +----+----+----+  +----+----+----+  +----+----+----+
    // |  3 |  3 |  3 |  |  3 |  3 |  3 |  |  3 |  3 |  3 |
    // +----+----+----+  +----+----+----+  +----+----+----+
    // |  3 |  3 |  3 |  |  3 |  3 |  3 |  | ND |  3 |  3 |
    // +----+----+----+  +----+----+----+  +----+----+----+

    // result_we_want:

    // +----+----+----+  +----+----+----+  +----+----+----+
    // | ND | ND | ND |  | ND | ND | ND |  | ND | ND | ND |
    // +----+----+----+  +----+----+----+  +----+----+----+
    // |  T |  T |  T |  |  T | ND | ND |  | ND | ND | ND |
    // +----+----+----+  +----+----+----+  +----+----+----+
    // |  T |  T | ND |  |  T | ND | ND |  | ND | ND |  T |
    // +----+----+----+  +----+----+----+  +----+----+----+
    //
    // +----+----+----+  +----+----+----+  +----+----+----+
    // |  T |  T |  T |  | ND |  T |  T |  |  T |  T |  T |
    // +----+----+----+  +----+----+----+  +----+----+----+
    // | ND | ND | ND |  | ND |  T |  T |  |  T | ND |  T |
    // +----+----+----+  +----+----+----+  +----+----+----+
    // | ND | ND | ND |  | ND | ND | ND |  | ND | ND | ND |
    // +----+----+----+  +----+----+----+  +----+----+----+
    //
    // +----+----+----+  +----+----+----+  +----+----+----+
    // | ND | ND | ND |  | ND | ND | ND |  | ND | ND | ND |
    // +----+----+----+  +----+----+----+  +----+----+----+
    // | ND | ND | ND |  | ND | ND | ND |  |  T |  T |  T |
    // +----+----+----+  +----+----+----+  +----+----+----+
    // |  T |  T |  T |  |  T |  T |  T |  | ND |  T | ND |
    // +----+----+----+  +----+----+----+  +----+----+----+

    lue::Rank const rank{2};

    using ZoneElement = std::uint32_t;
    using ValueElement = double;
    using ResultElement = std::uint8_t;
    using Shape = lue::Shape<lue::Count, rank>;
    using ValueArray = lue::PartitionedArray<ValueElement, rank>;
    using ZoneArray = lue::PartitionedArray<ZoneElement, rank>;
    using ResultArray = lue::PartitionedArray<ResultElement, rank>;

    ValueElement const x1{lue::policy::no_data_value<ValueElement>};
    ZoneElement const x2{lue::policy::no_data_value<ZoneElement>};
    Shape const array_shape{9, 9};
    Shape const partition_shape{3, 3};
    lue::Count const nr_zones{3};
    lue::Count const nr_cells{10};

    // clang-format off
    ValueArray const value_array = lue::test::create_partitioned_array<ValueArray>(
        array_shape,
        partition_shape,
        {
            {
                x1,  2,  3,
                10, 11, 12,
                19, 20, 21,
            },
            {
                 4,  5,  6,
                13, 14, 15,
                22, 23, 24,
            },
            {
                 7,  8,  9,
                16, 17, 18,
                25, 26, 27,
            },
            {
                28, 29, 30,
                37, 38, 39,
                46, 47, 48,
            },
            {
                x1, 32, 33,
                40, 41, 42,
                49, 50, 51,
            },
            {
                34, 35, 36,
                43, x1, 45,
                52, 53, 54,
            },
            {
                55, 56, x1,
                64, 65, 66,
                73, 74, 75,
            },
            {
                58, 59, 60,
                67, 68, 69,
                76, 77, 78,
            },
            {
                61, 62, 63,
                70, 71, 72,
                79, 80, x1,
            },
        });

    ZoneArray const zone_array = lue::test::create_partitioned_array<ZoneArray>(
        array_shape,
        partition_shape,
        {
            {
                 1,  1,  1,
                 1,  1,  1,
                 1,  1, x2,
            },
            {
                 1,  2,  2,
                 1, x2,  2,
                 1,  2,  2,
            },
            {
                 2,  2, x2,
                 2,  2,  2,
                 2,  2,  2,
            },
            {
                 1,  1,  1,
                x2,  3,  3,
                 3,  3,  3,
            },
            {
                 1,  2,  2,
                 3,  2,  2,
                 3,  3,  3,
            },
            {
                 2,  2,  2,
                 2,  2,  2,
                 3,  3,  3,
            },
            {
                 3,  3, x2,
                 3,  3,  3,
                 3,  3,  3,
            },
            {
                 3,  3,  3,
                 3,  3,  3,
                 3,  3,  3,
            },
            {
                 3,  3,  3,
                 3,  3,  3,
                x2,  3,  3,
            },
        });
    // clang-format on

    ResultArray const result_we_got =
        lue::value_policies::highest_n<ResultElement>(zone_array, value_array, nr_cells);

    auto const& partitions = result_we_got.partitions();

    BOOST_REQUIRE_EQUAL(lue::nr_elements(partitions.shape()), 9);
    BOOST_REQUIRE(lue::all_are_valid(partitions));

    hpx::wait_all(partitions.begin(), partitions.end());

    // partition(0, 0)
    {
        auto const& partition_we_got = partitions(0, 0);
        auto const offset_we_got = partition_we_got.offset().get();
        auto const shape_we_got = partition_we_got.shape().get();

        Offset const offset_we_want{0, 0};
        BOOST_CHECK_EQUAL(offset_we_got, offset_we_want);

        Shape const shape_we_want{3, 3};
        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);

        auto const data_we_got = partition_we_got.data();


        // ResultElement const true_value{1};
        // ResultElement const nodata{1};

        // BOOST_CHECK_EQUAL(data(0, 0), true_value);


        ///     // fragments 1
        ///     {
        ///         auto const route_fragments_we_got = partition_we_got.route_fragments(1).get();

        ///         BOOST_REQUIRE_EQUAL(std::size(route_fragments_we_got), 3);

        ///         {
        ///             auto const& route_fragment = route_fragments_we_got[0];

        ///             BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 2);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 7);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 6);
        ///             BOOST_CHECK(!route_fragment.is_last());
        ///         }

        ///         {
        ///             auto const& route_fragment = route_fragments_we_got[1];

        ///             BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 3);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 5);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 4);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 3);
        ///             BOOST_CHECK(!route_fragment.is_last());
        ///         }

        ///         {
        ///             auto const& route_fragment = route_fragments_we_got[2];

        ///             BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 2);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 2);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 1);
        ///             BOOST_CHECK(route_fragment.is_last());
        ///         }
        ///     }
    }

    // partition(0, 1)
    {
        auto const& partition_we_got = partitions(0, 1);
        auto const offset_we_got = partition_we_got.offset().get();
        auto const shape_we_got = partition_we_got.shape().get();

        Offset const offset_we_want{0, 3};
        BOOST_CHECK_EQUAL(offset_we_got, offset_we_want);

        Shape const shape_we_want{3, 3};
        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);

        ///     // fragments 1
        ///     {
        ///         auto const route_fragments_we_got = partition_we_got.route_fragments(1).get();

        ///         BOOST_REQUIRE_EQUAL(std::size(route_fragments_we_got), 3);

        ///         {
        ///             auto const& route_fragment = route_fragments_we_got[0];

        ///             BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 1);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 6);
        ///             BOOST_CHECK(!route_fragment.is_last());
        ///         }

        ///         {
        ///             auto const& route_fragment = route_fragments_we_got[1];

        ///             BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 1);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 3);
        ///             BOOST_CHECK(!route_fragment.is_last());
        ///         }

        ///         {
        ///             auto const& route_fragment = route_fragments_we_got[2];

        ///             BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 1);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 0);
        ///             BOOST_CHECK(!route_fragment.is_last());
        ///         }
        ///     }

        ///     // fragments 2
        ///     {
        ///         auto const route_fragments_we_got = partition_we_got.route_fragments(2).get();

        ///         BOOST_REQUIRE_EQUAL(std::size(route_fragments_we_got), 3);

        ///         {
        ///             auto const& route_fragment = route_fragments_we_got[0];

        ///             BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 2);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 8);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 7);
        ///             BOOST_CHECK(!route_fragment.is_last());
        ///         }

        ///         {
        ///             auto const& route_fragment = route_fragments_we_got[1];

        ///             BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 1);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 5);
        ///             BOOST_CHECK(!route_fragment.is_last());
        ///         }

        ///         {
        ///             auto const& route_fragment = route_fragments_we_got[2];

        ///             BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 2);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 2);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 1);
        ///             BOOST_CHECK(route_fragment.is_last());
        ///         }
        ///     }
    }

    // partition(0, 2)
    {
        auto const& partition_we_got = partitions(0, 2);
        auto const shape_we_got = partition_we_got.shape().get();
        auto const offset_we_got = partition_we_got.offset().get();

        Offset const offset_we_want{0, 6};
        BOOST_CHECK_EQUAL(offset_we_got, offset_we_want);

        Shape const shape_we_want{3, 3};
        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);

        ///     // fragments 1
        ///     {
        ///         auto const route_fragments_we_got = partition_we_got.route_fragments(2).get();

        ///         BOOST_REQUIRE_EQUAL(std::size(route_fragments_we_got), 3);

        ///         {
        ///             auto const& route_fragment = route_fragments_we_got[0];

        ///             BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 3);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 8);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 7);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 6);
        ///             BOOST_CHECK(!route_fragment.is_last());
        ///         }

        ///         {
        ///             auto const& route_fragment = route_fragments_we_got[1];

        ///             BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 3);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 5);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 4);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 3);
        ///             BOOST_CHECK(!route_fragment.is_last());
        ///         }

        ///         {
        ///             auto const& route_fragment = route_fragments_we_got[2];

        ///             BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 2);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 1);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 0);
        ///             BOOST_CHECK(!route_fragment.is_last());
        ///         }
        ///     }
    }

    // partition(1, 0)
    {
        auto const& partition_we_got = partitions(1, 0);
        auto const offset_we_got = partition_we_got.offset().get();
        auto const shape_we_got = partition_we_got.shape().get();

        Offset const offset_we_want{3, 0};
        BOOST_CHECK_EQUAL(offset_we_got, offset_we_want);

        Shape const shape_we_want{3, 3};
        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);

        ///     // fragments 1
        ///     {
        ///         auto const route_fragments_we_got = partition_we_got.route_fragments(1).get();

        ///         BOOST_REQUIRE_EQUAL(std::size(route_fragments_we_got), 1);

        ///         {
        ///             auto const& route_fragment = route_fragments_we_got[0];

        ///             BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 3);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 2);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 1);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 0);
        ///             BOOST_CHECK(!route_fragment.is_last());
        ///         }
        ///     }

        ///     // fragments 3
        ///     {
        ///         auto const route_fragments_we_got = partition_we_got.route_fragments(3).get();

        ///         BOOST_REQUIRE_EQUAL(std::size(route_fragments_we_got), 2);

        ///         {
        ///             auto const& route_fragment = route_fragments_we_got[0];

        ///             BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 3);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 8);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 7);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 6);
        ///             BOOST_CHECK(!route_fragment.is_last());
        ///         }

        ///         {
        ///             auto const& route_fragment = route_fragments_we_got[1];

        ///             BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 2);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 5);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 4);
        ///             BOOST_CHECK(route_fragment.is_last());
        ///         }
        ///     }
    }

    // partition(1, 1)
    {
        auto const& partition_we_got = partitions(1, 1);
        auto const offset_we_got = partition_we_got.offset().get();
        auto const shape_we_got = partition_we_got.shape().get();

        Offset const offset_we_want{3, 3};
        BOOST_CHECK_EQUAL(offset_we_got, offset_we_want);

        Shape const shape_we_want{3, 3};
        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);

        ///     // fragments 2
        ///     {
        ///         auto const route_fragments_we_got = partition_we_got.route_fragments(2).get();

        ///         BOOST_REQUIRE_EQUAL(std::size(route_fragments_we_got), 2);

        ///         {
        ///             auto const& route_fragment = route_fragments_we_got[0];

        ///             BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 2);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 5);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 4);
        ///             BOOST_CHECK(!route_fragment.is_last());
        ///         }

        ///         {
        ///             auto const& route_fragment = route_fragments_we_got[1];

        ///             BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 2);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 2);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 1);
        ///             BOOST_CHECK(!route_fragment.is_last());
        ///         }
        ///     }

        ///     // fragments 3
        ///     {
        ///         auto const route_fragments_we_got = partition_we_got.route_fragments(3).get();

        ///         BOOST_REQUIRE_EQUAL(std::size(route_fragments_we_got), 2);

        ///         {
        ///             auto const& route_fragment = route_fragments_we_got[0];

        ///             BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 3);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 8);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 7);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 6);
        ///             BOOST_CHECK(!route_fragment.is_last());
        ///         }

        ///         {
        ///             auto const& route_fragment = route_fragments_we_got[1];

        ///             BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 1);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 3);
        ///             BOOST_CHECK(!route_fragment.is_last());
        ///         }
        ///     }
    }

    // partition(1, 2)
    {
        auto const& partition_we_got = partitions(1, 2);
        auto const offset_we_got = partition_we_got.offset().get();
        auto const shape_we_got = partition_we_got.shape().get();

        Offset const offset_we_want{3, 6};
        BOOST_CHECK_EQUAL(offset_we_got, offset_we_want);

        Shape const shape_we_want{3, 3};
        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);

        ///     // fragments 2
        ///     {
        ///         auto const route_fragments_we_got = partition_we_got.route_fragments(2).get();

        ///         BOOST_REQUIRE_EQUAL(std::size(route_fragments_we_got), 2);

        ///         {
        ///             auto const& route_fragment = route_fragments_we_got[0];

        ///             BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 2);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 5);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 3);
        ///             BOOST_CHECK(!route_fragment.is_last());
        ///         }

        ///         {
        ///             auto const& route_fragment = route_fragments_we_got[1];

        ///             BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 3);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 2);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 1);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 0);
        ///             BOOST_CHECK(!route_fragment.is_last());
        ///         }
        ///     }

        ///     // fragments 3
        ///     {
        ///         auto const route_fragments_we_got = partition_we_got.route_fragments(3).get();

        ///         BOOST_REQUIRE_EQUAL(std::size(route_fragments_we_got), 1);

        ///         {
        ///             auto const& route_fragment = route_fragments_we_got[0];

        ///             BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 3);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 8);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 7);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 6);
        ///             BOOST_CHECK(!route_fragment.is_last());
        ///         }
        ///     }
    }

    // partition(2, 0)
    {
        auto const& partition_we_got = partitions(2, 0);
        auto const offset_we_got = partition_we_got.offset().get();
        auto const shape_we_got = partition_we_got.shape().get();

        Offset const offset_we_want{6, 0};
        BOOST_CHECK_EQUAL(offset_we_got, offset_we_want);

        Shape const shape_we_want{3, 3};
        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);

        ///     // fragments 3
        ///     {
        ///         auto const route_fragments_we_got = partition_we_got.route_fragments(3).get();

        ///         BOOST_REQUIRE_EQUAL(std::size(route_fragments_we_got), 3);

        ///         {
        ///             auto const& route_fragment = route_fragments_we_got[0];

        ///             BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 3);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 8);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 7);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 6);
        ///             BOOST_CHECK(!route_fragment.is_last());
        ///         }

        ///         {
        ///             auto const& route_fragment = route_fragments_we_got[1];

        ///             BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 3);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 5);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 4);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 3);
        ///             BOOST_CHECK(!route_fragment.is_last());
        ///         }

        ///         {
        ///             auto const& route_fragment = route_fragments_we_got[2];

        ///             BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 2);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 1);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 0);
        ///             BOOST_CHECK(!route_fragment.is_last());
        ///         }
        ///     }
    }

    // partition(2, 1)
    {
        auto const& partition_we_got = partitions(2, 1);
        auto const offset_we_got = partition_we_got.offset().get();
        auto const shape_we_got = partition_we_got.shape().get();

        Offset const offset_we_want{6, 3};
        BOOST_CHECK_EQUAL(offset_we_got, offset_we_want);

        Shape const shape_we_want{3, 3};
        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);

        ///     // fragments 3
        ///     {
        ///         auto const route_fragments_we_got = partition_we_got.route_fragments(3).get();

        ///         BOOST_REQUIRE_EQUAL(std::size(route_fragments_we_got), 3);

        ///         {
        ///             auto const& route_fragment = route_fragments_we_got[0];

        ///             BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 3);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 8);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 7);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 6);
        ///             BOOST_CHECK(!route_fragment.is_last());
        ///         }

        ///         {
        ///             auto const& route_fragment = route_fragments_we_got[1];

        ///             BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 3);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 5);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 4);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 3);
        ///             BOOST_CHECK(!route_fragment.is_last());
        ///         }

        ///         {
        ///             auto const& route_fragment = route_fragments_we_got[2];

        ///             BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 3);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 2);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 1);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 0);
        ///             BOOST_CHECK(!route_fragment.is_last());
        ///         }
        ///     }
    }

    // partition(2, 2)
    {
        auto const& partition_we_got = partitions(2, 2);
        auto const offset_we_got = partition_we_got.offset().get();
        auto const shape_we_got = partition_we_got.shape().get();

        Offset const offset_we_want{6, 6};
        BOOST_CHECK_EQUAL(offset_we_got, offset_we_want);

        Shape const shape_we_want{3, 3};
        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);

        ///     // fragments 3
        ///     {
        ///         auto const route_fragments_we_got = partition_we_got.route_fragments(3).get();

        ///         BOOST_REQUIRE_EQUAL(std::size(route_fragments_we_got), 3);

        ///         {
        ///             auto const& route_fragment = route_fragments_we_got[0];

        ///             BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 1);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 7);
        ///             BOOST_CHECK(!route_fragment.is_last());
        ///         }

        ///         {
        ///             auto const& route_fragment = route_fragments_we_got[1];

        ///             BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 3);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 5);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 4);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 3);
        ///             BOOST_CHECK(!route_fragment.is_last());
        ///         }

        ///         {
        ///             auto const& route_fragment = route_fragments_we_got[2];

        ///             BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 3);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 2);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 1);
        ///             BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 0);
        ///             BOOST_CHECK(!route_fragment.is_last());
        ///         }
        ///     }
    }
}
