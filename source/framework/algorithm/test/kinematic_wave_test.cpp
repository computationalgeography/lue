#define BOOST_TEST_MODULE lue framework algorithm accu_kinematic_wave
#include "flow_accumulation.hpp"
#include "lue/framework/algorithm/definition/kinematic_wave.hpp"  // iterate_to_new_discharge
#include "lue/framework/algorithm/value_policies/all.hpp"
#include "lue/framework/algorithm/value_policies/greater_than.hpp"
#include "lue/framework/algorithm/value_policies/kinematic_wave.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


BOOST_AUTO_TEST_CASE(overloads)
{
    using FlowDirectionElement = lue::FlowDirectionElement;
    using FloatingPointElement = lue::FloatingPointElement<0>;

    lue::PartitionedArray<FlowDirectionElement, 2> const flow_direction{};
    [[maybe_unused]] lue::PartitionedArray<FloatingPointElement, 2> outflow{};
    [[maybe_unused]] lue::PartitionedArray<FloatingPointElement, 2> residue{};

    // TODO: Which combinations make sense and which don't?
    // TODO: Revisit all routing overloads: supporting scalars only makes sense if these arguments are
    //       typically computed. Don't support them if that does not make sense.
    lue::PartitionedArray<FloatingPointElement, 2> const current_outflow_raster{};
    lue::PartitionedArray<FloatingPointElement, 2> const inflow_raster{};
    lue::PartitionedArray<FloatingPointElement, 2> const alpha_raster{};
    lue::PartitionedArray<FloatingPointElement, 2> const beta_raster{};
    lue::PartitionedArray<FloatingPointElement, 2> const channel_length_raster{};

    lue::Scalar<FloatingPointElement> const alpha_scalar{};
    lue::Scalar<FloatingPointElement> const beta_scalar{};
    lue::Scalar<FloatingPointElement> const time_step_duration_scalar{};
    lue::Scalar<FloatingPointElement> const channel_length_scalar{};

    {
        auto const outflow = lue::value_policies::kinematic_wave(
            flow_direction,
            current_outflow_raster,
            inflow_raster,
            alpha_raster,
            beta_raster,
            time_step_duration_scalar,
            channel_length_raster);
    }

    {
        auto const outflow = lue::value_policies::kinematic_wave(
            flow_direction,
            current_outflow_raster,
            inflow_raster,
            alpha_scalar,
            beta_scalar,
            time_step_duration_scalar,
            channel_length_scalar);
    }
}


// namespace tt = boost::test_tools;


