#define BOOST_TEST_MODULE lue core time duration
#include "lue/core/time.hpp"
#include <boost/test/unit_test.hpp>


namespace ldm = lue::data_model;


BOOST_AUTO_TEST_CASE(seconds)
{
    // Period of 60 seconds modelled as 6 * (10 seconds)
    std::size_t const nr_ticks = 6;

    using TickPeriod = ldm::time::TickPeriod<ldm::time::Second>;
    using Duration = ldm::time::Duration<TickPeriod>;

    Duration duration(nr_ticks);

    BOOST_CHECK_EQUAL(duration.nr_ticks(), nr_ticks);
}
