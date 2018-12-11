#define BOOST_TEST_MODULE lue framework core shape
#include <boost/test/unit_test.hpp>
#include "lue/framework/core/shape.hpp"


BOOST_AUTO_TEST_CASE(construct)
{
    using T = std::uint32_t;
    std::size_t const rank = 2;
    using Shape = lue::Shape<T, rank>;

    {
        Shape shape{};
        BOOST_CHECK_EQUAL(shape.size(), 2);
        BOOST_CHECK_EQUAL(shape[0], T{0});
        BOOST_CHECK_EQUAL(shape[1], T{0});
    }

    {
        Shape shape{30, 40};
        BOOST_CHECK_EQUAL(shape.size(), 2);
        BOOST_CHECK_EQUAL(shape[0], T{30});
        BOOST_CHECK_EQUAL(shape[1], T{40});
    }
}

