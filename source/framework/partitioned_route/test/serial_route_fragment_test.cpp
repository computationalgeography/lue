#define BOOST_TEST_MODULE lue framework partitioned_route serial_route_fragment
#include "lue/framework/partitioned_route/serial_route_fragment.hpp"
#include "lue/stream.hpp"
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(default_construct)
{
    // Default initialization
    {
        lue::SerialRouteFragment<2> fragment;

        BOOST_CHECK_EQUAL(fragment.nr_cells(), 0);
    }

    // Value initialization
    {
        lue::SerialRouteFragment<2> fragment{};

        BOOST_CHECK_EQUAL(fragment.nr_cells(), 0);
    }
}


BOOST_AUTO_TEST_CASE(use_case01)
{
    lue::SerialRouteFragment<2>::CellsIdxs const cells_idxs{{0, 1}, {2, 3}, {4, 5}};

    // Create fragment as we go
    lue::SerialRouteFragment<2> fragment;
    fragment.append(cells_idxs[0]);
    fragment.append(cells_idxs[1]);
    fragment.append(cells_idxs[2]);

    BOOST_TEST(fragment.cells_idxs() == cells_idxs, boost::test_tools::per_element());
}


BOOST_AUTO_TEST_CASE(use_case02)
{
    lue::SerialRouteFragment<2>::CellsIdxs const cells_idxs{{0, 1}, {2, 3}, {4, 5}};

    // Create fragment given collection of cells idxs
    lue::SerialRouteFragment<2> fragment{cells_idxs};

    BOOST_TEST(fragment.cells_idxs() == cells_idxs, boost::test_tools::per_element());
}


BOOST_AUTO_TEST_CASE(use_case03)
{
    lue::SerialRouteFragment<2>::CellsIdxs cells_idxs{{0, 1}, {2, 3}, {4, 5}};

    // Create fragment given collection of cells idxs, and append
    lue::SerialRouteFragment<2> fragment{cells_idxs};
    fragment.append(cells_idxs[0]);
    fragment.append(cells_idxs[1]);
    fragment.append(cells_idxs[2]);

    cells_idxs.reserve(2 * cells_idxs.size());  // Make sure iterates stay valid during insert
    cells_idxs.insert(cells_idxs.end(), cells_idxs.begin(), cells_idxs.end());

    BOOST_TEST(fragment.cells_idxs() == cells_idxs, boost::test_tools::per_element());
}
