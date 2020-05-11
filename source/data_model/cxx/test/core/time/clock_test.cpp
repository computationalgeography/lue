#define BOOST_TEST_MODULE lue core time clock
#include <boost/test/unit_test.hpp>
#include "lue/core/time.hpp"


BOOST_AUTO_TEST_CASE(seconds)
{
    namespace ldm = lue::data_model;

    // Clock with a Unix time epoch and a tick period of 10 seconds
    std::size_t const nr_seconds_per_tick = 10;

    using TickPeriod = ldm::time::TickPeriod<ldm::time::Second>;
    using Clock = ldm::time::Clock<TickPeriod>;

    TickPeriod tick_period(nr_seconds_per_tick);
    Clock clock;

    BOOST_CHECK(clock.epoch().kind() == ldm::time::Epoch::Kind::common_era);
    BOOST_REQUIRE(clock.epoch().origin());
    BOOST_CHECK_EQUAL(*clock.epoch().origin(), "1970-01-01T00:00:00+00:00");
    BOOST_REQUIRE(clock.epoch().calendar());
    BOOST_CHECK(*clock.epoch().calendar() == ldm::time::Calendar::gregorian);
}
