#define BOOST_TEST_MODULE lue framework algorithm decreasing_order
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/algorithm/value_policies/decreasing_order.hpp"
#include "lue/framework/algorithm/value_policies/greater_than.hpp"
#include "lue/framework/algorithm/value_policies/uniform.hpp"
#include "lue/framework/algorithm/value_policies/where.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/stream.hpp"
#include <random>

#include <hpx/iostream.hpp>


namespace {

    lue::Rank const rank{2};

    template<typename Element>
    using Array = lue::PartitionedArray<Element, rank>;

    using Route = lue::SerialRoute<rank>;

    using Offset = lue::Offset<lue::Index, rank>;

    using Shape = lue::Shape<lue::Count, rank>;  // = typename Array<std::int32_t>::Shape;

}  // Anonymous namespace


// TODO Empty arrays are not supported yet (see domain_decomposition.hpp)
// BOOST_AUTO_TEST_CASE(empty_array)
// {
//     // Create empty array. Verify this works and that the result is also an empty route.
//     using namespace lue::value_policies;
//
//     using InputElement = float;
//
//     Shape const array_shape{0, 0};
//     lue::Count const nr_elements{0};
//
//     Array<InputElement> input_array = lue::create_partitioned_array<InputElement>(array_shape);
//     // Route route = decreasing_order(input_array);
//
//     // BOOST_CHECK_EQUAL(route.shape(), array_shape);
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

    // route_we_want:
    // - Per zone a network, connecting all non-no-data cells within the zone

    // +----+----+----+  +----+----+----+  +----+----+----+
    // | ND | 13 | 12 |  | 11 | 22 | 21 |  | 20 | 19 | ND |
    // +----+----+----+  +----+----+----+  +----+----+----+
    // | 10 |  9 |  8 |  |  7 | ND | 18 |  | 17 | 16 | 15 |
    // +----+----+----+  +----+----+----+  +----+----+----+
    // |  6 |  5 | ND |  |  4 | 14 | 13 |  | 12 | 11 | 10 |
    // +----+----+----+  +----+----+----+  +----+----+----+
    //
    // +----+----+----+  +----+----+----+  +----+----+----+
    // |  3 |  2 |  1 |  | ND |  9 |  8 |  |  7 |  6 |  5 |
    // +----+----+----+  +----+----+----+  +----+----+----+
    // | ND | 36 | 35 |  | 34 |  4 |  3 |  |  2 | ND |  1 |
    // +----+----+----+  +----+----+----+  +----+----+----+
    // | 33 | 32 | 31 |  | 30 | 29 | 28 |  | 27 | 26 | 25 |
    // +----+----+----+  +----+----+----+  +----+----+----+
    //
    // +----+----+----+  +----+----+----+  +----+----+----+
    // | 24 | 23 | ND |  | 22 | 21 | 20 |  | 19 | 18 | 17 |
    // +----+----+----+  +----+----+----+  +----+----+----+
    // | 16 | 15 | 14 |  | 13 | 12 | 11 |  | 10 |  9 |  8 |
    // +----+----+----+  +----+----+----+  +----+----+----+
    // |  7 |  6 |  5 |  |  4 |  3 |  2 |  | ND |  1 | ND |
    // +----+----+----+  +----+----+----+  +----+----+----+

    lue::Rank const rank{2};

    using ZoneElement = std::uint32_t;
    using ValueElement = double;
    using Shape = lue::Shape<lue::Count, rank>;
    using ValueArray = lue::PartitionedArray<ValueElement, rank>;
    using ZoneArray = lue::PartitionedArray<ZoneElement, rank>;

    ValueElement const x1{lue::policy::no_data_value<ValueElement>};
    ZoneElement const x2{lue::policy::no_data_value<ZoneElement>};
    Shape const array_shape{9, 9};
    Shape const partition_shape{3, 3};
    lue::Count const nr_zones{3};

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

    Route const route_we_got = lue::value_policies::decreasing_order(zone_array, value_array);

    route_we_got.wait();

    BOOST_CHECK_EQUAL(route_we_got.nr_routes(), nr_zones);

    auto const& partitions = route_we_got.partitions();

    BOOST_REQUIRE(lue::all_are_valid(partitions));

    // partition(0, 0)
    {
        auto const& partition_we_got = partitions(0, 0);
        partition_we_got.wait();

        auto const offset_we_got = partition_we_got.offset().get();
        auto const shape_we_got = partition_we_got.shape().get();
        auto const nr_routes_we_got = partition_we_got.nr_routes().get();
        auto const nr_route_fragments_we_got = partition_we_got.nr_route_fragments().get();
        auto const route_ids_we_got = partition_we_got.route_ids().get();

        Offset const offset_we_want{0, 0};
        BOOST_CHECK_EQUAL(offset_we_got, offset_we_want);

        Shape const shape_we_want{3, 3};
        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);

        BOOST_CHECK_EQUAL(nr_routes_we_got, 1);
        BOOST_REQUIRE_EQUAL(nr_route_fragments_we_got, 3);

        BOOST_REQUIRE_EQUAL(route_ids_we_got.size(), 1);
        BOOST_CHECK(route_ids_we_got.find(1) != route_ids_we_got.end());

        // fragments 1
        {
            auto const route_fragments_we_got = partition_we_got.route_fragments(1).get();

            BOOST_REQUIRE_EQUAL(std::size(route_fragments_we_got), 3);

            {
                auto const& route_fragment = route_fragments_we_got[0];

                BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 2);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 7);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 6);
                BOOST_CHECK(!route_fragment.is_last());
            }

            {
                auto const& route_fragment = route_fragments_we_got[1];

                BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 3);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 5);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 4);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 3);
                BOOST_CHECK(!route_fragment.is_last());
            }

            {
                auto const& route_fragment = route_fragments_we_got[2];

                BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 2);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 2);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 1);
                BOOST_CHECK(route_fragment.is_last());
            }
        }
    }

    // partition(0, 1)
    {
        auto const& partition_we_got = partitions(0, 1);
        partition_we_got.wait();

        auto const offset_we_got = partition_we_got.offset().get();
        auto const shape_we_got = partition_we_got.shape().get();
        auto const nr_routes_we_got = partition_we_got.nr_routes().get();
        auto const nr_route_fragments_we_got = partition_we_got.nr_route_fragments().get();
        auto const route_ids_we_got = partition_we_got.route_ids().get();

        Offset const offset_we_want{0, 3};
        BOOST_CHECK_EQUAL(offset_we_got, offset_we_want);

        Shape const shape_we_want{3, 3};
        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);

        BOOST_CHECK_EQUAL(nr_routes_we_got, 2);
        BOOST_REQUIRE_EQUAL(nr_route_fragments_we_got, 6);

        BOOST_REQUIRE_EQUAL(route_ids_we_got.size(), 2);
        BOOST_CHECK(route_ids_we_got.find(1) != route_ids_we_got.end());
        BOOST_CHECK(route_ids_we_got.find(2) != route_ids_we_got.end());

        // fragments 1
        {
            auto const route_fragments_we_got = partition_we_got.route_fragments(1).get();

            BOOST_REQUIRE_EQUAL(std::size(route_fragments_we_got), 3);

            {
                auto const& route_fragment = route_fragments_we_got[0];

                BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 1);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 6);
                BOOST_CHECK(!route_fragment.is_last());
            }

            {
                auto const& route_fragment = route_fragments_we_got[1];

                BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 1);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 3);
                BOOST_CHECK(!route_fragment.is_last());
            }

            {
                auto const& route_fragment = route_fragments_we_got[2];

                BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 1);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 0);
                BOOST_CHECK(!route_fragment.is_last());
            }
        }

        // fragments 2
        {
            auto const route_fragments_we_got = partition_we_got.route_fragments(2).get();

            BOOST_REQUIRE_EQUAL(std::size(route_fragments_we_got), 3);

            {
                auto const& route_fragment = route_fragments_we_got[0];

                BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 2);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 8);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 7);
                BOOST_CHECK(!route_fragment.is_last());
            }

            {
                auto const& route_fragment = route_fragments_we_got[1];

                BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 1);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 5);
                BOOST_CHECK(!route_fragment.is_last());
            }

            {
                auto const& route_fragment = route_fragments_we_got[2];

                BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 2);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 2);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 1);
                BOOST_CHECK(route_fragment.is_last());
            }
        }
    }

    // partition(0, 2)
    {
        auto const& partition_we_got = partitions(0, 2);
        partition_we_got.wait();

        auto const offset_we_got = partition_we_got.offset().get();
        auto const shape_we_got = partition_we_got.shape().get();
        auto const nr_routes_we_got = partition_we_got.nr_routes().get();
        auto const nr_route_fragments_we_got = partition_we_got.nr_route_fragments().get();
        auto const route_ids_we_got = partition_we_got.route_ids().get();

        Offset const offset_we_want{0, 6};
        BOOST_CHECK_EQUAL(offset_we_got, offset_we_want);

        Shape const shape_we_want{3, 3};
        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);

        BOOST_CHECK_EQUAL(nr_routes_we_got, 1);
        BOOST_REQUIRE_EQUAL(nr_route_fragments_we_got, 3);

        BOOST_REQUIRE_EQUAL(route_ids_we_got.size(), 1);
        BOOST_CHECK(route_ids_we_got.find(2) != route_ids_we_got.end());

        // fragments 1
        {
            auto const route_fragments_we_got = partition_we_got.route_fragments(2).get();

            BOOST_REQUIRE_EQUAL(std::size(route_fragments_we_got), 3);

            {
                auto const& route_fragment = route_fragments_we_got[0];

                BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 3);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 8);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 7);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 6);
                BOOST_CHECK(!route_fragment.is_last());
            }

            {
                auto const& route_fragment = route_fragments_we_got[1];

                BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 3);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 5);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 4);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 3);
                BOOST_CHECK(!route_fragment.is_last());
            }

            {
                auto const& route_fragment = route_fragments_we_got[2];

                BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 2);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 1);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 0);
                BOOST_CHECK(!route_fragment.is_last());
            }
        }
    }

    // partition(1, 0)
    {
        auto const& partition_we_got = partitions(1, 0);
        partition_we_got.wait();

        auto const offset_we_got = partition_we_got.offset().get();
        auto const shape_we_got = partition_we_got.shape().get();
        auto const nr_routes_we_got = partition_we_got.nr_routes().get();
        auto const nr_route_fragments_we_got = partition_we_got.nr_route_fragments().get();
        auto const route_ids_we_got = partition_we_got.route_ids().get();

        Offset const offset_we_want{3, 0};
        BOOST_CHECK_EQUAL(offset_we_got, offset_we_want);

        Shape const shape_we_want{3, 3};
        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);

        BOOST_CHECK_EQUAL(nr_routes_we_got, 2);
        BOOST_REQUIRE_EQUAL(nr_route_fragments_we_got, 3);

        BOOST_REQUIRE_EQUAL(route_ids_we_got.size(), 2);
        BOOST_CHECK(route_ids_we_got.find(1) != route_ids_we_got.end());
        BOOST_CHECK(route_ids_we_got.find(3) != route_ids_we_got.end());

        // fragments 1
        {
            auto const route_fragments_we_got = partition_we_got.route_fragments(1).get();

            BOOST_REQUIRE_EQUAL(std::size(route_fragments_we_got), 1);

            {
                auto const& route_fragment = route_fragments_we_got[0];

                BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 3);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 2);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 1);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 0);
                BOOST_CHECK(!route_fragment.is_last());
            }
        }

        // fragments 3
        {
            auto const route_fragments_we_got = partition_we_got.route_fragments(3).get();

            BOOST_REQUIRE_EQUAL(std::size(route_fragments_we_got), 2);

            {
                auto const& route_fragment = route_fragments_we_got[0];

                BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 3);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 8);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 7);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 6);
                BOOST_CHECK(!route_fragment.is_last());
            }

            {
                auto const& route_fragment = route_fragments_we_got[1];

                BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 2);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 5);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 4);
                BOOST_CHECK(route_fragment.is_last());
            }
        }
    }

    // partition(1, 1)
    {
        auto const& partition_we_got = partitions(1, 1);
        partition_we_got.wait();

        auto const offset_we_got = partition_we_got.offset().get();
        auto const shape_we_got = partition_we_got.shape().get();
        auto const nr_routes_we_got = partition_we_got.nr_routes().get();
        auto const nr_route_fragments_we_got = partition_we_got.nr_route_fragments().get();
        auto const route_ids_we_got = partition_we_got.route_ids().get();

        Offset const offset_we_want{3, 3};
        BOOST_CHECK_EQUAL(offset_we_got, offset_we_want);

        Shape const shape_we_want{3, 3};
        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);

        BOOST_CHECK_EQUAL(nr_routes_we_got, 2);
        BOOST_REQUIRE_EQUAL(nr_route_fragments_we_got, 4);

        BOOST_REQUIRE_EQUAL(route_ids_we_got.size(), 2);
        BOOST_CHECK(route_ids_we_got.find(2) != route_ids_we_got.end());
        BOOST_CHECK(route_ids_we_got.find(3) != route_ids_we_got.end());

        // fragments 2
        {
            auto const route_fragments_we_got = partition_we_got.route_fragments(2).get();

            BOOST_REQUIRE_EQUAL(std::size(route_fragments_we_got), 2);

            {
                auto const& route_fragment = route_fragments_we_got[0];

                BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 2);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 5);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 4);
                BOOST_CHECK(!route_fragment.is_last());
            }

            {
                auto const& route_fragment = route_fragments_we_got[1];

                BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 2);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 2);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 1);
                BOOST_CHECK(!route_fragment.is_last());
            }
        }

        // fragments 3
        {
            auto const route_fragments_we_got = partition_we_got.route_fragments(3).get();

            BOOST_REQUIRE_EQUAL(std::size(route_fragments_we_got), 2);

            {
                auto const& route_fragment = route_fragments_we_got[0];

                BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 3);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 8);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 7);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 6);
                BOOST_CHECK(!route_fragment.is_last());
            }

            {
                auto const& route_fragment = route_fragments_we_got[1];

                BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 1);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 3);
                BOOST_CHECK(!route_fragment.is_last());
            }
        }
    }

    // partition(1, 2)
    {
        auto const& partition_we_got = partitions(1, 2);
        partition_we_got.wait();

        auto const offset_we_got = partition_we_got.offset().get();
        auto const shape_we_got = partition_we_got.shape().get();
        auto const nr_routes_we_got = partition_we_got.nr_routes().get();
        auto const nr_route_fragments_we_got = partition_we_got.nr_route_fragments().get();
        auto const route_ids_we_got = partition_we_got.route_ids().get();

        Offset const offset_we_want{3, 6};
        BOOST_CHECK_EQUAL(offset_we_got, offset_we_want);

        Shape const shape_we_want{3, 3};
        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);

        BOOST_CHECK_EQUAL(nr_routes_we_got, 2);
        BOOST_REQUIRE_EQUAL(nr_route_fragments_we_got, 3);

        BOOST_REQUIRE_EQUAL(route_ids_we_got.size(), 2);
        BOOST_CHECK(route_ids_we_got.find(2) != route_ids_we_got.end());
        BOOST_CHECK(route_ids_we_got.find(3) != route_ids_we_got.end());

        // fragments 2
        {
            auto const route_fragments_we_got = partition_we_got.route_fragments(2).get();

            BOOST_REQUIRE_EQUAL(std::size(route_fragments_we_got), 2);

            {
                auto const& route_fragment = route_fragments_we_got[0];

                BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 2);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 5);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 3);
                BOOST_CHECK(!route_fragment.is_last());
            }

            {
                auto const& route_fragment = route_fragments_we_got[1];

                BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 3);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 2);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 1);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 0);
                BOOST_CHECK(!route_fragment.is_last());
            }
        }

        // fragments 3
        {
            auto const route_fragments_we_got = partition_we_got.route_fragments(3).get();

            BOOST_REQUIRE_EQUAL(std::size(route_fragments_we_got), 1);

            {
                auto const& route_fragment = route_fragments_we_got[0];

                BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 3);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 8);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 7);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 6);
                BOOST_CHECK(!route_fragment.is_last());
            }
        }
    }

    // partition(2, 0)
    {
        auto const& partition_we_got = partitions(2, 0);
        partition_we_got.wait();

        auto const offset_we_got = partition_we_got.offset().get();
        auto const shape_we_got = partition_we_got.shape().get();
        auto const nr_routes_we_got = partition_we_got.nr_routes().get();
        auto const nr_route_fragments_we_got = partition_we_got.nr_route_fragments().get();
        auto const route_ids_we_got = partition_we_got.route_ids().get();

        Offset const offset_we_want{6, 0};
        BOOST_CHECK_EQUAL(offset_we_got, offset_we_want);

        Shape const shape_we_want{3, 3};
        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);

        BOOST_CHECK_EQUAL(nr_routes_we_got, 1);
        BOOST_REQUIRE_EQUAL(nr_route_fragments_we_got, 3);

        BOOST_REQUIRE_EQUAL(route_ids_we_got.size(), 1);
        BOOST_CHECK(route_ids_we_got.find(3) != route_ids_we_got.end());

        // fragments 3
        {
            auto const route_fragments_we_got = partition_we_got.route_fragments(3).get();

            BOOST_REQUIRE_EQUAL(std::size(route_fragments_we_got), 3);

            {
                auto const& route_fragment = route_fragments_we_got[0];

                BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 3);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 8);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 7);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 6);
                BOOST_CHECK(!route_fragment.is_last());
            }

            {
                auto const& route_fragment = route_fragments_we_got[1];

                BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 3);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 5);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 4);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 3);
                BOOST_CHECK(!route_fragment.is_last());
            }

            {
                auto const& route_fragment = route_fragments_we_got[2];

                BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 2);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 1);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 0);
                BOOST_CHECK(!route_fragment.is_last());
            }
        }
    }

    // partition(2, 1)
    {
        auto const& partition_we_got = partitions(2, 1);
        partition_we_got.wait();

        auto const offset_we_got = partition_we_got.offset().get();
        auto const shape_we_got = partition_we_got.shape().get();
        auto const nr_routes_we_got = partition_we_got.nr_routes().get();
        auto const nr_route_fragments_we_got = partition_we_got.nr_route_fragments().get();
        auto const route_ids_we_got = partition_we_got.route_ids().get();

        Offset const offset_we_want{6, 3};
        BOOST_CHECK_EQUAL(offset_we_got, offset_we_want);

        Shape const shape_we_want{3, 3};
        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);

        BOOST_CHECK_EQUAL(nr_routes_we_got, 1);
        BOOST_REQUIRE_EQUAL(nr_route_fragments_we_got, 3);

        BOOST_REQUIRE_EQUAL(route_ids_we_got.size(), 1);
        BOOST_CHECK(route_ids_we_got.find(3) != route_ids_we_got.end());

        // fragments 3
        {
            auto const route_fragments_we_got = partition_we_got.route_fragments(3).get();

            BOOST_REQUIRE_EQUAL(std::size(route_fragments_we_got), 3);

            {
                auto const& route_fragment = route_fragments_we_got[0];

                BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 3);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 8);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 7);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 6);
                BOOST_CHECK(!route_fragment.is_last());
            }

            {
                auto const& route_fragment = route_fragments_we_got[1];

                BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 3);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 5);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 4);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 3);
                BOOST_CHECK(!route_fragment.is_last());
            }

            {
                auto const& route_fragment = route_fragments_we_got[2];

                BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 3);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 2);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 1);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 0);
                BOOST_CHECK(!route_fragment.is_last());
            }
        }
    }

    // partition(2, 2)
    {
        auto const& partition_we_got = partitions(2, 2);
        partition_we_got.wait();

        auto const offset_we_got = partition_we_got.offset().get();
        auto const shape_we_got = partition_we_got.shape().get();
        auto const nr_routes_we_got = partition_we_got.nr_routes().get();
        auto const nr_route_fragments_we_got = partition_we_got.nr_route_fragments().get();
        auto const route_ids_we_got = partition_we_got.route_ids().get();

        Offset const offset_we_want{6, 6};
        BOOST_CHECK_EQUAL(offset_we_got, offset_we_want);

        Shape const shape_we_want{3, 3};
        BOOST_CHECK_EQUAL(shape_we_got, shape_we_want);

        BOOST_CHECK_EQUAL(nr_routes_we_got, 1);
        BOOST_REQUIRE_EQUAL(nr_route_fragments_we_got, 3);

        BOOST_REQUIRE_EQUAL(route_ids_we_got.size(), 1);
        BOOST_CHECK(route_ids_we_got.find(3) != route_ids_we_got.end());

        // fragments 3
        {
            auto const route_fragments_we_got = partition_we_got.route_fragments(3).get();

            BOOST_REQUIRE_EQUAL(std::size(route_fragments_we_got), 3);

            {
                auto const& route_fragment = route_fragments_we_got[0];

                BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 1);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 7);
                BOOST_CHECK(!route_fragment.is_last());
            }

            {
                auto const& route_fragment = route_fragments_we_got[1];

                BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 3);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 5);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 4);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 3);
                BOOST_CHECK(!route_fragment.is_last());
            }

            {
                auto const& route_fragment = route_fragments_we_got[2];

                BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 3);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 2);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 1);
                BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 0);
                BOOST_CHECK(!route_fragment.is_last());
            }
        }
    }
}


