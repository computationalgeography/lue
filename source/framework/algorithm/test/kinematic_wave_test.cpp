#define BOOST_TEST_MODULE lue framework algorithm kinematic_wave
#include "flow_accumulation.hpp"
#include "lue/framework/algorithm/definition/kinematic_wave.hpp"
#include "lue/framework/algorithm/value_policies/all.hpp"
#include "lue/framework/algorithm/value_policies/greater_than.hpp"
#include "lue/framework/algorithm/value_policies/kinematic_wave.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace tt = boost::test_tools;


BOOST_AUTO_TEST_CASE(pcraster_manual_example)
{
    // https://pcraster.geo.uu.nl/pcraster/4.4.1/documentation/pcraster_manual/sphinx/op_kinematic.html
    auto flow_direction = lue::test::pcraster_example_flow_direction();

    auto const array_shape{flow_direction.shape()};
    auto const partition_shape{array_shape};

    using Element = double;

    auto const current_discharge = lue::test::create_partitioned_array<lue::PartitionedArray<Element, 2>>(
        array_shape,
        partition_shape,
        // clang-format off
        {{
            10, 10, 10, 10, 10,
            10, 10, 10, 10, 10,
            10, 10, 10, 10, 10,
            10, 10, 10, 10, 50,
            50, 50, 50, 50, 49,
        }}  // clang-format on
    );

    auto const inflow = lue::create_partitioned_array<Element>(array_shape, partition_shape, 0);

    Element const alpha = 1.5;
    Element const beta = 0.6;
    Element const time_step_duration = 15;

    auto const channel_length = lue::create_partitioned_array<Element>(array_shape, partition_shape, 10);

    auto const discharge_we_want = lue::test::create_partitioned_array<lue::PartitionedArray<Element, 2>>(
        array_shape,
        partition_shape,
        // clang-format off
        {{
            2.32293,  2.32293,  2.32293,  2.32293, 2.32293,
            4.00491,  4.00491,  5.76591,  4.00491, 2.32293,
            5.27401,  9.81514,  8.40418,  4.00491, 2.32293,
            2.32293, 21.22170,  9.68365,  9.60760, 7.19019,
            7.19019, 57.27763, 17.74742, 12.90772, 7.09124,
        }}  // clang-format on
    );

    auto const new_discharge = lue::value_policies::kinematic_wave(
        flow_direction, current_discharge, inflow, alpha, beta, time_step_duration, channel_length);

    lue::test::check_arrays_are_close(new_discharge, discharge_we_want);
}


BOOST_AUTO_TEST_CASE(zero_discharge_and_inflow)
{
    auto flow_direction = lue::test::pcraster_example_flow_direction();

    auto const array_shape{flow_direction.shape()};
    auto const partition_shape{array_shape};

    using Element = double;

    auto const current_discharge = lue::create_partitioned_array<Element>(array_shape, partition_shape, 0);
    auto const inflow = lue::create_partitioned_array<Element>(array_shape, partition_shape, 0);

    Element const alpha = 1.5;
    Element const beta = 0.6;
    Element const time_step_duration = 15;

    auto const channel_length = lue::create_partitioned_array<Element>(array_shape, partition_shape, 10);

    auto const discharge_we_want = lue::create_partitioned_array<Element>(array_shape, partition_shape, 0);

    auto const new_discharge = lue::value_policies::kinematic_wave(
        flow_direction, current_discharge, inflow, alpha, beta, time_step_duration, channel_length);

    lue::test::check_arrays_are_close(new_discharge, discharge_we_want);
}


BOOST_AUTO_TEST_CASE(non_zero_discharge_and_zero_inflow)
{
    auto flow_direction = lue::test::pcraster_example_flow_direction();

    auto const array_shape{flow_direction.shape()};
    auto const partition_shape{array_shape};

    using Element = double;

    auto const current_discharge = lue::create_partitioned_array<Element>(array_shape, partition_shape, 1);
    auto const inflow = lue::create_partitioned_array<Element>(array_shape, partition_shape, 0);

    Element const alpha = 1.5;
    Element const beta = 0.6;
    Element const time_step_duration = 15;

    auto const channel_length = lue::create_partitioned_array<Element>(array_shape, partition_shape, 10);

    auto const new_discharge = lue::value_policies::kinematic_wave(
        flow_direction, current_discharge, inflow, alpha, beta, time_step_duration, channel_length);

    using namespace lue::value_policies;

    BOOST_CHECK(lue::value_policies::all(new_discharge > Element{0}).get());
}


