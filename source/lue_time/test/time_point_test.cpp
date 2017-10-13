#define BOOST_TEST_MODULE lue time point
#include <boost/test/unit_test.hpp>
#include "lue/time/clock.hpp"
#include "lue/time/tick_period.hpp"
#include "lue/time/time_point.hpp"


BOOST_AUTO_TEST_CASE(seconds)
{
    using namespace lue::time;

    // Time point at 6 ticks in a clock with a tick period of 10 seconds,
    // so the point is located at 60 seconds
    std::size_t const nr_seconds_per_tick = 10;
    std::size_t const nr_ticks = 6;

    using TickPeriod = TickPeriod<Second>;
    using Clock = Clock<TickPeriod>;
    using Duration = Clock::Duration;
    using TimePoint = TimePoint<Clock>;

    TickPeriod tick_period{nr_seconds_per_tick};
    Clock clock{tick_period};
    Duration duration{nr_ticks};
    TimePoint time_point{duration};

    BOOST_CHECK(time_point.duration() == duration);
}
