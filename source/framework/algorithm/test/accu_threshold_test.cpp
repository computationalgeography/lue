#define BOOST_TEST_MODULE lue framework algorithm accu_threshold
#include "flow_accumulation.hpp"
#include "lue/framework/algorithm/value_policies/accu_threshold.hpp"
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
    lue::PartitionedArray<MaterialElement, 2> const threshold_raster{};
    lue::Scalar<MaterialElement> const inflow_scalar{};
    lue::Scalar<MaterialElement> const threshold_scalar{};

    // raster, raster
    std::tie(outflow, residue) =
        lue::value_policies::accu_threshold(flow_direction, inflow_raster, threshold_raster);

    // scalar, scalar
    std::tie(outflow, residue) =
        lue::value_policies::accu_threshold(flow_direction, inflow_scalar, threshold_scalar);

    // TODO: Support once API lib is ready
    // // value, value
    // std::tie(outflow, residue) =
    //     lue::value_policies::accu_threshold(flow_direction, inflow_value, threshold_value);

    // raster, scalar
    std::tie(outflow, residue) =
        lue::value_policies::accu_threshold(flow_direction, inflow_raster, threshold_scalar);

    // TODO: Support once API lib is ready
    // // raster, value
    // std::tie(outflow, residue) =
    //     lue::value_policies::accu_threshold(flow_direction, inflow_raster, threshold_value);

    // scalar, raster
    std::tie(outflow, residue) =
        lue::value_policies::accu_threshold(flow_direction, inflow_scalar, threshold_raster);

    // TODO: Support once API lib is ready
    // // scalar, value
    // std::tie(outflow, residue) =
    //     lue::value_policies::accu_threshold(flow_direction, inflow_scalar, threshold_value);

    // TODO: Support once API lib is ready
    // // value, raster
    // std::tie(outflow, residue) =
    //     lue::value_policies::accu_threshold(flow_direction, inflow_value, threshold_raster);

    // TODO: Support once API lib is ready
    // // value, scalar
    // std::tie(outflow, residue) =
    //     lue::value_policies::accu_threshold(flow_direction, inflow_value, threshold_scalar);
}


BOOST_AUTO_TEST_CASE(pcraster_manual_example1)
{
    auto const flow_direction = lue::test::pcraster_example_flow_direction();
    auto const& array_shape{flow_direction.shape()};
    auto const partition_shape{array_shape};

    using MaterialElement = lue::FloatingPointElement<0>;

    auto const inflow = lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                6.0, 0.5, 2.0, 2.0, 2.0,
                0.5, 0.5, 2.0, 2.0, 2.0,
                0.5, 0.5, 2.0, 2.0, 0.0,
                0.5, 0.5, 6.0, 0.0, 0.0,
                0.5, 6.0, 6.0, 6.0, 6.0,
            }
            // clang-format on
            // NOLINTEND
        });

    lue::Scalar<MaterialElement> const threshold = 1.5;

    auto const [outflow_we_got, residue_we_got] =
        lue::value_policies::accu_threshold(flow_direction, inflow, threshold);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            array_shape,
            partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    4.5, 0.0, 0.5, 0.5, 0.5,
                    3.5, 0.0, 1.5, 1.0, 0.5,
                    2.5, 0.5, 2.5, 1.0, 0.0,
                    0.0, 4.5, 4.5, 0.0, 0.0,
                    0.0, 27.0, 13.5, 9.0, 4.5,
                }
                // clang-format on
                // NOLINTEND
            });

    auto const residue_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            array_shape,
            partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    1.5, 0.5, 1.5, 1.5, 1.5,
                    1.5, 0.5, 1.5, 1.5, 1.5,
                    1.5, 1.5, 1.5, 1.5, 0.0,
                    0.5, 1.5, 1.5, 0.0, 0.0,
                    0.5, 1.5, 1.5, 1.5, 1.5,
                }
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
    lue::test::check_arrays_are_equal(residue_we_got, residue_we_want);
}


BOOST_AUTO_TEST_CASE(pcraster_manual_example2)
{
    auto const flow_direction = lue::test::pcraster_example_flow_direction();
    auto const& array_shape{flow_direction.shape()};
    auto const partition_shape{array_shape};

    using MaterialElement = lue::FloatingPointElement<0>;

    auto const inflow = lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                10.0, 10.0, 10.0, 10.0, 10.0,
                10.0, 10.0, 10.0, 10.0, 10.0,
                10.0, 10.0, 10.0, 10.0, 10.0,
                10.0, 10.0, 10.0, 10.0, 50.0,
                50.0, 50.0, 50.0, 50.0, 49.0,
            }
            // clang-format on
            // NOLINTEND
        });

    auto const threshold = lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                0.0, 40.0, 9.0, 9.0, 9.0,
                0.0, 40.0, 9.0, 9.0, 9.0,
                0.0, 40.0, 30.0, 9.0, 9.0,
                0.0, 40.0, 9.0, 9.0, 50.0,
                0.0, 40.0, 40.0, 40.0, 50.0,
            }
            // clang-format on
            // NOLINTEND
        });

    auto const [outflow_we_got, residue_we_got] =
        lue::value_policies::accu_threshold(flow_direction, inflow, threshold);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            array_shape,
            partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    10.0, 0.0, 1.0, 1.0, 1.0,
                    20.0, 0.0, 3.0, 2.0, 1.0,
                    30.0, 0.0, 0.0, 2.0, 1.0,
                    10.0, 0.0, 3.0, 2.0, 0.0,
                    50.0, 93.0, 20.0, 10.0, 0.0,
                }
                // clang-format on
                // NOLINTEND
            });

    auto const residue_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            array_shape,
            partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    0.0, 10.0, 9.0, 9.0, 9.0,
                    0.0, 10.0, 9.0, 9.0, 9.0,
                    0.0, 13.0, 14.0, 9.0, 9.0,
                    0.0, 40.0, 9.0, 9.0, 50.0,
                    0.0, 40.0, 40.0, 40.0, 49.0,
                }
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
    lue::test::check_arrays_are_equal(residue_we_got, residue_we_want);
}


BOOST_AUTO_TEST_CASE(parallel_east)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using MaterialElement = lue::FloatingPointElement<0>;

    auto const flow_direction = lue::create_partitioned_array<FlowDirectionElement, 2>(
        lue::test::array_shape, lue::test::partition_shape, lue::test::e);
    lue::Scalar<MaterialElement> const inflow = 1;
    lue::Scalar<MaterialElement> const threshold = 0;

    auto const [outflow_we_got, residue_we_got] =
        lue::value_policies::accu_threshold(flow_direction, inflow, threshold);

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
                    0, 0, 0,
                    0, 0, 0,
                    0, 0, 0,
                },
                {
                    0, 0, 0,
                    0, 0, 0,
                    0, 0, 0,
                },
                {
                    0, 0, 0,
                    0, 0, 0,
                    0, 0, 0,
                },
                {
                    0, 0, 0,
                    0, 0, 0,
                    0, 0, 0,
                },
                {
                    0, 0, 0,
                    0, 0, 0,
                    0, 0, 0,
                },
                {
                    0, 0, 0,
                    0, 0, 0,
                    0, 0, 0,
                },
                {
                    0, 0, 0,
                    0, 0, 0,
                    0, 0, 0,
                },
                {
                    0, 0, 0,
                    0, 0, 0,
                    0, 0, 0,
                },
                {
                    0, 0, 0,
                    0, 0, 0,
                    0, 0, 0,
                }
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
    lue::test::check_arrays_are_equal(residue_we_got, residue_we_want);
}
