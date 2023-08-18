#define BOOST_TEST_MODULE lue vulkan descriptor_pool
#include "lue/vulkan/descriptor_pool.hpp"
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(default_construct)
{
    {
        lue::vulkan::DescriptorPool descriptor_pool;

        BOOST_CHECK(!descriptor_pool.is_valid());
    }

    {
        lue::vulkan::DescriptorPool descriptor_pool{};

        BOOST_CHECK(!descriptor_pool.is_valid());
    }
}
