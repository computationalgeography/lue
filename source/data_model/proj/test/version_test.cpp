#define BOOST_TEST_MODULE lue proj version_test
#include "lue/proj/version.hpp"
#include <boost/test/included/unit_test.hpp>


BOOST_AUTO_TEST_CASE(version)
{
    std::string const version{lue::proj::version()};

    BOOST_CHECK(!version.empty());
}