BOOST_AUTO_TEST_CASE(zero_discharge_and_non_zero_inflow)
{
    auto flow_direction = lue::test::pcraster_example_flow_direction();

    auto const array_shape{flow_direction.shape()};
    auto const partition_shape{array_shape};

    using Element = double;

    auto const current_discharge = lue::create_partitioned_array<Element>(array_shape, partition_shape, 0);
    auto const inflow = lue::create_partitioned_array<Element>(array_shape, partition_shape, 1);

    Element const alpha = 1.5;
    Element const beta = 0.6;
    Element const time_step_duration = 15;

    auto const channel_length = lue::create_partitioned_array<Element>(array_shape, partition_shape, 10);

    auto const new_discharge = lue::value_policies::kinematic_wave(
        flow_direction, current_discharge, inflow, alpha, beta, time_step_duration, channel_length);

    using namespace lue::value_policies;

    BOOST_CHECK(lue::value_policies::all(new_discharge > Element{0}).get());
}


// TODO Add tests for extreme inputs:
// - What about "wrong" values for alpha, beta, time_step_duration?


BOOST_AUTO_TEST_CASE(dry_cell)
{
    {
        double const new_discharge{lue::detail::iterate_to_new_discharge<double>(
            0,     // upstream_discharge
            0,     // current_discharge
            0,     // lateral_inflow
            1.5,   // alpha
            0.6,   // beta
            15,    // time_step_duration
            10)};  // channel_length
        double const discharge_we_want{0};

        BOOST_TEST(new_discharge == discharge_we_want, tt::tolerance(1e-6));
    }

    {
        double const new_discharge{lue::detail::iterate_to_new_discharge<double>(
            1,     // upstream_discharge
            0,     // current_discharge
            -1,    // lateral_inflow
            1.5,   // alpha
            0.6,   // beta
            15,    // time_step_duration
            10)};  // channel_length
        double const discharge_we_want{0};

        BOOST_TEST(new_discharge == discharge_we_want, tt::tolerance(1e-6));
    }

    {
        double const new_discharge{lue::detail::iterate_to_new_discharge<double>(
            1,     // upstream_discharge
            1,     // current_discharge
            -2,    // lateral_inflow
            1.5,   // alpha
            0.6,   // beta
            15,    // time_step_duration
            10)};  // channel_length
        double const discharge_we_want{0};

        BOOST_TEST(new_discharge == discharge_we_want, tt::tolerance(1e-6));
    }

    {
        double const new_discharge{lue::detail::iterate_to_new_discharge<double>(
            0,     // upstream_discharge
            1,     // current_discharge
            -1,    // lateral_inflow
            1.5,   // alpha
            0.6,   // beta
            15,    // time_step_duration
            10)};  // channel_length
        double const discharge_we_want{0};

        BOOST_TEST(new_discharge == discharge_we_want, tt::tolerance(1e-6));
    }

    {
        double const new_discharge{lue::detail::iterate_to_new_discharge<double>(
            0,     // upstream_discharge
            0,     // current_discharge
            -1,    // lateral_inflow
            1.5,   // alpha
            0.6,   // beta
            15,    // time_step_duration
            10)};  // channel_length
        double const discharge_we_want{0};

        BOOST_TEST(new_discharge == discharge_we_want, tt::tolerance(1e-6));
    }
}


BOOST_AUTO_TEST_CASE(crashed_in_pcraster1)
{
    double const new_discharge{lue::detail::iterate_to_new_discharge<double>(
        0.000201343,   // upstream_discharge
        0.000115866,   // current_discharge
        -0.000290263,  // lateral_inflow
        1.73684,       // alpha
        0.6,           // beta
        15,            // time_step_duration
        10)};          // channel_length
    // /* epsilon */ 1E-12);  // epsilon
    double const discharge_we_want{0.000031450866300937};

    BOOST_TEST(new_discharge == discharge_we_want, tt::tolerance(1e-6));
}


BOOST_AUTO_TEST_CASE(crashed_in_pcraster2)
{
    double const new_discharge{lue::detail::iterate_to_new_discharge<double>(
        0,             // upstream_discharge
        1.11659e-07,   // current_discharge
        -1.32678e-05,  // lateral_inflow
        1.6808,        // alpha
        0.6,           // beta
        15,            // time_step_duration
        10)};          // channel_length
    // /* epsilon */ 1E-12);  // epsilon

    double const discharge_we_want{1e-30};

    BOOST_TEST(new_discharge == discharge_we_want, tt::tolerance(1e-6));
}
