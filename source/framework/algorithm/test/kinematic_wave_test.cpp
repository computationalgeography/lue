#define BOOST_TEST_MODULE lue framework algorithm kinematic_wave
#include "flow_accumulation.hpp"
#include "lue/framework/algorithm/value_policies/all.hpp"
#include "lue/framework/algorithm/value_policies/greater_than.hpp"
#include "lue/framework/algorithm/value_policies/kinematic_wave.hpp"
#include "lue/framework/test/array.hpp"
#include "lue/framework/test/compare.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


BOOST_AUTO_TEST_CASE(pcraster_manual_example)
{
    auto flow_direction = lue::test::pcraster_example_flow_direction();

    auto const array_shape{flow_direction.shape()};
    auto const partition_shape{array_shape};

    using Element = double;

    auto const discharge = lue::test::create_partitioned_array<lue::PartitionedArray<Element, 2>>(
        array_shape,
        partition_shape,
        {{
            10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
            10, 10, 10, 10, 10, 10, 50, 50, 50, 50, 50, 49,
        }});

    auto const inflow = lue::create_partitioned_array<Element>(array_shape, partition_shape, 0);

    Element const alpha = 1.5;
    Element const beta = 0.6;
    Element const time_step_duration = 15;

    auto const channel_length = lue::create_partitioned_array<Element>(array_shape, partition_shape, 10);

    auto const discharge_we_want = lue::test::create_partitioned_array<lue::PartitionedArray<Element, 2>>(
        array_shape,
        partition_shape,
        {{
            2.32293, 2.32293, 2.32293, 2.32293,  2.32293,  4.00491,  4.00491, 5.76591,  4.00491,
            2.32293, 5.27401, 9.81514, 8.40418,  4.00491,  2.32293,  2.32293, 21.22170, 9.68365,
            9.60760, 7.19019, 7.19019, 57.27763, 17.74742, 12.90772, 7.09124,
        }});

    auto const kinematic_wave = lue::value_policies::kinematic_wave(
        flow_direction, discharge, inflow, alpha, beta, time_step_duration, channel_length);

    lue::test::check_arrays_are_close(kinematic_wave, discharge_we_want);
}


BOOST_AUTO_TEST_CASE(zero_discharge_and_inflow)
{
    auto flow_direction = lue::test::pcraster_example_flow_direction();

    auto const array_shape{flow_direction.shape()};
    auto const partition_shape{array_shape};

    using Element = double;

    auto const discharge = lue::create_partitioned_array<Element>(array_shape, partition_shape, 0);
    auto const inflow = lue::create_partitioned_array<Element>(array_shape, partition_shape, 0);

    Element const alpha = 1.5;
    Element const beta = 0.6;
    Element const time_step_duration = 15;

    auto const channel_length = lue::create_partitioned_array<Element>(array_shape, partition_shape, 10);

    auto const discharge_we_want = lue::create_partitioned_array<Element>(array_shape, partition_shape, 0);

    auto const kinematic_wave = lue::value_policies::kinematic_wave(
        flow_direction, discharge, inflow, alpha, beta, time_step_duration, channel_length);

    lue::test::check_arrays_are_close(kinematic_wave, discharge_we_want);
}


BOOST_AUTO_TEST_CASE(non_zero_discharge_and_zero_inflow)
{
    auto flow_direction = lue::test::pcraster_example_flow_direction();

    auto const array_shape{flow_direction.shape()};
    auto const partition_shape{array_shape};

    using Element = double;

    auto const discharge = lue::create_partitioned_array<Element>(array_shape, partition_shape, 1);
    auto const inflow = lue::create_partitioned_array<Element>(array_shape, partition_shape, 0);

    Element const alpha = 1.5;
    Element const beta = 0.6;
    Element const time_step_duration = 15;

    auto const channel_length = lue::create_partitioned_array<Element>(array_shape, partition_shape, 10);

    auto const kinematic_wave = lue::value_policies::kinematic_wave(
        flow_direction, discharge, inflow, alpha, beta, time_step_duration, channel_length);

    using namespace lue::value_policies;

    BOOST_CHECK(lue::value_policies::all(kinematic_wave > Element{0}).get());
}


BOOST_AUTO_TEST_CASE(zero_discharge_and_non_zero_inflow)
{
    auto flow_direction = lue::test::pcraster_example_flow_direction();

    auto const array_shape{flow_direction.shape()};
    auto const partition_shape{array_shape};

    using Element = double;

    auto const discharge = lue::create_partitioned_array<Element>(array_shape, partition_shape, 0);
    auto const inflow = lue::create_partitioned_array<Element>(array_shape, partition_shape, 1);

    Element const alpha = 1.5;
    Element const beta = 0.6;
    Element const time_step_duration = 15;

    auto const channel_length = lue::create_partitioned_array<Element>(array_shape, partition_shape, 10);

    auto const kinematic_wave = lue::value_policies::kinematic_wave(
        flow_direction, discharge, inflow, alpha, beta, time_step_duration, channel_length);

    using namespace lue::value_policies;

    BOOST_CHECK(lue::value_policies::all(kinematic_wave > Element{0}).get());
}


// TODO Add tests for extreme inputs:
// - What about "wrong" values for alpha, beta, time_step_duration?
