#define BOOST_TEST_MODULE lue translate time to_string
#include <boost/test/unit_test.hpp>
#include "lue/utility/lue_translate/time/to_string.h"


using namespace lue;


BOOST_AUTO_TEST_CASE(time_point_to_iso_8601_string_)
{
    {
        MonthTimePoint time_point(2016, 5);
        std::string string;

        string = time_point_to_iso_8601_string(time_point);
        BOOST_CHECK_EQUAL(string, "2016-05");
    }
}


BOOST_AUTO_TEST_CASE(time_duration_to_iso_8601_string_)
{
    {
        MonthDuration time_duration(1);
        std::string string;

        string = time_duration_to_iso_8601_string(time_duration);
        BOOST_CHECK_EQUAL(string, "P1M");
    }
}
