#define BOOST_TEST_MODULE lue framework core grain
#include <boost/test/unit_test.hpp>
#include "lue/framework/core/grain.hpp"


BOOST_AUTO_TEST_CASE(construct)
{
    using T = std::uint32_t;
    std::size_t const rank = 2;
    using Grain = lue::Grain<T, rank>;
    using Start = Grain::Start;
    using Shape = Grain::Shape;

    {
        Grain grain{};

        BOOST_CHECK_EQUAL(grain.start()[0], T{0});
        BOOST_CHECK_EQUAL(grain.start()[1], T{0});
        BOOST_CHECK_EQUAL(grain.shape()[0], T{0});
        BOOST_CHECK_EQUAL(grain.shape()[1], T{0});
    }

    {
        Grain grain{Shape{30, 40}};

        BOOST_CHECK_EQUAL(grain.start()[0], T{0});
        BOOST_CHECK_EQUAL(grain.start()[1], T{0});
        BOOST_CHECK_EQUAL(grain.shape()[0], T{30});
        BOOST_CHECK_EQUAL(grain.shape()[1], T{40});
    }

    {
        Grain grain{Start{4, 5}, Shape{30, 40}};

        BOOST_CHECK_EQUAL(grain.start()[0], T{4});
        BOOST_CHECK_EQUAL(grain.start()[1], T{5});
        BOOST_CHECK_EQUAL(grain.shape()[0], T{30});
        BOOST_CHECK_EQUAL(grain.shape()[1], T{40});
    }
}
