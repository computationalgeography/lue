#define BOOST_TEST_MODULE lue vulkan debug_report_callback
#include "lue/vulkan/debug_report_callback.hpp"
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(default_construct)
{
    {
        lue::vulkan::DebugReportCallback callback;

        BOOST_CHECK(!callback);
    }

    {
        lue::vulkan::DebugReportCallback callback{};

        BOOST_CHECK(!callback);
    }
}
