#define BOOST_TEST_MODULE lue core time tick_period
#include "lue/core/time.hpp"
#include <boost/test/included/unit_test.hpp>


BOOST_AUTO_TEST_CASE(seconds)
{
    namespace ldm = lue::data_model;

    // Every 5 * (1 seconds) a tick
    std::size_t const nr_seconds = 5;

    ldm::time::TickPeriod<ldm::time::Second> tick_period(nr_seconds);

    BOOST_CHECK_EQUAL(tick_period.nr_units(), nr_seconds);
}
