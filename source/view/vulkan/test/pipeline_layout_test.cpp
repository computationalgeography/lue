#define BOOST_TEST_MODULE lue vulkan pipeline_layout
#include "lue/vulkan/pipeline_layout.hpp"
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(default_construct)
{
    {
        lue::vulkan::PipelineLayout pipeline_layout;

        BOOST_CHECK(!pipeline_layout.is_valid());
    }

    {
        lue::vulkan::PipelineLayout pipeline_layout{};

        BOOST_CHECK(!pipeline_layout.is_valid());
    }
}
