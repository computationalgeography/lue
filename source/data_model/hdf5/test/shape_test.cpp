#define BOOST_TEST_MODULE lue hdf5 shape
#include "lue/hdf5/shape.hpp"
#include <boost/test/included/unit_test.hpp>


namespace lh5 = lue::hdf5;


BOOST_AUTO_TEST_CASE(shape_size_1)
{
    // Empty shape
    lh5::Shape const shape{};
    auto const size = lh5::size_of_shape(shape, 4);
    BOOST_CHECK_EQUAL(size, 4);
}


BOOST_AUTO_TEST_CASE(shape_size_2)
{
    lh5::Shape const shape{2};
    auto const size = lh5::size_of_shape(shape, 4);
    BOOST_CHECK_EQUAL(size, 8);
}


BOOST_AUTO_TEST_CASE(shape_size_3)
{
    lh5::Shape const shape{3, 2};
    auto const size = lh5::size_of_shape(shape, 4);
    BOOST_CHECK_EQUAL(size, 24);
}
