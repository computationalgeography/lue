#define BOOST_TEST_MODULE lue vulkan surface
#include "lue/vulkan/surface.hpp"
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(default_construct)
{
    {
        lue::vulkan::Surface surface;

        BOOST_CHECK(!surface.is_valid());
    }

    {
        lue::vulkan::Surface surface{};

        BOOST_CHECK(!surface.is_valid());
    }
}
