#define BOOST_TEST_MODULE lue translate time from_string
#include <boost/test/unit_test.hpp>
#include "lue/utility/lue_translate/time/from_string.h"


using namespace lue;


BOOST_AUTO_TEST_CASE(parse_months_)
{
    {
        bool status;
        MonthTimePoint time_point;
        MonthDuration duration;
        std::tie(status, time_point, duration) = parse_months("2016-05",
            "P1M");

        BOOST_REQUIRE(status);
        BOOST_CHECK_EQUAL(time_point.year(), 2016);
        BOOST_CHECK_EQUAL(time_point.month(), 5);
        BOOST_CHECK_EQUAL(duration.count(), 1);
    }
}
