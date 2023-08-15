#define BOOST_TEST_MODULE lue vulkan semaphore
#include "lue/vulkan/semaphore.hpp"
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(default_construct)
{
    {
        lue::vulkan::Semaphore semaphore;

        BOOST_CHECK(!semaphore.is_valid());
    }

    {
        lue::vulkan::Semaphore semaphore{};

        BOOST_CHECK(!semaphore.is_valid());
    }
}
