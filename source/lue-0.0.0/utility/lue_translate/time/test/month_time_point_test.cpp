#define BOOST_TEST_MODULE lue translate time month_time_point
#include <boost/test/unit_test.hpp>
#include "lue/utility/lue_translate/time/month_time_point.h"


using namespace lue;


BOOST_AUTO_TEST_CASE(constructor)
{
    {
        MonthTimePoint time_point;
        BOOST_CHECK_EQUAL(time_point.year(), 0);
        BOOST_CHECK_EQUAL(time_point.month(), 0);
    }

    {
        MonthTimePoint time_point(2016, 5);
        BOOST_CHECK_EQUAL(time_point.year(), 2016);
        BOOST_CHECK_EQUAL(time_point.month(), 5);
    }

}
