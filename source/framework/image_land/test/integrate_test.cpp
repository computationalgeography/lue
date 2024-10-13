#define BOOST_TEST_MODULE lue framework image_land integrate
#include "lue/framework/algorithm/value_policies/decreasing_order.hpp"
#include "lue/framework/algorithm/value_policies/integrate.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


BOOST_AUTO_TEST_CASE(use_case_01)
{
    using ZoneElement = lue::SignedIntegralElement<0>;
    using RouteID = ZoneElement;
    using SuitabilityElement = lue::FloatingPointElement<0>;
    using IntegrandElement = lue::FloatingPointElement<0>;

    lue::Rank const rank{2};

    using ZoneArray = lue::PartitionedArray<ZoneElement, rank>;
    using IntegrandArray = lue::PartitionedArray<IntegrandElement, rank>;
    using SuitabilityArray = lue::PartitionedArray<SuitabilityElement, rank>;
    using Route = lue::SerialRoute<RouteID, rank>;

    using Shape = lue::ShapeT<ZoneArray>;

    lue::Count const max_nr_cells{10};

    Shape const array_shape{4, 4};
    Shape const partition_shape{2, 2};

    ZoneElement const z{lue::policy::no_data_value<ZoneElement>};

    // +---+---+  +---+---+
    // | 1 | 2 |  | 3 | x |
    // +---+---+  +---+---+
    // | 2 | 3 |  | x | 2 |
    // +---+---+  +---+---+
    //
    // +---+---+  +---+---+
    // | 3 | x |  | 2 | 3 |
    // +---+---+  +---+---+
    // | x | 2 |  | 3 | 1 |
    // +---+---+  +---+---+
    ZoneArray const zone_array = lue::test::create_partitioned_array<ZoneArray>(
        array_shape,
        partition_shape,
        {
            // clang-format off
            {
                1, 2,
                2, 3,
            },
            {
                3, z,
                z, 2,
            },
            {
                3, z,
                z, 2,
            },
            {
                2, 3,
                3, 1,
            },
            // clang-format on
        });

    // +----+----+  +----+----+
    // |  x |  5 |  |  9 | 13 |
    // +----+----+  +----+----+
    // |  2 |  x |  | 10 | 14 |
    // +----+----+  +----+----+
    //
    // +----+----+  +----+----+
    // |  3 |  7 |  |  x | 15 |
    // +----+----+  +----+----+
    // |  4 |  8 |  | 12 |  x |
    // +----+----+  +----+----+
    SuitabilityElement const s{lue::policy::no_data_value<SuitabilityElement>};
    SuitabilityArray const field = lue::test::create_partitioned_array<SuitabilityArray>(
        array_shape,
        partition_shape,
        {
            // clang-format off
            {
                s, 5,
                2, s,
            },
            {
                9, 13,
                10, 14,
            },
            {
                3, 7,
                4, 8,
            },
            {
                s, 15,
                12, s,
            },
            // clang-format on
        });

    // +----+----+  +----+----+
    // |  x | b3 |  | c3 |  x |
    // +----+----+  +----+----+
    // | b4 |  x |  |  x | b1 |
    // +----+----+  +----+----+
    //
    // +----+----+  +----+----+
    // | c4 |  x |  |  x | c1 |
    // +----+----+  +----+----+
    // |  x | b2 |  | c2 |  x |
    // +----+----+  +----+----+
    Route const route{lue::value_policies::decreasing_order(zone_array, field, max_nr_cells)};

    // +----+----+  +----+----+
    // |  1 |  x |  |  1 |  2 |
    // +----+----+  +----+----+
    // |  1 |  2 |  |  1 |  2 |
    // +----+----+  +----+----+
    //
    // +----+----+  +----+----+
    // |  1 |  2 |  |  1 |  2 |
    // +----+----+  +----+----+
    // |  1 |  2 |  |  1 |  2 |
    // +----+----+  +----+----+
    IntegrandElement const i{lue::policy::no_data_value<IntegrandElement>};
    IntegrandArray const integrand = lue::test::create_partitioned_array<IntegrandArray>(
        array_shape,
        partition_shape,
        {
            // clang-format off
            {
                1, i,
                1, 2,
            },
            {
                1, 2,
                1, 2,
            },
            {
                1, 2,
                1, 2,
            },
            {
                1, 2,
                1, 2,
            },
            // clang-format on
        });

    IntegrandArray const result_we_got = lue::value_policies::integrate(route, integrand, max_nr_cells);

    // +----+----+  +----+----+
    // |  x |  x |  |  4 |  x |
    // +----+----+  +----+----+
    // |  5 |  x |  |  x |  2 |
    // +----+----+  +----+----+
    //
    // +----+----+  +----+----+
    // |  5 |  x |  |  x |  2 |
    // +----+----+  +----+----+
    // |  x |  4 |  |  3 |  x |
    // +----+----+  +----+----+
    IntegrandArray const result_we_want = lue::test::create_partitioned_array<IntegrandArray>(
        array_shape,
        partition_shape,
        {
            // clang-format off
            {
                i, i,
                5, i,
            },
            {
                4, i,
                i, 2,
            },
            {
                5, i,
                i, 4,
            },
            {
                i, 2,
                3, i,
            },
            // clang-format on
        });

    lue::test::check_arrays_are_equal(result_we_got, result_we_want);
}
