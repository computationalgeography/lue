#define BOOST_TEST_MODULE lue vulkan command_pool
#include "lue/vulkan/command_pool.hpp"
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(default_construct)
{
    {
        lue::vulkan::CommandPool command_pool;

        BOOST_CHECK(!command_pool);
    }

    {
        lue::vulkan::CommandPool command_pool{};

        BOOST_CHECK(!command_pool);
    }
}
