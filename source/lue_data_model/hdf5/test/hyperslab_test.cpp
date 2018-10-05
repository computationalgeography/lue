#define BOOST_TEST_MODULE lue hdf5 hyperslab
#include <boost/test/unit_test.hpp>
#include "lue/hdf5/hyperslab.hpp"


using namespace lue::hdf5;


BOOST_AUTO_TEST_CASE(hyper_slab_1)
{
    Shape const shape{};
    Hyperslab const hyperslab{shape};

    BOOST_CHECK(hyperslab.empty());
    BOOST_CHECK_EQUAL(hyperslab.nr_elements(), 0);
}


BOOST_AUTO_TEST_CASE(hyper_slab_2)
{
    Shape const shape{4};
    Hyperslab const hyperslab{shape};

    BOOST_CHECK(!hyperslab.empty());
    BOOST_CHECK_EQUAL(hyperslab.nr_elements(), 4);
}


BOOST_AUTO_TEST_CASE(shape_size_3)
{
    Shape const shape{3, 2};
    Hyperslab const hyperslab{shape};

    BOOST_CHECK(!hyperslab.empty());
    BOOST_CHECK_EQUAL(hyperslab.nr_elements(), 6);
}
