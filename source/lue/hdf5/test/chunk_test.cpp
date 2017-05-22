#define BOOST_TEST_MODULE lue hdf5 chunk
#include <boost/test/unit_test.hpp>
#include "lue/hdf5/chunk.h"


using namespace lue::hdf5;


Shape test_chunk(
    Shape const& value_shape)
{
    using T = std::int32_t;

    auto const shape = chunk_shape<T>(value_shape);

    BOOST_CHECK_EQUAL(shape.size(), value_shape.size() + 1);

    auto const size = size_of_chunk<T>(shape);

    BOOST_CHECK_GE(size, lower_chunk_size_limit());
    BOOST_CHECK_LE(size, upper_chunk_size_limit());

    return shape;
}


BOOST_AUTO_TEST_CASE(chunk_shape_small_1)
{
    // 0D values
    Shape const value_shape{};
    test_chunk(value_shape);
}


BOOST_AUTO_TEST_CASE(chunk_shape_small_2)
{
    // 1D values
    Shape const value_shape({10});
    test_chunk(value_shape);
}


BOOST_AUTO_TEST_CASE(chunk_shape_small_3)
{
    // 2D values
    Shape const value_shape({2, 3});
    test_chunk(value_shape);
}


BOOST_AUTO_TEST_CASE(chunk_shape_large_1)
{
    // 1D values
    Shape const value_shape({2000000});
    test_chunk(value_shape);
}


BOOST_AUTO_TEST_CASE(chunk_shape_large_2)
{
    // 2D values
    {
        Shape const value_shape({2000000, 2000000});
        auto const chunk_shape = test_chunk(value_shape);
    }

    {
        Shape const value_shape({1, 2000000});
        auto const chunk_shape = test_chunk(value_shape);

        BOOST_CHECK_EQUAL(chunk_shape[0], 1.0);
        BOOST_CHECK_EQUAL(chunk_shape[1], 1.0);
    }

    {
        Shape const value_shape({2000000, 1});
        auto const chunk_shape = test_chunk(value_shape);

        BOOST_CHECK_EQUAL(chunk_shape[0], 1.0);
        BOOST_CHECK_EQUAL(chunk_shape[2], 1.0);
    }
}
