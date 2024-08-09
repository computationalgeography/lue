#define BOOST_TEST_MODULE lue framework core bounding_box
#include "lue/framework/core/bounding_box.hpp"
#include <boost/test/included/unit_test.hpp>


BOOST_AUTO_TEST_CASE(default_construct)
{
    using Element = std::int32_t;

    // The default constructor of a pair value-initializes the members. In case of integers this should result
    // in the value zero.

    {
        lue::BoundingBox<Element, 1> box;

        BOOST_CHECK_EQUAL(box.min_limit(0), 0);
        BOOST_CHECK_EQUAL(box.max_limit(0), 0);
        BOOST_CHECK_EQUAL(box.extent(0), 0);
    }

    {
        lue::BoundingBox<Element, 2> box;

        BOOST_CHECK_EQUAL(box.min_limit(0), 0);
        BOOST_CHECK_EQUAL(box.max_limit(0), 0);
        BOOST_CHECK_EQUAL(box.extent(0), 0);
        BOOST_CHECK_EQUAL(box.min_limit(1), 0);
        BOOST_CHECK_EQUAL(box.max_limit(1), 0);
        BOOST_CHECK_EQUAL(box.extent(1), 0);
    }
}


BOOST_AUTO_TEST_CASE(constructor)
{
    using Element = std::int32_t;

    {
        lue::BoundingBox<Element, 1> box{{{{5, 7}}}};

        BOOST_CHECK_EQUAL(box.min_limit(0), 5);
        BOOST_CHECK_EQUAL(box.max_limit(0), 7);
        BOOST_CHECK_EQUAL(box.extent(0), 2);
    }

    {
        lue::BoundingBox<Element, 2> box{{{
            {5, 7},
            {4, 8},
        }}};

        BOOST_CHECK_EQUAL(box.min_limit(0), 5);
        BOOST_CHECK_EQUAL(box.max_limit(0), 7);
        BOOST_CHECK_EQUAL(box.extent(0), 2);
        BOOST_CHECK_EQUAL(box.min_limit(1), 4);
        BOOST_CHECK_EQUAL(box.max_limit(1), 8);
        BOOST_CHECK_EQUAL(box.extent(1), 4);
    }
}
