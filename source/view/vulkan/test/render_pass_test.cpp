#define BOOST_TEST_MODULE lue vulkan render_pass
#include "lue/vulkan/render_pass.hpp"
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(default_construct)
{
    {
        lue::vulkan::RenderPass render_pass;

        BOOST_CHECK(!render_pass.is_valid());
    }

    {
        lue::vulkan::RenderPass render_pass{};

        BOOST_CHECK(!render_pass.is_valid());
    }
}
