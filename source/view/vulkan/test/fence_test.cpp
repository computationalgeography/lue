#define BOOST_TEST_MODULE lue vulkan fence
#include "lue/vulkan/fence.hpp"
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(default_construct)
{
    {
        lue::vulkan::Fence fence;

        BOOST_CHECK(!fence);
    }

    {
        lue::vulkan::Fence fence{};

        BOOST_CHECK(!fence);
    }
}
