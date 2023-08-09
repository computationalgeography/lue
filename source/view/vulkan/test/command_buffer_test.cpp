#define BOOST_TEST_MODULE lue vulkan command_buffer
#include "lue/vulkan/command_buffer.hpp"
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(default_construct)
{
    {
        lue::vulkan::CommandBuffer command_buffer;

        BOOST_CHECK(!command_buffer);
    }

    {
        lue::vulkan::CommandBuffer command_buffer{};

        BOOST_CHECK(!command_buffer);
    }
}
