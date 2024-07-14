#define BOOST_TEST_MODULE lue gdal version_test
#include "lue/gdal/version.hpp"
#include <boost/test/included/unit_test.hpp>


BOOST_AUTO_TEST_CASE(version)
{
    std::string const version{lue::gdal::version()};

    BOOST_CHECK(!version.empty());
}
