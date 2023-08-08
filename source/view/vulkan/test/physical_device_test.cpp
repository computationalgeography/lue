#define BOOST_TEST_MODULE lue vulkan physical_device
#include "lue/vulkan/physical_device.hpp"
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(default_construct)
{
    {
        lue::vulkan::PhysicalDevice physical_device;

        BOOST_CHECK(!physical_device);
    }

    {
        lue::vulkan::PhysicalDevice physical_device{};

        BOOST_CHECK(!physical_device);
    }
}
