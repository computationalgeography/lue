#define BOOST_TEST_MODULE lue shared no_data
#include <boost/test/unit_test.hpp>
#include "lue/no_data.hpp"


BOOST_AUTO_TEST_CASE(integral)
{
    std::int32_t value{5};

    BOOST_CHECK(!lue::is_no_data(value));

    lue::set_no_data(value);

    BOOST_CHECK(lue::is_no_data(value));
}


BOOST_AUTO_TEST_CASE(floating_point)
{
    double value{5.5};

    BOOST_CHECK(!lue::is_no_data(value));

    lue::set_no_data(value);

    BOOST_CHECK(lue::is_no_data(value));
}
