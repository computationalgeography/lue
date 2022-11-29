#define BOOST_TEST_MODULE lue framework algorithm kinematic_wave
#include "lue/framework/algorithm/value_policies/kinematic_wave.hpp"
/// #include "lue/framework/algorithm/definition/kinematic_wave.hpp"  // iterate_to_new_discharge
#include "flow_accumulation.hpp"
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
        array_shape, partition_shape,
        {{
            10, 10, 10, 10, 10,
            10, 10, 10, 10, 10,
            10, 10, 10, 10, 10,
            10, 10, 10, 10, 50,
            50, 50, 50, 50, 49,
        }});

    auto const inflow = lue::create_partitioned_array<Element>(array_shape, partition_shape, 0);

    Element const alpha = 1.5;
    Element const beta = 0.6;
    Element const time_step_duration = 15;

    auto const channel_length = lue::create_partitioned_array<Element>(array_shape, partition_shape, 10);

    auto discharge_we_want = lue::test::create_partitioned_array<lue::PartitionedArray<Element, 2>>(
        array_shape, partition_shape,
        {{
             2.32293,  2.32293,  2.32293,  2.32293, 2.32293,
             4.00491,  4.00491,  5.76591,  4.00491, 2.32293,
             5.27401,  9.81514,  8.40418,  4.00491, 2.32293,
             2.32293, 21.22170,  9.68365,  9.60760, 7.19019,
             7.19019, 57.27763, 17.74742, 12.90772, 7.09124,
        }});

    auto const kinematic_wave = lue::value_policies::kinematic_wave(
        flow_direction, discharge, inflow, alpha, beta, time_step_duration, channel_length);

    lue::test::check_arrays_are_close(kinematic_wave, discharge_we_want);
}


/// //! some stuff that crashed
/// BOOST_AUTO_TEST_CASE(iterate1)
/// {
///     double const discharge = lue::detail::iterate_to_new_discharge<double>(
///         0.000201343,   // upstream_discharge
///         0.000115866,   // current_discharge
///         -0.000290263,  // inflow
///         1.73684,       // alpha
///         0.6,           // beta
///         15,            // time_step_duration
///         10);           // channel_length
/// 
/// 
///     BOOST_CHECK_EQUAL(discharge, 5.5);
///     // double retV = 0.000031450866300937;
/// 
/// 
///     // // adapt initial estimator Qkx twice
///     // //  Qkx   = MAX(Qkx, 1e-30);
///     // double cmpEps(1E-12);
///     // double v = IterateToQnew(
///     //                  /* Qin    */ 0.000201343,
///     //                  /* Qold   */ 0.000115866,
///     //                  /* q      */ -0.000290263,
///     //                  /* alpha  */ 1.73684,
///     //                  /* beta   */ 0.6,
///     //                  /* deltaT */ 15,
///     //                  /* deltaX */ 10,
///     //                  /* epsilon */ 1E-12);
///     // // printf("\n %20.18f \n",v);
///     // double retV = 0.000031450866300937;
///     // BOOST_CHECK(v > (retV-cmpEps) && v < (retV+cmpEps) );
/// }


/// //! more stuff that crashed
/// BOOST_AUTO_TEST_CASE(iterate2)
/// {
///  // does not terminate
///  double v = IterateToQnew(
///               /* Qin */ 0,
///               /* Qold */  1.11659e-07,
///               /* q */ -1.32678e-05,
///               /* alpha */ 1.6808,
///               /* beta */ 0.6,
///               /* deltaT */ 15,
///               /* deltaX */ 10,
///               /* epsilon */ 1E-12);
///  
///  BOOST_CHECK(v == 1e-30);
/// }
