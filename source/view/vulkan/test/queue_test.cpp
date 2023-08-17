#define BOOST_TEST_MODULE lue vulkan queue
#include "lue/vulkan/queue.hpp"
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(default_construct_queue)
{
    {
        lue::vulkan::Queue queue;

        BOOST_CHECK(!queue.is_valid());
    }

    {
        lue::vulkan::Queue queue{};

        BOOST_CHECK(!queue.is_valid());
    }
}
