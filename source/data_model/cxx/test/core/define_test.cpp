#define BOOST_TEST_MODULE lue core define
#include "lue/core/define.hpp"
#include <boost/test/included/unit_test.hpp>


BOOST_AUTO_TEST_CASE(define)
{
    bool const test_implemented{false};

    BOOST_WARN(test_implemented);
}
