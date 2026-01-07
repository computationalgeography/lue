#define BOOST_TEST_MODULE lue framework algorithm accu_trigger
#include "flow_accumulation.hpp"
#include "lue/framework/algorithm/value_policies/accu_trigger.hpp"
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
    lue::PartitionedArray<MaterialElement, 2> const trigger_raster{};
    lue::Scalar<MaterialElement> const inflow_scalar{};
    lue::Scalar<MaterialElement> const trigger_scalar{};

    // raster, raster
    std::tie(outflow, residue) =
        lue::value_policies::accu_trigger(flow_direction, inflow_raster, trigger_raster);

    // scalar, scalar
    std::tie(outflow, residue) =
        lue::value_policies::accu_trigger(flow_direction, inflow_scalar, trigger_scalar);

    // TODO: Support once API lib is ready
    // // value, value
    // std::tie(outflow, residue) =
    //     lue::value_policies::accu_trigger(flow_direction, inflow_value, trigger_value);

    // raster, scalar
    std::tie(outflow, residue) =
        lue::value_policies::accu_trigger(flow_direction, inflow_raster, trigger_scalar);

    // TODO: Support once API lib is ready
    // // raster, value
    // std::tie(outflow, residue) =
    //     lue::value_policies::accu_trigger(flow_direction, inflow_raster, trigger_value);

    // scalar, raster
    std::tie(outflow, residue) =
        lue::value_policies::accu_trigger(flow_direction, inflow_scalar, trigger_raster);

    // TODO: Support once API lib is ready
    // // scalar, value
    // std::tie(outflow, residue) =
    //     lue::value_policies::accu_trigger(flow_direction, inflow_scalar, trigger_value);

    // TODO: Support once API lib is ready
    // // value, raster
    // std::tie(outflow, residue) =
    //     lue::value_policies::accu_trigger(flow_direction, inflow_value, trigger_raster);

    // TODO: Support once API lib is ready
    // // value, scalar
    // std::tie(outflow, residue) =
    //     lue::value_policies::accu_trigger(flow_direction, inflow_value, trigger_scalar);
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

    lue::Scalar<MaterialElement> const trigger = 1.5;

    auto const [outflow_we_got, residue_we_got] =
        lue::value_policies::accu_trigger(flow_direction, inflow, trigger);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            array_shape,
            partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    6.0,  0.0,  2.0,  2.0, 2.0,
                    6.5,  0.0,  6.0,  4.0, 2.0,
                    7.0,  6.5, 10.0,  4.0, 0.0,
                    0.0, 24.0,  6.0,  0.0, 0.0,
                    0.0, 54.0, 18.0, 12.0, 6.0,
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
                    0.0, 0.5, 0.0, 0.0, 0.0,
                    0.0, 0.5, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0, 0.0,
                    0.5, 0.0, 0.0, 0.0, 0.0,
                    0.5, 0.0, 0.0, 0.0, 0.0,
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

    auto const trigger = lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                0.0, 40.0,  9.0,  9.0,  9.0,
                0.0, 40.0,  9.0,  9.0,  9.0,
                0.0, 40.0, 30.0,  9.0,  9.0,
                0.0, 40.0,  9.0,  9.0, 50.0,
                0.0, 40.0, 40.0, 40.0, 50.0,
            }
            // clang-format on
            // NOLINTEND
        });

    auto const [outflow_we_got, residue_we_got] =
        lue::value_policies::accu_trigger(flow_direction, inflow, trigger);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            array_shape,
            partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    10.0,   0.0,  10.0, 10.0, 10.0,
                    20.0,   0.0,  30.0, 20.0, 10.0,
                    30.0,  40.0,  50.0, 20.0, 10.0,
                    10.0, 130.0,  80.0, 70.0, 50.0,
                    50.0, 420.0, 100.0, 50.0,  0.0,
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
                     0.0,  10.0,  0.0,  0.0,  0.0,
                     0.0,  10.0,  0.0,  0.0,  0.0,
                     0.0,   0.0,  0.0,  0.0,  0.0,
                     0.0,   0.0,  0.0,  0.0,  0.0,
                     0.0,   0.0,  0.0,  0.0, 49.0,
                }
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
    lue::test::check_arrays_are_equal(residue_we_got, residue_we_want);
}


BOOST_AUTO_TEST_CASE(pcraster_manual_example3)
{
    auto const flow_direction = lue::test::pcraster_example_flow_direction();
    auto const& array_shape{flow_direction.shape()};
    auto const partition_shape{array_shape};

    using MaterialElement = lue::FloatingPointElement<0>;

    auto const nd = lue::policy::no_data_value<MaterialElement>;

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

    auto const trigger = lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
        array_shape,
        partition_shape,
        {
            // NOLINTBEGIN
            // clang-format off
            {
                 nd, 40.0,  9.0,  9.0,  9.0,
                0.0, 40.0,  9.0,  9.0,  9.0,
                0.0, 40.0, 30.0,  9.0,  9.0,
                0.0, 40.0,  9.0,  9.0, 50.0,
                0.0, 40.0, 40.0, 40.0, 50.0,
            }
            // clang-format on
            // NOLINTEND
        });

    auto const [outflow_we_got, residue_we_got] =
        lue::value_policies::accu_trigger(flow_direction, inflow, trigger);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<MaterialElement, 2>>(
            array_shape,
            partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                      nd,   0.0,  10.0, 10.0, 10.0,
                      nd,   0.0,  30.0, 20.0, 10.0,
                      nd,  40.0,  50.0, 20.0, 10.0,
                    10.0,    nd,  80.0, 70.0, 50.0,
                    50.0,    nd, 100.0, 50.0,  0.0,
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
                      nd,  10.0,  0.0,  0.0,  0.0,
                      nd,  10.0,  0.0,  0.0,  0.0,
                      nd,   0.0,  0.0,  0.0,  0.0,
                     0.0,    nd,  0.0,  0.0,  0.0,
                     0.0,    nd,  0.0,  0.0, 49.0,
                }
                // clang-format on
                // NOLINTEND
            });

    lue::test::check_arrays_are_equal(outflow_we_got, outflow_we_want);
    lue::test::check_arrays_are_equal(residue_we_got, residue_we_want);
}
