#define BOOST_TEST_MODULE lue framework core span
#include "lue/framework/core/span.hpp"
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(dynamic_span_test)
{
    lue::Count const nr_rows = 60;
    lue::Count const nr_cols = 40;
    lue::Count const nr_cells = nr_rows * nr_cols;

    // NOLINTNEXTLINE(clang-diagnostic-unused-lambda-capture)
    auto idx = [nr_rows, nr_cols](lue::Index const row, lue::Index const col) -> lue::Index
    { return row * nr_cols + col; };

    // Allocate a 1D array
    std::vector<int> cells(nr_cells);

    // Fill array with increasing valueѕ
    std::iota(cells.begin(), cells.end(), 0);

    // View 1D array as a 2D array
    lue::DynamicSpan<int, 2> raster(cells.data(), nr_rows, nr_cols);
    BOOST_REQUIRE_EQUAL(raster(0, 0), 0);
    BOOST_REQUIRE_EQUAL(raster(3, 4), idx(3, 4));
    BOOST_REQUIRE_EQUAL(raster(nr_rows - 1, nr_cols - 1), nr_cells - 1);

    // View part of the 2D array as a new view

    // Select a single cell
    {
        auto const cell = lue::subspan(raster, 3, 4);
        BOOST_CHECK_EQUAL(cell.rank(), 0);
        BOOST_CHECK_EQUAL(cell(), idx(3, 4));
    }

    // Select a single row
    {
        auto const row = lue::subspan(raster, 3, std::experimental::all);
        BOOST_CHECK_EQUAL(row.rank(), 1);
        BOOST_REQUIRE_EQUAL(row.extent(0), nr_cols);
        BOOST_CHECK_EQUAL(row(0), idx(3, 0));
        BOOST_CHECK_EQUAL(row(1), idx(3, 0 + 1));
    }

    // Select a single column
    {
        auto const col = lue::subspan(raster, std::experimental::all, 4);
        BOOST_CHECK_EQUAL(col.rank(), 1);
        BOOST_REQUIRE_EQUAL(col.extent(0), nr_rows);
        BOOST_CHECK_EQUAL(col(0), idx(0, 4));
        BOOST_CHECK_EQUAL(col(1), idx(0 + 1, 4));
    }


    // Select a window
    {
        lue::Count const radius = 1;
        lue::Count const window_size = 2 * radius + 1;
        auto const window = lue::subspan(
            raster,
            std::pair<lue::Index, lue::Index>{3, 3 + window_size},
            std::pair<lue::Index, lue::Index>{4, 4 + window_size});
        BOOST_CHECK_EQUAL(window.rank(), 2);
        BOOST_REQUIRE_EQUAL(window.extent(0), window_size);
        BOOST_REQUIRE_EQUAL(window.extent(1), window_size);
        BOOST_CHECK_EQUAL(window(0, 0), idx(3 + 0, 4 + 0));
        BOOST_CHECK_EQUAL(window(0, 1), idx(3 + 0, 4 + 1));
        BOOST_CHECK_EQUAL(window(0, 2), idx(3 + 0, 4 + 2));
        BOOST_CHECK_EQUAL(window(1, 0), idx(3 + 1, 4 + 0));
        BOOST_CHECK_EQUAL(window(1, 1), idx(3 + 1, 4 + 1));
        BOOST_CHECK_EQUAL(window(1, 2), idx(3 + 1, 4 + 2));
        BOOST_CHECK_EQUAL(window(2, 0), idx(3 + 2, 4 + 0));
        BOOST_CHECK_EQUAL(window(2, 1), idx(3 + 2, 4 + 1));
        BOOST_CHECK_EQUAL(window(2, 2), idx(3 + 2, 4 + 2));
    }
}
