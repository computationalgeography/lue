#define BOOST_TEST_MODULE lue qa scalability instrument run
#include "lue/qa/scalability/instrument/run.hpp"
#include <boost/test/included/unit_test.hpp>


BOOST_AUTO_TEST_CASE(use_case_01)
{
    lue::qa::Run run{};
    run.start();
    run.stop();

    using namespace std::chrono_literals;

    BOOST_TEST(run.duration<std::chrono::nanoseconds>() > std::chrono::nanoseconds(0ns));
    BOOST_TEST(run.duration<std::chrono::seconds>() == std::chrono::seconds(0s));
}
