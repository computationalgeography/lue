#define BOOST_TEST_MODULE lue framework partitioned_route partitioned_serial_route
#include "lue/framework/partitioned_route/serial_route.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


BOOST_AUTO_TEST_CASE(default_construct)
{
    // Default initialization
    {
        lue::SerialRoute<2> route;

        BOOST_CHECK_EQUAL(route.nr_routes(), 0);
    }

    // Value initialization
    {
        lue::SerialRoute<2> route{};

        BOOST_CHECK_EQUAL(route.nr_routes(), 0);
    }
}


BOOST_AUTO_TEST_CASE(construct_from_cells)
{
    lue::SerialRoute<2> route{};
}