BOOST_AUTO_TEST_CASE(use_case_02)
{
    // Same as use_case_01, but with a single partition
    lue::Rank const rank{2};

    using ZoneElement = std::uint32_t;
    using ValueElement = double;
    using Shape = lue::Shape<lue::Count, rank>;
    using ValueArray = lue::PartitionedArray<ValueElement, rank>;
    using ZoneArray = lue::PartitionedArray<ZoneElement, rank>;

    ValueElement const x1{lue::policy::no_data_value<ValueElement>};
    ZoneElement const x2{lue::policy::no_data_value<ZoneElement>};
    Shape const array_shape{9, 9};
    Shape const partition_shape{array_shape};
    lue::Count const nr_zones{3};

    // clang-format off
    ValueArray const value_array = lue::test::create_partitioned_array<ValueArray>(
        array_shape,
        partition_shape,
        {
            {
                x1,  2,  3,  4,  5,  6,  7,  8,  9,
                10, 11, 12, 13, 14, 15, 16, 17, 18,
                19, 20, 21, 22, 23, 24, 25, 26, 27,
                28, 29, 30, x1, 32, 33, 34, 35, 36,
                37, 38, 39, 40, 41, 42, 43, x1, 45,
                46, 47, 48, 49, 50, 51, 52, 53, 54,
                55, 56, x1, 58, 59, 60, 61, 62, 63,
                64, 65, 66, 67, 68, 69, 70, 71, 72,
                73, 74, 75, 76, 77, 78, 79, 80, x1,
            },
        });

    ZoneArray const zone_array = lue::test::create_partitioned_array<ZoneArray>(
        array_shape,
        partition_shape,
        {
            {
                 1,  1,  1,  1,  2,  2,  2,  2, x2,
                 1,  1,  1,  1, x2,  2,  2,  2,  2,
                 1,  1, x2,  1,  2,  2,  2,  2,  2,
                 1,  1,  1,  1,  2,  2,  2,  2,  2,
                x2,  3,  3,  3,  2,  2,  2,  2,  2,
                 3,  3,  3,  3,  3,  3,  3,  3,  3,
                 3,  3, x2,  3,  3,  3,  3,  3,  3,
                 3,  3,  3,  3,  3,  3,  3,  3,  3,
                 3,  3,  3,  3,  3,  3, x2,  3,  3,
            },
        });
    // clang-format on

    Route const route_we_got = lue::value_policies::decreasing_order(zone_array, value_array);

    route_we_got.wait();

    BOOST_CHECK_EQUAL(route_we_got.nr_routes(), nr_zones);

    auto const& partitions = route_we_got.partitions();

    BOOST_REQUIRE(lue::all_are_valid(partitions));

    auto const& partition_we_got = partitions(0, 0);
    partition_we_got.wait();

    auto const offset_we_got = partition_we_got.offset().get();
    auto const shape_we_got = partition_we_got.shape().get();
    auto const nr_routes_we_got = partition_we_got.nr_routes().get();
    auto const nr_route_fragments_we_got = partition_we_got.nr_route_fragments().get();
    auto const route_ids_we_got = partition_we_got.route_ids().get();

    Offset const offset_we_want{0, 0};
    BOOST_CHECK_EQUAL(offset_we_got, offset_we_want);

    BOOST_CHECK_EQUAL(shape_we_got, partition_shape);

    BOOST_CHECK_EQUAL(nr_routes_we_got, 3);
    BOOST_REQUIRE_EQUAL(nr_route_fragments_we_got, 3);

    //    +----+----+----+----+----+----+----+----+----+
    //  0 | ND | 13 | 12 | 11 | 22 | 21 | 20 | 19 | ND |  8
    //    +----+----+----+----+----+----+----+----+----+
    //  9 | 10 |  9 |  8 |  7 | ND | 18 | 17 | 16 | 15 | 17
    //    +----+----+----+----+----+----+----+----+----+
    // 18 |  6 |  5 | ND |  4 | 14 | 13 | 12 | 11 | 10 | 26
    //    +----+----+----+----+----+----+----+----+----+
    // 27 |  3 |  2 |  1 | ND |  9 |  8 |  7 |  6 |  5 | 35
    //    +----+----+----+----+----+----+----+----+----+
    // 36 | ND | 36 | 35 | 34 |  4 |  3 |  2 | ND |  1 | 44
    //    +----+----+----+----+----+----+----+----+----+
    // 45 | 33 | 32 | 31 | 30 | 29 | 28 | 27 | 26 | 25 | 53
    //    +----+----+----+----+----+----+----+----+----+
    // 54 | 24 | 23 | ND | 22 | 21 | 20 | 19 | 18 | 17 | 62
    //    +----+----+----+----+----+----+----+----+----+
    // 63 | 16 | 15 | 14 | 13 | 12 | 11 | 10 |  9 |  8 | 71
    //    +----+----+----+----+----+----+----+----+----+
    // 72 |  7 |  6 |  5 |  4 |  3 |  2 | ND |  1 | ND | 80
    //    +----+----+----+----+----+----+----+----+----+

    BOOST_REQUIRE_EQUAL(route_ids_we_got.size(), 3);
    BOOST_CHECK(route_ids_we_got.find(1) != route_ids_we_got.end());
    BOOST_CHECK(route_ids_we_got.find(2) != route_ids_we_got.end());
    BOOST_CHECK(route_ids_we_got.find(3) != route_ids_we_got.end());

    // fragments 1
    {
        auto const route_fragments_we_got = partition_we_got.route_fragments(1).get();

        BOOST_REQUIRE_EQUAL(std::size(route_fragments_we_got), 1);

        auto const& route_fragment = route_fragments_we_got[0];

        BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 13);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 29);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 28);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 27);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[3], 21);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[4], 19);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[5], 18);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[6], 12);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[7], 11);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[8], 10);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[9], 9);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[10], 3);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[11], 2);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[12], 1);
        BOOST_CHECK(route_fragment.is_last());
    }

    // fragments 2
    {
        auto const route_fragments_we_got = partition_we_got.route_fragments(2).get();

        BOOST_REQUIRE_EQUAL(std::size(route_fragments_we_got), 1);

        auto const& route_fragment = route_fragments_we_got[0];

        BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 22);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 44);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 42);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 41);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[3], 40);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[4], 35);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[5], 34);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[6], 33);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[7], 32);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[8], 31);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[9], 26);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[10], 25);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[11], 24);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[12], 23);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[13], 22);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[14], 17);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[15], 16);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[16], 15);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[17], 14);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[18], 7);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[19], 6);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[20], 5);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[21], 4);
        BOOST_CHECK(route_fragment.is_last());
    }

    // fragments 3
    {
        auto const route_fragments_we_got = partition_we_got.route_fragments(3).get();

        BOOST_REQUIRE_EQUAL(std::size(route_fragments_we_got), 1);

        auto const& route_fragment = route_fragments_we_got[0];

        BOOST_REQUIRE_EQUAL(route_fragment.nr_cells(), 36);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[0], 79);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[1], 77);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[2], 76);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[3], 75);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[4], 74);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[5], 73);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[6], 72);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[7], 71);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[8], 70);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[9], 69);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[10], 68);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[11], 67);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[12], 66);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[13], 65);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[14], 64);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[15], 63);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[16], 62);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[17], 61);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[18], 60);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[19], 59);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[20], 58);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[21], 57);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[22], 55);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[23], 54);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[24], 53);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[25], 52);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[26], 51);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[27], 50);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[28], 49);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[29], 48);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[30], 47);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[31], 46);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[32], 45);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[33], 39);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[34], 38);
        BOOST_CHECK_EQUAL(route_fragment.cell_idxs()[35], 37);
        BOOST_CHECK(route_fragment.is_last());
    }
}


