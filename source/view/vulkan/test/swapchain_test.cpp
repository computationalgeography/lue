#define BOOST_TEST_MODULE lue vulkan swapchain
#include "lue/vulkan/swapchain.hpp"
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(default_construct)
{
    {
        lue::vulkan::Swapchain swapchain;

        BOOST_CHECK(!swapchain.is_valid());
    }

    {
        lue::vulkan::Swapchain swapchain{};

        BOOST_CHECK(!swapchain.is_valid());
    }
}
