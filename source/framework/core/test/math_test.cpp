#define BOOST_TEST_MODULE lue framework core math
#include <boost/test/unit_test.hpp>
#include "lue/framework/core/math.hpp"


BOOST_AUTO_TEST_CASE(map_indices)
{
    using Input = std::size_t;
    using Output = std::size_t;

    // Equal input and output ranges
    {
        Input from_min = 0;
        Input from_max = 10;
        Output to_min = 0;
        Output to_max = 10;

        BOOST_CHECK_EQUAL(
            lue::map_to_range(from_min, from_max, to_min, to_max, Input{0}),
            0);
        BOOST_CHECK_EQUAL(
            lue::map_to_range(from_min, from_max, to_min, to_max, Input{5}),
            5);
        BOOST_CHECK_EQUAL(
            lue::map_to_range(from_min, from_max, to_min, to_max, Input{10}),
            10);
    }

    // Smaller input range than output range
    {
        Input from_min = 0;
        Input from_max = 4;
        Output to_min = 0;
        Output to_max = 10;

        BOOST_CHECK_EQUAL(
            lue::map_to_range(from_min, from_max, to_min, to_max, Input{0}),
            0);
        BOOST_CHECK_EQUAL(
            lue::map_to_range(from_min, from_max, to_min, to_max, Input{2}),
            5);
        BOOST_CHECK_EQUAL(
            lue::map_to_range(from_min, from_max, to_min, to_max, Input{4}),
            10);
    }

    // Larger input range than output range
    {
        Input from_min = 0;
        Input from_max = 10;
        Output to_min = 0;
        Output to_max = 4;

        BOOST_CHECK_EQUAL(
            lue::map_to_range(from_min, from_max, to_min, to_max, Input{0}),
            0);
        BOOST_CHECK_EQUAL(
            lue::map_to_range(from_min, from_max, to_min, to_max, Input{5}),
            2);
        BOOST_CHECK_EQUAL(
            lue::map_to_range(from_min, from_max, to_min, to_max, Input{10}),
            4);
    }

    // Input range with single element
    {
        Input from_min = 0;
        Input from_max = 0;
        Output to_min = 0;
        Output to_max = 4;

        BOOST_CHECK_EQUAL(
            lue::map_to_range(from_min, from_max, to_min, to_max, Input{0}),
            0);
    }

    // Output range with single element
    {
        Input from_min = 0;
        Input from_max = 10;
        Output to_min = 0;
        Output to_max = 0;

        BOOST_CHECK_EQUAL(
            lue::map_to_range(from_min, from_max, to_min, to_max, Input{0}),
            0);
        BOOST_CHECK_EQUAL(
            lue::map_to_range(from_min, from_max, to_min, to_max, Input{5}),
            0);
        BOOST_CHECK_EQUAL(
            lue::map_to_range(from_min, from_max, to_min, to_max, Input{10}),
            0);
    }
}
