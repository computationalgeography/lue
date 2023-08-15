#define BOOST_TEST_MODULE lue vulkan device
#include "lue/vulkan/device.hpp"
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(default_construct_queue)
{
    {
        lue::vulkan::Device::Queue queue;

        BOOST_CHECK(!queue.is_valid());
    }

    {
        lue::vulkan::Device::Queue queue{};

        BOOST_CHECK(!queue.is_valid());
    }
}


BOOST_AUTO_TEST_CASE(default_construct_device)
{
    {
        lue::vulkan::Device device;

        BOOST_CHECK(!device.is_valid());
    }

    {
        lue::vulkan::Device device{};

        BOOST_CHECK(!device.is_valid());
    }
}
