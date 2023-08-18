#define BOOST_TEST_MODULE lue vulkan descriptor_set_layout
#include "lue/vulkan/descriptor_set_layout.hpp"
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(default_construct)
{
    {
        lue::vulkan::DescriptorSetLayout descriptor_set_layout;

        BOOST_CHECK(!descriptor_set_layout.is_valid());
    }

    {
        lue::vulkan::DescriptorSetLayout descriptor_set_layout{};

        BOOST_CHECK(!descriptor_set_layout.is_valid());
    }
}
