#define BOOST_TEST_MODULE lue translate stack_name
#include <boost/test/unit_test.hpp>
#include "lue/translate/stack_name.hpp"


BOOST_AUTO_TEST_CASE(use_case_1)
{
    namespace lu = lue::utility;

    std::string const raster_name{"my_raster.tif"};
    lu::StackName const stack_name{raster_name};

    BOOST_CHECK_EQUAL(stack_name[0], "my_raster_0.tif");
    BOOST_CHECK_EQUAL(stack_name[100], "my_raster_100.tif");
}


BOOST_AUTO_TEST_CASE(use_case_2)
{
    namespace lu = lue::utility;

    std::string const raster_name{"/blah/my_raster"};
    lu::StackName const stack_name{raster_name};

    BOOST_CHECK_EQUAL(stack_name[0], "/blah/my_raster_0");
    BOOST_CHECK_EQUAL(stack_name[100], "/blah/my_raster_100");
}


BOOST_AUTO_TEST_CASE(use_case_3)
{
    namespace lu = lue::utility;

    std::string const raster_name{""};
    lu::StackName const stack_name{raster_name};

    BOOST_CHECK_EQUAL(stack_name[0], "_0");
    BOOST_CHECK_EQUAL(stack_name[100], "_100");
}
