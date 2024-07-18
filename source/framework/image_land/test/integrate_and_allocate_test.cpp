#define BOOST_TEST_MODULE lue framework image_land integrate_and_allocate
#include "lue/framework/algorithm/value_policies/decreasing_order.hpp"
#include "lue/framework/algorithm/value_policies/integrate_and_allocate.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


BOOST_AUTO_TEST_CASE(use_case_01)
{
    using ZoneElement = std::uint32_t;
    using RouteID = ZoneElement;
    using FloatingPointElement = float;

    lue::Rank const rank{2};

    using ZoneArray = lue::PartitionedArray<ZoneElement, rank>;
    using FloatingPointArray = lue::PartitionedArray<FloatingPointElement, rank>;
    using Route = lue::SerialRoute<RouteID, rank>;

    using Shape = lue::ShapeT<ZoneArray>;

    lue::Count const max_nr_cells{10};

    Shape const array_shape{4, 4};
    Shape const partition_shape{2, 2};

    ZoneElement const no_data_zone{lue::policy::no_data_value<ZoneElement>};
    FloatingPointElement const no_data_float{lue::policy::no_data_value<FloatingPointElement>};

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
                3, no_data_zone,
                no_data_zone, 2,
            },
            {
                3, no_data_zone,
                no_data_zone, 2,
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
    FloatingPointArray const field = lue::test::create_partitioned_array<FloatingPointArray>(
        array_shape,
        partition_shape,
        {
            // clang-format off
            {
                no_data_float, 5,
                2, no_data_float,
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
                no_data_float, 15,
                12, no_data_float,
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
    Route const routes{lue::value_policies::decreasing_order(zone_array, field, max_nr_cells)};

    FloatingPointArray const sdp_factors =
        lue::create_partitioned_array(array_shape, partition_shape, FloatingPointElement{0.2});
    std::vector<std::reference_wrapper<FloatingPointArray const>> const sdp_factors_per_crop{sdp_factors};

    FloatingPointArray const yield_factors =
        lue::create_partitioned_array(array_shape, partition_shape, FloatingPointElement{0.2});
    std::vector<std::reference_wrapper<FloatingPointArray const>> const yield_factors_per_crop{yield_factors};

    FloatingPointArray const initial_crop_fractions =
        lue::create_partitioned_array(array_shape, partition_shape, FloatingPointElement{0.5});
    std::vector<std::reference_wrapper<FloatingPointArray const>> const initial_crop_fractions_per_crop{
        initial_crop_fractions};

    std::map<ZoneElement, std::vector<FloatingPointElement>> zonal_demands_per_crop{
        {1, {100}}, {2, {100}}, {3, {100}}};
    auto zonal_demands_per_crop_f =
        hpx::make_ready_future<std::map<ZoneElement, std::vector<FloatingPointElement>>>(
            std::move(zonal_demands_per_crop))
            .share();

    std::map<ZoneElement, std::vector<FloatingPointElement>> current_zonal_production_per_crop{
        {1, {1}}, {2, {1}}, {3, {1}}};
    auto current_zonal_production_per_crop_f =
        hpx::make_ready_future<std::map<ZoneElement, std::vector<FloatingPointElement>>>(
            std::move(current_zonal_production_per_crop))
            .share();

    FloatingPointArray const irrigated_crop_fractions =
        lue::create_partitioned_array(array_shape, partition_shape, FloatingPointElement{0.5});

    std::vector<FloatingPointArray> crop_fraction_per_crop_we_got{};
    hpx::shared_future<std::map<ZoneElement, std::vector<FloatingPointElement>>>
        zonal_production_per_crop_we_got;

    std::tie(crop_fraction_per_crop_we_got, zonal_production_per_crop_we_got) =
        lue::value_policies::integrate_and_allocate(
            routes,
            sdp_factors_per_crop,
            yield_factors_per_crop,
            initial_crop_fractions_per_crop,
            zonal_demands_per_crop_f,
            current_zonal_production_per_crop_f,
            irrigated_crop_fractions);

    // TODO Test results
}