BOOST_AUTO_TEST_CASE(single_partition_many_routes)
{
    // Verify that multiple threads don't trip over each other
    lue::Rank const rank{2};

    using ZoneElement = std::uint64_t;
    using ValueElement = float;
    using Shape = lue::Shape<lue::Count, rank>;
    using ValueArray = lue::PartitionedArray<ValueElement, rank>;
    using ZoneArray = lue::PartitionedArray<ZoneElement, rank>;

    ValueElement const x1{lue::policy::no_data_value<ValueElement>};
    ZoneElement const x2{lue::policy::no_data_value<ZoneElement>};

    Shape const array_shape{500, 500};
    Shape const partition_shape{array_shape};
    lue::Count const nr_zones{10};

    using namespace lue::value_policies;

    ValueArray value_array{lue::value_policies::uniform<ValueElement>(array_shape, partition_shape, 0, 100)};

    value_array = lue::value_policies::where(value_array > ValueElement{10.0}, value_array);

    ZoneArray const zone_array{
        lue::value_policies::uniform<ZoneElement>(array_shape, partition_shape, 1, nr_zones)};

    Route const route_we_got = lue::value_policies::decreasing_order(zone_array, value_array);

    route_we_got.wait();

    // This assumes all zones actually occur in zone_array
    BOOST_CHECK_EQUAL(route_we_got.nr_routes(), nr_zones);
}


