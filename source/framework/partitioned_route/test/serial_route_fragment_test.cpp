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
    lue::SerialRouteFragment<2>::CellIdxs const cell_idxs{1, 3, 5};

    // Create fragment as we go
    lue::SerialRouteFragment<2> fragment;
    fragment.append(cell_idxs[0]);
    fragment.append(cell_idxs[1]);
    fragment.append(cell_idxs[2]);

    BOOST_TEST(fragment.cell_idxs() == cell_idxs, boost::test_tools::per_element());
}


BOOST_AUTO_TEST_CASE(use_case02)
{
    lue::SerialRouteFragment<2>::CellIdxs const cell_idxs{1, 3, 5};

    // Create fragment given collection of cells idxs
    lue::SerialRouteFragment<2> fragment{cell_idxs};

    BOOST_TEST(fragment.cell_idxs() == cell_idxs, boost::test_tools::per_element());
}


BOOST_AUTO_TEST_CASE(use_case03)
{
    lue::SerialRouteFragment<2>::CellIdxs cell_idxs{1, 3, 5};

    // Create fragment given collection of cells idxs, and append
    lue::SerialRouteFragment<2> fragment{cell_idxs};
    fragment.append(cell_idxs[0]);
    fragment.append(cell_idxs[1]);
    fragment.append(cell_idxs[2]);

    cell_idxs.reserve(2 * cell_idxs.size());  // Make sure iterators stay valid during insert
    cell_idxs.insert(cell_idxs.end(), cell_idxs.begin(), cell_idxs.end());

    BOOST_TEST(fragment.cell_idxs() == cell_idxs, boost::test_tools::per_element());
}
