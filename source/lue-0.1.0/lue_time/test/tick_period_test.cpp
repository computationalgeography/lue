#define BOOST_TEST_MODULE lue time tick_period
#include <boost/test/unit_test.hpp>
#include "lue/time/tick_period.hpp"


BOOST_AUTO_TEST_CASE(seconds)
{
    using namespace lue::time;

    // Every 5 * (1 seconds) a tick
    std::size_t const nr_seconds = 5;

    TickPeriod<Second> tick_period(nr_seconds);

    BOOST_CHECK_EQUAL(tick_period.nr_units(), nr_seconds);
}
