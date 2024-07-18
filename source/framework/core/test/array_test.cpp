#define BOOST_TEST_MODULE lue framework core array_test
#include "lue/framework/core/array.hpp"
#include "lue/framework/test/stream.hpp"
#include <hpx/config.hpp>
#include <boost/test/included/unit_test.hpp>


BOOST_AUTO_TEST_CASE(rank)
{
    using Element = bool;

    // 1d
    {
        lue::Rank const rank = 1;
        using Array = lue::Array<Element, rank>;
        BOOST_CHECK_EQUAL(lue::rank<Array>, rank);
    }

    // 2d
    {
        lue::Rank const rank = 2;
        using Array = lue::Array<Element, rank>;
        BOOST_CHECK_EQUAL(lue::rank<Array>, rank);
    }
}


BOOST_AUTO_TEST_CASE(array_bool_1d)
{
    using Element = bool;
    lue::Rank const rank = 1;
    using Array = lue::Array<Element, rank>;
    using Shape = lue::ShapeT<Array>;

    // ()
    {
        Shape const shape{};
        Array array{shape};

        BOOST_CHECK_EQUAL(lue::shape(array), shape);
        BOOST_CHECK_EQUAL(lue::nr_elements(array), 0u);
    }

    // (3)
    {
        Shape const shape{3};
        Array array{shape, true};

        BOOST_CHECK_EQUAL(lue::shape(array), shape);
        BOOST_CHECK_EQUAL(lue::nr_elements(array), 3u);

        BOOST_CHECK_EQUAL(array[0], true);
        BOOST_CHECK_EQUAL(array[1], true);
        BOOST_CHECK_EQUAL(array[2], true);

        BOOST_CHECK_EQUAL(array(0), true);
        BOOST_CHECK_EQUAL(array(1), true);
        BOOST_CHECK_EQUAL(array(2), true);
    }
}


BOOST_AUTO_TEST_CASE(array_bool_2d)
{
    using Element = bool;
    lue::Rank const rank = 2;
    using Array = lue::Array<Element, rank>;
    using Shape = lue::ShapeT<Array>;

    // ()
    {
        Shape const shape{};
        Array array{shape};

        BOOST_CHECK_EQUAL(lue::shape(array), shape);
        BOOST_CHECK_EQUAL(lue::nr_elements(array), 0u);
    }

    // (3, 3)
    {
        Shape const shape{3, 3};
        Array array{shape};

        BOOST_CHECK_EQUAL(lue::shape(array), shape);
        BOOST_CHECK_EQUAL(lue::nr_elements(array), 9u);

        std::fill(array.begin(), array.end(), false);
        std::int32_t sum = std::accumulate(array.begin(), array.end(), 0);
        BOOST_CHECK_EQUAL(sum, 0);

        std::fill(array.begin(), array.end(), true);
        sum = std::accumulate(array.begin(), array.end(), 0);
        BOOST_CHECK_EQUAL(sum, 9);

        BOOST_CHECK_EQUAL(array[0], true);
        BOOST_CHECK_EQUAL(array[1], true);
        BOOST_CHECK_EQUAL(array[2], true);
        BOOST_CHECK_EQUAL(array[3], true);
        BOOST_CHECK_EQUAL(array[4], true);
        BOOST_CHECK_EQUAL(array[5], true);
        BOOST_CHECK_EQUAL(array[6], true);
        BOOST_CHECK_EQUAL(array[7], true);
        BOOST_CHECK_EQUAL(array[8], true);

        BOOST_CHECK_EQUAL(array(0, 0), true);
        BOOST_CHECK_EQUAL(array(0, 1), true);
        BOOST_CHECK_EQUAL(array(0, 2), true);
        BOOST_CHECK_EQUAL(array(1, 0), true);
        BOOST_CHECK_EQUAL(array(1, 1), true);
        BOOST_CHECK_EQUAL(array(1, 2), true);
        BOOST_CHECK_EQUAL(array(2, 0), true);
        BOOST_CHECK_EQUAL(array(2, 1), true);
        BOOST_CHECK_EQUAL(array(2, 2), true);
    }
}
