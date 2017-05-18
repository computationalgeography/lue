#define BOOST_TEST_MODULE lue hdf5 shape
#include <boost/test/unit_test.hpp>
#include "lue/hdf5/shape.h"


using namespace lue::hdf5;


BOOST_AUTO_TEST_CASE(shape_size_1)
{
    // Empty shape
    Shape const shape;
    auto const size = size_of_shape(shape, 4);
    BOOST_CHECK_EQUAL(size, 4);
}


BOOST_AUTO_TEST_CASE(shape_size_2)
{
    Shape const shape{2};
    auto const size = size_of_shape(shape, 4);
    BOOST_CHECK_EQUAL(size, 8);
}


BOOST_AUTO_TEST_CASE(shape_size_3)
{
    Shape const shape{3, 2};
    auto const size = size_of_shape(shape, 4);
    BOOST_CHECK_EQUAL(size, 24);
}
