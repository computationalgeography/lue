#define BOOST_TEST_MODULE lue vulkan shader_module
#include "lue/vulkan/shader_module.hpp"
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(default_construct)
{
    {
        lue::vulkan::ShaderModule shader_module;

        BOOST_CHECK(!shader_module.is_valid());
    }

    {
        lue::vulkan::ShaderModule shader_module{};

        BOOST_CHECK(!shader_module.is_valid());
    }
}
