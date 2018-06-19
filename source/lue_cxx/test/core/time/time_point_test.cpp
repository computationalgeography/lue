#define BOOST_TEST_MODULE lue core time point
#include <boost/test/unit_test.hpp>
#include "lue/core/time.hpp"


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


BOOST_AUTO_TEST_CASE(days)
{
    using namespace lue::time;

    // Types for tracking days
    using TickPeriod = TickPeriod<Day>;
    using Clock = Clock<TickPeriod>;
    using Duration = Clock::Duration;
    using TimePoint = TimePoint<Clock>;

    // Clock with a resolution of 2 days
    TickPeriod tick_period{2};
    Clock clock(tick_period);

    // Select day 6 (3 * 2 days)
    Duration duration{3};
    TimePoint day_6{duration};

    BOOST_CHECK_EQUAL(clock.nr_units(day_6), 6);
}
