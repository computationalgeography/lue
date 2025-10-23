#define BOOST_TEST_MODULE lue framework algorithm accu_fraction
#include "flow_accumulation.hpp"
#include "lue/framework/algorithm/value_policies/accu_fraction.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


BOOST_AUTO_TEST_CASE(overloads)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    lue::PartitionedArray<FlowDirectionElement, 2> const flow_direction{};
    [[maybe_unused]] lue::PartitionedArray<MaterialElement, 2> outflow{};
    [[maybe_unused]] lue::PartitionedArray<MaterialElement, 2> residue{};

    lue::PartitionedArray<MaterialElement, 2> const inflow_raster{};
    lue::PartitionedArray<MaterialElement, 2> const fraction_raster{};
    lue::Scalar<MaterialElement> const inflow_scalar{};
    lue::Scalar<MaterialElement> const fraction_scalar{};
    // MaterialElement const inflow_value{};
    // MaterialElement const fraction_value{};

    // raster, raster
    std::tie(outflow, residue) =
        lue::value_policies::accu_fraction(flow_direction, inflow_raster, fraction_raster);

    // scalar, scalar
    std::tie(outflow, residue) =
        lue::value_policies::accu_fraction(flow_direction, inflow_scalar, fraction_scalar);

    // TODO: Support once API lib is ready
    // // value, value
    // std::tie(outflow, residue) =
    //     lue::value_policies::accu_fraction(flow_direction, inflow_value, fraction_value);

    // raster, scalar
    std::tie(outflow, residue) =
        lue::value_policies::accu_fraction(flow_direction, inflow_raster, fraction_scalar);

    // TODO: Support once API lib is ready
    // // raster, value
    // std::tie(outflow, residue) =
    //     lue::value_policies::accu_fraction(flow_direction, inflow_raster, fraction_value);

    // scalar, raster
    std::tie(outflow, residue) =
        lue::value_policies::accu_fraction(flow_direction, inflow_scalar, fraction_raster);

    // TODO: Support once API lib is ready
    // // scalar, value
    // std::tie(outflow, residue) =
    //     lue::value_policies::accu_fraction(flow_direction, inflow_scalar, fraction_value);

    // TODO: Support once API lib is ready
    // // value, raster
    // std::tie(outflow, residue) =
    //     lue::value_policies::accu_fraction(flow_direction, inflow_value, fraction_raster);

    // TODO: Support once API lib is ready
    // // value, scalar
    // std::tie(outflow, residue) =
    //     lue::value_policies::accu_fraction(flow_direction, inflow_value, fraction_scalar);
}


BOOST_AUTO_TEST_CASE(spiral_in_case)
{
    // accu_fraction(flow_direction, material, ones) == accu(flow_direction, material)
    using MaterialElement = lue::FloatingPointElement<0>;

    auto const flow_direction = lue::test::spiral_in();
    auto const inflow = lue::test::ones<MaterialElement>();
    auto const fraction = lue::test::ones<MaterialElement>();

    auto const [outflow_we_got, residue_we_got] =
        lue::value_policies::accu_fraction(flow_direction, inflow, fraction);

    auto outflow_we_want = lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
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
    auto const residue_we_want = lue::test::zeros<MaterialElement>();

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
    lue::test::check_arrays_are_equal(residue_we_got, residue_we_want);
}


BOOST_AUTO_TEST_CASE(merging_streams_case_01)
{
    // No-data material in ridge cell. From there on, it must propagate down to all downstream cells, in all
    // downstream partitions. Similar for a no-data fraction in ridge cell.

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
            }
            // clang-format on
            // NOLINTEND
        });
    auto const fraction = lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
        lue::test::array_shape,
        lue::test::partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                // 0, 0
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
            },
            {
                // 0, 1
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
            },
            {
                // 0, 2
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
            },
            {
                // 1, 0
                0.5, 0.5, 0.5,
                0.5, 0.5, x,
                0.5, 0.5, 0.5,
            },
            {
                // 1, 1
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
            },
            {
                // 1, 2
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
            },
            {
                // 2, 0
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
            },
            {
                // 2, 1
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
            },
            {
                // 2, 2
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
                0.5, 0.5, 0.5,
            },
            // clang-format on
            // NOLINTEND
        });

    auto const [outflow_we_got, residue_we_got] =
        lue::value_policies::accu_fraction(flow_direction, inflow, fraction);

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
                    x, x, 0.50,
                },
                {
                    // 0, 1
                    x, 0.50, 0.50,
                    x, 0.50, 0.50,
                    x, 0.75, x,
                },
                {
                    // 0, 2
                    x, x, 0.50,
                    1.00, x, 0.50,
                    x, x, 0.50,
                },
                {
                    // 1, 0
                    x, x, 0.50,
                    x, x, x,
                    x, x, 0.50,
                },
                {
                    // 1, 1
                    x, x, x,
                    x, x, 0.75,
                    0.75, x, 1,
                },
                {
                    // 1, 2
                    x, x, x,
                    0.50, x, x,
                    0.50, x, x,
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
                    x, x, 0.75,
                    0.9375, 0.875, 0.75,
                },
                {
                    // 2, 2
                    0.50, x, x,
                    0.50, x, x,
                    0.50, x, x,
                }
                // clang-format on
                // NOLINTEND
            });
    auto const residue_we_want =
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
                    x, x, 0.5,
                },
                {
                    // 0, 1
                    x, 0.50, 0.50,
                    x, 0.50, 0.50,
                    x, 0.75, x,
                },
                {
                    // 0, 2
                    x, x, 0.50,
                    1.00, x, 0.50,
                    x, x, 0.50,
                },
                {
                    // 1, 0
                    x, x, 0.50,
                    x, x, x,
                    x, x, 0.50,
                },
                {
                    // 1, 1
                    x, x, x,
                    x, x, 0.75,
                    0.75, x, 1,
                },
                {
                    // 1, 2
                    x, x, x,
                    0.50, x, x,
                    0.50, x, x,
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
                    x, x, 0.75,
                    0.9375, 0.875, 0.75,
                },
                {
                    // 2, 2
                    0.50, x, x,
                    0.50, x, x,
                    0.50, x, x,
                },
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
    lue::test::check_arrays_are_equal(residue_we_got, residue_we_want);
}
