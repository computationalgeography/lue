#define BOOST_TEST_MODULE lue framework core shape
#include <boost/test/unit_test.hpp>
#include "lue/framework/core/shape.hpp"


namespace {

using Index = std::uint32_t;

template<
    lue::Rank rank>
using Shape = lue::Shape<Index, rank>;

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(construct_0d)
{
    {
        Shape<0> shape{};
        BOOST_CHECK_EQUAL(shape.size(), 0);
    }
}


BOOST_AUTO_TEST_CASE(construct_2d)
{
    {
        Shape<2> shape{};
        BOOST_CHECK_EQUAL(shape.size(), 2);
        BOOST_CHECK_EQUAL(shape[0], Index{0});
        BOOST_CHECK_EQUAL(shape[1], Index{0});
    }

    {
        Shape<2> shape{30, 40};
        BOOST_CHECK_EQUAL(shape.size(), 2);
        BOOST_CHECK_EQUAL(shape[0], Index{30});
        BOOST_CHECK_EQUAL(shape[1], Index{40});
    }
}


BOOST_AUTO_TEST_CASE(nr_elements)
{
    {
        Shape<0> shape{};
        BOOST_CHECK_EQUAL(lue::nr_elements(shape), 0);
    }

    {
        Shape<2> shape{};
        BOOST_CHECK_EQUAL(lue::nr_elements(shape), 0);
    }

    {
        Shape<2> shape{30, 40};
        BOOST_CHECK_EQUAL(lue::nr_elements(shape), 1200);
    }

    {
        Shape<2> shape{57926, 57926};
        BOOST_CHECK_EQUAL(lue::nr_elements(shape), 3355421476);
    }
}
