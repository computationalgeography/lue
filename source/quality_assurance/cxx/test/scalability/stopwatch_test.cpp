#define BOOST_TEST_MODULE lue qa scalability instrument stopwatch
#include "lue/qa/scalability/instrument/stopwatch.hpp"
#include <boost/test/included/unit_test.hpp>


BOOST_AUTO_TEST_CASE(use_case_01)
{
    lue::qa::Stopwatch stopwatch{};
    stopwatch.start();
    stopwatch.stop();

    using namespace std::chrono_literals;

    // Casting to bool because some compilers still don't support streaming chrono durations...
    BOOST_TEST(
        static_cast<bool>(stopwatch.duration<std::chrono::nanoseconds>() > std::chrono::nanoseconds(0ns)));
    BOOST_TEST(static_cast<bool>(stopwatch.duration<std::chrono::seconds>() == std::chrono::seconds(0s)));
}
