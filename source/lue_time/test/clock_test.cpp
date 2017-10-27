#define BOOST_TEST_MODULE lue time clock
#include <boost/test/unit_test.hpp>
#include "lue/time/clock.hpp"
#include "lue/time/tick_period.hpp"


BOOST_AUTO_TEST_CASE(seconds)
{
    using namespace lue::time;

    // Clock with a tick period of 10 seconds
    std::size_t const nr_seconds_per_tick = 10;

    using TickPeriod = TickPeriod<Second>;
    using Clock = Clock<TickPeriod>;

    TickPeriod tick_period(nr_seconds_per_tick);
    Clock clock;
}
