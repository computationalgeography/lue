#define BOOST_TEST_MODULE lue vulkan buffer
#include "lue/vulkan/buffer.hpp"
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(default_construct)
{
    {
        lue::vulkan::Buffer buffer;

        BOOST_CHECK(!buffer.is_valid());
    }

    {
        lue::vulkan::Buffer buffer{};

        BOOST_CHECK(!buffer.is_valid());
    }
}
