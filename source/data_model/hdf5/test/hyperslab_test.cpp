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