BOOST_AUTO_TEST_CASE(pcraster_manual_example)
{
    // https://pcraster.geo.uu.nl/pcraster/4.4.1/documentation/pcraster_manual/sphinx/op_kinematic.html
    auto const flow_direction = lue::test::pcraster_example_flow_direction();
    auto const& array_shape{flow_direction.shape()};
    auto const partition_shape{array_shape};

    using FloatingPointElement = lue::FloatingPointElement<0>;

    auto const current_outflow =
        lue::test::create_partitioned_array<lue::PartitionedArray<FloatingPointElement, 2>>(
            array_shape,
            partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    10, 10, 10, 10, 10,
                    10, 10, 10, 10, 10,
                    10, 10, 10, 10, 10,
                    10, 10, 10, 10, 50,
                    50, 50, 50, 50, 49,
                }
                // clang-format on
                // NOLINTEND
            });

    auto const inflow =
        lue::create_partitioned_array<FloatingPointElement, 2>(array_shape, partition_shape, 0);
    lue::Scalar<FloatingPointElement> const alpha{1.5};
    lue::Scalar<FloatingPointElement> const beta{0.6};
    lue::Scalar<FloatingPointElement> const time_step_duration{15};
    lue::Scalar<FloatingPointElement> const channel_length{10};

    auto const outflow_we_got = lue::value_policies::kinematic_wave(
        flow_direction, current_outflow, inflow, alpha, beta, time_step_duration, channel_length);

    auto const outflow_we_want =
        lue::test::create_partitioned_array<lue::PartitionedArray<FloatingPointElement, 2>>(
            array_shape,
            partition_shape,
            {
                // NOLINTBEGIN
                // clang-format off
                {
                    2.32293,  2.32293,  2.32293,  2.32293, 2.32293,
                    4.00491,  4.00491,  5.76591,  4.00491, 2.32293,
                    5.27401,  9.81514,  8.40418,  4.00491, 2.32293,
                    2.32293, 21.22170,  9.68365,  9.60760, 7.19019,
                    7.19019, 57.27763, 17.74742, 12.90772, 7.09124,
                }
                // clang-format on            FloatingPointElement
                // NOLINTEND
            });

    lue::test::check_arrays_are_close(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(zero_discharge_and_inflow)
{
    auto const flow_direction = lue::test::pcraster_example_flow_direction();
    auto const& array_shape{flow_direction.shape()};
    auto const partition_shape{array_shape};

    using FloatingPointElement = lue::FloatingPointElement<0>;

    auto const current_outflow =
        lue::create_partitioned_array<FloatingPointElement>(array_shape, partition_shape, 0);
    auto const inflow = lue::create_partitioned_array<FloatingPointElement, 2>(array_shape, partition_shape, 0);
    lue::Scalar<FloatingPointElement> const alpha{1.5};
    lue::Scalar<FloatingPointElement> const beta{0.6};
    lue::Scalar<FloatingPointElement> const time_step_duration{15};
    lue::Scalar<FloatingPointElement> const channel_length{10};

    auto const outflow_we_got = lue::value_policies::kinematic_wave(
        flow_direction, current_outflow, inflow, alpha, beta, time_step_duration, channel_length);

    auto const outflow_we_want =
        lue::create_partitioned_array<FloatingPointElement>(array_shape, partition_shape, 0);

    lue::test::check_arrays_are_close(outflow_we_got, outflow_we_want);
}


BOOST_AUTO_TEST_CASE(non_zero_discharge_and_zero_inflow)
{
    auto const flow_direction = lue::test::pcraster_example_flow_direction();
    auto const& array_shape{flow_direction.shape()};
    auto const partition_shape{array_shape};

    using FloatingPointElement = lue::FloatingPointElement<0>;

    auto const current_outflow =
        lue::create_partitioned_array<FloatingPointElement>(array_shape, partition_shape, 1);
    auto const inflow = lue::create_partitioned_array<FloatingPointElement, 2>(array_shape, partition_shape, 0);
    lue::Scalar<FloatingPointElement> const alpha{1.5};
    lue::Scalar<FloatingPointElement> const beta{0.6};
    lue::Scalar<FloatingPointElement> const time_step_duration{15};
    lue::Scalar<FloatingPointElement> const channel_length{10};

    auto const outflow_we_got = lue::value_policies::kinematic_wave(
        flow_direction, current_outflow, inflow, alpha, beta, time_step_duration, channel_length);

    using namespace lue::value_policies;

    BOOST_CHECK(lue::value_policies::all(outflow_we_got > FloatingPointElement{0}).future().get());
}


BOOST_AUTO_TEST_CASE(zero_discharge_and_non_zero_inflow)
{
    auto const flow_direction = lue::test::pcraster_example_flow_direction();
    auto const& array_shape{flow_direction.shape()};
    auto const partition_shape{array_shape};

    using FloatingPointElement = lue::FloatingPointElement<0>;

    auto const current_outflow =
        lue::create_partitioned_array<FloatingPointElement>(array_shape, partition_shape, 0);
    auto const inflow = lue::create_partitioned_array<FloatingPointElement, 2>(array_shape, partition_shape, 1);
    lue::Scalar<FloatingPointElement> const alpha{1.5};
    lue::Scalar<FloatingPointElement> const beta{0.6};
    lue::Scalar<FloatingPointElement> const time_step_duration{15};
    lue::Scalar<FloatingPointElement> const channel_length{10};

    auto const outflow_we_got = lue::value_policies::kinematic_wave(
        flow_direction, current_outflow, inflow, alpha, beta, time_step_duration, channel_length);

    using namespace lue::value_policies;

    BOOST_CHECK(lue::value_policies::all(outflow_we_got > FloatingPointElement{0}).future().get());
}


// TODO: Add tests for extreme inputs:
// - What about "wrong" values for alpha, beta, time_step_duration?


BOOST_AUTO_TEST_CASE(dry_cell)
{
    using FloatingPoint = lue::FloatingPointElement<0>;

    {
        FloatingPoint const outflow_we_got{lue::detail::iterate_to_new_discharge<FloatingPoint>(
            0,     // upstream_discharge
            0,     // current_outflow
            0,     // lateral_inflow
            1.5,   // alpha
            0.6,   // beta
            15,    // time_step_duration
            10)};  // channel_length
        FloatingPoint const outflow_we_want{0};

        BOOST_TEST(outflow_we_got == outflow_we_want, tt::tolerance(1e-6));
    }

    {
        FloatingPoint const outflow_we_got{lue::detail::iterate_to_new_discharge<FloatingPoint>(
            1,     // upstream_discharge
            0,     // current_outflow
            -1,    // lateral_inflow
            1.5,   // alpha
            0.6,   // beta
            15,    // time_step_duration
            10)};  // channel_length
        FloatingPoint const outflow_we_want{0};

        BOOST_TEST(outflow_we_got == outflow_we_want, tt::tolerance(1e-6));
    }

    {
        FloatingPoint const outflow_we_got{lue::detail::iterate_to_new_discharge<FloatingPoint>(
            1,     // upstream_discharge
            1,     // current_outflow
            -2,    // lateral_inflow
            1.5,   // alpha
            0.6,   // beta
            15,    // time_step_duration
            10)};  // channel_length
        FloatingPoint const outflow_we_want{0};

        BOOST_TEST(outflow_we_got == outflow_we_want, tt::tolerance(1e-6));
    }

    {
        FloatingPoint const outflow_we_got{lue::detail::iterate_to_new_discharge<FloatingPoint>(
            0,     // upstream_discharge
            1,     // current_outflow
            -1,    // lateral_inflow
            1.5,   // alpha
            0.6,   // beta
            15,    // time_step_duration
            10)};  // channel_length
        FloatingPoint const outflow_we_want{0};

        BOOST_TEST(outflow_we_got == outflow_we_want, tt::tolerance(1e-6));
    }

    {
        FloatingPoint const outflow_we_got{lue::detail::iterate_to_new_discharge<FloatingPoint>(
            0,     // upstream_discharge
            0,     // current_outflow
            -1,    // lateral_inflow
            1.5,   // alpha
            0.6,   // beta
            15,    // time_step_duration
            10)};  // channel_length
        FloatingPoint const outflow_we_want{0};

        BOOST_TEST(outflow_we_got == outflow_we_want, tt::tolerance(1e-6));
    }
}


// BOOST_AUTO_TEST_CASE(crashed_in_pcraster1)
// {
//     double const outflow_we_got{lue::detail::iterate_to_new_discharge<double>(
//         0.000201343,   // upstream_discharge
//         0.000115866,   // current_outflow
//         -0.000290263,  // lateral_inflow
//         1.73684,       // alpha
//         0.6,           // beta
//         15,            // time_step_duration
//         10)};          // channel_length
//     // /* epsilon */ 1E-12);  // epsilon
//     double const outflow_we_want{0.000031450866300937};
//
//     BOOST_TEST(outflow_we_got == outflow_we_want, tt::tolerance(1e-6));
// }


BOOST_AUTO_TEST_CASE(crashed_in_pcraster2)
{
    if constexpr (lue::arithmetic_element_supported<double>)
    {
        // TODO Port to all float types
        using FloatingPoint = double;

        FloatingPoint const outflow_we_got{lue::detail::iterate_to_new_discharge<FloatingPoint>(
            0,             // upstream_discharge
            1.11659e-07,   // current_outflow
            -1.32678e-05,  // lateral_inflow
            1.6808,        // alpha
            0.6,           // beta
            15,            // time_step_duration
            10)};          // channel_length

        FloatingPoint const outflow_we_want{std::numeric_limits<FloatingPoint>::min()};

        BOOST_TEST(outflow_we_got == outflow_we_want, tt::tolerance(1e-6));
    }
}


BOOST_AUTO_TEST_CASE(random_input)
{
    using FloatingPoint = lue::FloatingPointElement<0>;

    std::random_device random_device{};
    std::default_random_engine random_number_engine(random_device());

    std::uniform_real_distribution<FloatingPoint> discharge_distribution{0, 1000};
    std::uniform_real_distribution<FloatingPoint> lateral_inflow_distribution{-1000, 1000};
    std::uniform_real_distribution<FloatingPoint> alpha_distribution{0.5, 6.0};
    std::uniform_real_distribution<FloatingPoint> beta_distribution{0.5, 2.0};
    std::uniform_real_distribution<FloatingPoint> time_step_duration_distribution{1, 100};
    std::uniform_real_distribution<FloatingPoint> channel_length_distribution{1, 100};

    for (std::size_t i = 0; i < 10000; ++i)
    {
        FloatingPoint const upstream_discharge{discharge_distribution(random_number_engine)};
        FloatingPoint const current_outflow{discharge_distribution(random_number_engine)};
        FloatingPoint const alpha{alpha_distribution(random_number_engine)};
        FloatingPoint const beta{beta_distribution(random_number_engine)};
        FloatingPoint const time_step_duration{time_step_duration_distribution(random_number_engine)};
        FloatingPoint const channel_length{channel_length_distribution(random_number_engine)};

        FloatingPoint const lateral_inflow{
            lateral_inflow_distribution(random_number_engine) / channel_length};

        FloatingPoint outflow_we_got{-1};

        BOOST_TEST_INFO(
            std::format(
                "upstream_discharge: {}\n"
                "current_outflow: {}\n"
                "lateral_inflow: {}\n"
                "alpha: {}\n"
                "beta: {}\n"
                "time_step_duration: {}\n"
                "channel_length: {}\n",
                upstream_discharge,
                current_outflow,
                lateral_inflow,
                alpha,
                beta,
                time_step_duration,
                channel_length));

        // This call should not throw an exception
        outflow_we_got = lue::detail::iterate_to_new_discharge<FloatingPoint>(
            upstream_discharge,
            current_outflow,
            lateral_inflow,
            alpha,
            beta,
            time_step_duration,
            channel_length);

        BOOST_CHECK(outflow_we_got >= 0);
    }
}
