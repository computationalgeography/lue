#define BOOST_TEST_MODULE lue core time point
#include "lue/core/time.hpp"
#include <boost/test/included/unit_test.hpp>


BOOST_AUTO_TEST_CASE(seconds)
{
    namespace ldm = lue::data_model;

    // Time point at 6 ticks in a clock with a tick period of 10 seconds,
    // so the point is located at 60 seconds
    std::size_t const nr_seconds_per_tick = 10;
    std::size_t const nr_ticks = 6;

    using TickPeriod = ldm::time::TickPeriod<ldm::time::Second>;
    using Clock = ldm::time::Clock<TickPeriod>;
    using Duration = Clock::Duration;
    using TimePoint = ldm::time::TimePoint<Clock>;

    TickPeriod tick_period{nr_seconds_per_tick};
    Clock clock{ldm::time::Epoch{}, tick_period};
    Duration duration{nr_ticks};
    TimePoint time_point{duration};

    BOOST_CHECK(time_point.duration() == duration);
}


BOOST_AUTO_TEST_CASE(days)
{
    namespace ldm = lue::data_model;

    // Types for tracking days
    using TickPeriod = ldm::time::TickPeriod<ldm::time::Day>;
    using Clock = ldm::time::Clock<TickPeriod>;
    using Duration = Clock::Duration;
    using TimePoint = ldm::time::TimePoint<Clock>;

    // Clock with a resolution of 2 days
    TickPeriod tick_period{2};
    Clock clock(ldm::time::Epoch{}, tick_period);

    // Select day 6 (3 * 2 days)
    Duration duration{3};
    TimePoint day_6{duration};

    BOOST_CHECK_EQUAL(clock.nr_units(day_6), 6);
}
