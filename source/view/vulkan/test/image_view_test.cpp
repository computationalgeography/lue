#define BOOST_TEST_MODULE lue vulkan image_view
#include "lue/vulkan/image_view.hpp"
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(default_construct)
{
    {
        lue::vulkan::ImageView image_view;

        BOOST_CHECK(!image_view.is_valid());
    }

    {
        lue::vulkan::ImageView image_view{};

        BOOST_CHECK(!image_view.is_valid());
    }
}
