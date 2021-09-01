#define BOOST_TEST_MODULE lue hdf5 hyperslab
#include <boost/test/unit_test.hpp>
#include "lue/hdf5/hyperslab.hpp"


namespace lh5 = lue::hdf5;


BOOST_AUTO_TEST_CASE(default_construct)
{
    lh5::Hyperslab const hyperslab{};

    BOOST_CHECK(hyperslab.empty());
    BOOST_CHECK(hyperslab.start().empty());
    BOOST_CHECK(hyperslab.stride().empty());
    BOOST_CHECK(hyperslab.count().empty());
    BOOST_CHECK_EQUAL(hyperslab.nr_dimensions(), 0);
    BOOST_CHECK_EQUAL(hyperslab.nr_elements(), 0);
}


BOOST_AUTO_TEST_CASE(hyper_slab_1)
{
    lh5::Shape const shape{};
    lh5::Hyperslab const hyperslab{shape};

    BOOST_CHECK(hyperslab.empty());
    BOOST_CHECK_EQUAL(hyperslab.nr_elements(), 0);
}


BOOST_AUTO_TEST_CASE(hyper_slab_2)
{
    lh5::Shape const shape{4};
    lh5::Hyperslab const hyperslab{shape};

    BOOST_CHECK(!hyperslab.empty());
    BOOST_CHECK_EQUAL(hyperslab.nr_elements(), 4);
}


BOOST_AUTO_TEST_CASE(shape_size_3)
{
    lh5::Shape const shape{3, 2};
    lh5::Hyperslab const hyperslab{shape};

    BOOST_CHECK(!hyperslab.empty());
    BOOST_CHECK_EQUAL(hyperslab.nr_elements(), 6);
}


BOOST_AUTO_TEST_CASE(hyperslab)
{
    {
        lh5::Offset const& center{1, 1};
        lh5::Shape const& shape{3, 3};
        lh5::Shape const& array_shape{3, 3};

        lh5::Hyperslab const hyperslab{lh5::hyperslab(center, shape, array_shape)};

        BOOST_CHECK_EQUAL(hyperslab.nr_dimensions(), 2);
        BOOST_CHECK_EQUAL(hyperslab.nr_elements(), 9);
        BOOST_CHECK_EQUAL(hyperslab.start()[0], 0);
        BOOST_CHECK_EQUAL(hyperslab.start()[1], 0);
        BOOST_CHECK_EQUAL(hyperslab.stride()[0], 1);
        BOOST_CHECK_EQUAL(hyperslab.stride()[1], 1);
        BOOST_CHECK_EQUAL(hyperslab.count()[0], 3);
        BOOST_CHECK_EQUAL(hyperslab.count()[1], 3);
    }

    {
        lh5::Offset const& center{1, 1};
        lh5::Shape const& shape{1, 1};
        lh5::Shape const& array_shape{3, 3};

        lh5::Hyperslab const hyperslab{lh5::hyperslab(center, shape, array_shape)};

        BOOST_CHECK_EQUAL(hyperslab.nr_dimensions(), 2);
        BOOST_CHECK_EQUAL(hyperslab.nr_elements(), 1);
        BOOST_CHECK_EQUAL(hyperslab.start()[0], 1);
        BOOST_CHECK_EQUAL(hyperslab.start()[1], 1);
        BOOST_CHECK_EQUAL(hyperslab.stride()[0], 1);
        BOOST_CHECK_EQUAL(hyperslab.stride()[1], 1);
        BOOST_CHECK_EQUAL(hyperslab.count()[0], 1);
        BOOST_CHECK_EQUAL(hyperslab.count()[1], 1);
    }

    {
        lh5::Offset const& center{1, 1};
        lh5::Shape const& shape{2, 2};
        lh5::Shape const& array_shape{3, 3};

        lh5::Hyperslab const hyperslab{lh5::hyperslab(center, shape, array_shape)};

        BOOST_CHECK_EQUAL(hyperslab.nr_dimensions(), 2);
        BOOST_CHECK_EQUAL(hyperslab.nr_elements(), 4);
        BOOST_CHECK_EQUAL(hyperslab.start()[0], 0);
        BOOST_CHECK_EQUAL(hyperslab.start()[1], 0);
        BOOST_CHECK_EQUAL(hyperslab.stride()[0], 1);
        BOOST_CHECK_EQUAL(hyperslab.stride()[1], 1);
        BOOST_CHECK_EQUAL(hyperslab.count()[0], 2);
        BOOST_CHECK_EQUAL(hyperslab.count()[1], 2);
    }
}
