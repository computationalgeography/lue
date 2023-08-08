#define BOOST_TEST_MODULE lue vulkan instance
#include "lue/vulkan/instance.hpp"
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(default_construct)
{
    {
        lue::vulkan::Instance instance;

        BOOST_CHECK(!instance);
    }

    {
        lue::vulkan::Instance instance{};

        BOOST_CHECK(!instance);
    }
}
