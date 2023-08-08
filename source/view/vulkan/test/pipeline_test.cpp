#define BOOST_TEST_MODULE lue vulkan pipeline
#include "lue/vulkan/pipeline.hpp"
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(default_construct)
{
    {
        lue::vulkan::Pipeline pipeline;

        BOOST_CHECK(!pipeline);
    }

    {
        lue::vulkan::Pipeline pipeline{};

        BOOST_CHECK(!pipeline);
    }
}
