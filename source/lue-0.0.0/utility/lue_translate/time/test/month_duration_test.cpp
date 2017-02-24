#define BOOST_TEST_MODULE lue translate time month_duration
#include <boost/test/unit_test.hpp>
#include "lue/utility/lue_translate/time/month_duration.h"


using namespace lue;


BOOST_AUTO_TEST_CASE(constructor)
{
    {
        MonthDuration duration;
        BOOST_CHECK_EQUAL(duration.count(), 0);
    }

    {
        MonthDuration duration(5);
        BOOST_CHECK_EQUAL(duration.count(), 5);
    }
}
