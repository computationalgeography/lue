#define BOOST_TEST_MODULE lue vulkan pipeline_layout
#include "lue/vulkan/framebuffer.hpp"
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(default_construct)
{
    {
        lue::vulkan::Framebuffer framebuffer;

        BOOST_CHECK(!framebuffer.is_valid());
    }

    {
        lue::vulkan::Framebuffer framebuffer{};

        BOOST_CHECK(!framebuffer.is_valid());
    }
}