// BOOST_AUTO_TEST_CASE(all_no_data)
// {
// }


// BOOST_AUTO_TEST_CASE(random_input)
// {
//     lue::Rank const rank{2};
//
//     using ZoneElement = std::uint64_t;
//     using ValueElement = float;
//     using Shape = lue::Shape<lue::Count, rank>;
//     using ValueArray = lue::PartitionedArray<ValueElement, rank>;
//     using ZoneArray = lue::PartitionedArray<ZoneElement, rank>;
//
//     ValueElement const x1{lue::policy::no_data_value<ValueElement>};
//     ZoneElement const x2{lue::policy::no_data_value<ZoneElement>};
//
//     std::random_device random_device{};
//     std::default_random_engine random_number_engine(random_device());
//
//     Shape const array_shape = [&]()
//     {
//         lue::Count const min{100};
//         lue::Count const max{500};
//         std::uniform_int_distribution<lue::Count> distribution(min, max);
//
//         return Shape{
//             distribution(random_number_engine),
//             distribution(random_number_engine),
//         };
//     }();
//     Shape const partition_shape = [&]()
//     {
//         lue::Count const min{90};
//         lue::Count const max{100};
//         std::uniform_int_distribution<lue::Count> distribution(min, max);
//
//         return Shape{
//             distribution(random_number_engine),
//             distribution(random_number_engine),
//         };
//     }();
//     lue::Count const nr_zones = [&]()
//     {
//         lue::Count const min{1};
//         lue::Count const max{10};
//         std::uniform_int_distribution<lue::Count> distribution(min, max);
//
//         return distribution(random_number_engine);
//     }();
//
//     lue::Count const max_route_length = [&]()
//     {
//         lue::Count const min{0};
//         lue::Count const max{lue::nr_elements(array_shape)};
//         std::uniform_int_distribution<lue::Count> distribution(min, max);
//
//         return distribution(random_number_engine);
//     }();
//
//     using namespace lue::value_policies;
//
//     ValueArray value_array{lue::value_policies::uniform<ValueElement>(array_shape, partition_shape, 0,
//     100)};
//
//     value_array = lue::value_policies::where(value_array > ValueElement{10.0}, value_array);
//
//     ZoneArray const zone_array{
//         lue::value_policies::uniform<ZoneElement>(array_shape, partition_shape, 1, nr_zones)};
//
//     Route const route_we_got = lue::value_policies::decreasing_order(zone_array, value_array,
//     max_route_length);
//
//     route_we_got.wait();
//
//     BOOST_CHECK_EQUAL(route_we_got.nr_routes(), nr_zones);
// }
