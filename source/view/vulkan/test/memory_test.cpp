#define BOOST_TEST_MODULE lue vulkan memory
#include "lue/vulkan/memory.hpp"
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(default_construct)
{
    {
        lue::vulkan::Memory memory;

        BOOST_CHECK(!memory.is_valid());
    }

    {
        lue::vulkan::Memory memory{};

        BOOST_CHECK(!memory.is_valid());
    }
}
