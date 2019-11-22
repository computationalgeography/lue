#define BOOST_TEST_MODULE lue framework core array_visitor_test
#include <hpx/config.hpp>
#include <boost/test/unit_test.hpp>
#include "lue/framework/core/array_visitor.hpp"
#include "lue/framework/core/shape.hpp"
#include "lue/framework/test/stream.hpp"


namespace {

template<
    typename Shape>
class Visitor:
    public lue::ArrayVisitor<Shape>
{

public:

    using CellIndices = std::vector<lue::Index>;

    Visitor(
        Shape const& shape):

        lue::ArrayVisitor<Shape>{shape},
        _cell_indices{}

    {
    }

    CellIndices const& cell_indices() const
    {
        return _cell_indices;
    }

    void operator()()
    {
        // Record linear index of current cell
        _cell_indices.push_back(this->cursor().linear_idx());
    }

private:

    // Indices of cells visited
    CellIndices _cell_indices;

};


lue::Rank const rank = 2;
using Shape = lue::Shape<lue::Index, rank>;
using Cursor = lue::detail::ArrayVisitorCursor<Shape>;

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(visit_all_cells)
{
    Shape shape{{6, 4}};
    Cursor cursor{shape};

    BOOST_CHECK_EQUAL(cursor.start(), (Shape{0, 0}));
    BOOST_CHECK_EQUAL(cursor.count(), (Shape{6, 4}));
    BOOST_CHECK_EQUAL(cursor.dimension_idx(), 0);

    // 0, 0 → 0
    // ^
    BOOST_CHECK_EQUAL(cursor.current_cell(), (Shape{0, 0}));
    BOOST_CHECK_EQUAL(cursor.linear_idx(), 0);

    // 1, 0 → 4
    // ^
    ++cursor;
    BOOST_CHECK_EQUAL(cursor.current_cell(), (Shape{1, 0}));
    BOOST_CHECK_EQUAL(cursor.linear_idx(), 4);

    // 2, 0 → 8
    // ^
    ++cursor;
    BOOST_CHECK_EQUAL(cursor.current_cell(), (Shape{2, 0}));
    BOOST_CHECK_EQUAL(cursor.linear_idx(), 8);

    // 2, 0 → 8
    //    ^
    cursor.enter_next_dimension();
    BOOST_CHECK_EQUAL(cursor.current_cell(), (Shape{2, 0}));
    BOOST_CHECK_EQUAL(cursor.dimension_idx(), 1);

    // 2, 1 → 9
    //    ^
    ++cursor;
    BOOST_CHECK_EQUAL(cursor.current_cell(), (Shape{2, 1}));
    BOOST_CHECK_EQUAL(cursor.linear_idx(), 9);

    // 2, 3 → 11
    //    ^
    ++cursor;
    ++cursor;
    BOOST_CHECK_EQUAL(cursor.current_cell(), (Shape{2, 3}));
    BOOST_CHECK_EQUAL(cursor.linear_idx(), 11);

    // 2, 0 → 8
    // ^
    cursor.leave_current_dimension();
    BOOST_CHECK_EQUAL(cursor.current_cell(), (Shape{2, 0}));
    BOOST_CHECK_EQUAL(cursor.linear_idx(), 8);

    // 3, 0 → 12
    // ^
    ++cursor;
    BOOST_CHECK_EQUAL(cursor.current_cell(), (Shape{3, 0}));
    BOOST_CHECK_EQUAL(cursor.linear_idx(), 12);
}


BOOST_AUTO_TEST_CASE(visit_some_cells)
{
    // Visit only cells at indices 9, 10, 13, 14

    Shape shape{{6, 4}};
    Shape start{{2, 1}};
    Shape count{{2, 2}};
    Cursor cursor{shape, start, count};

    BOOST_CHECK_EQUAL(cursor.start(), start);
    BOOST_CHECK_EQUAL(cursor.count(), count);
    BOOST_CHECK_EQUAL(cursor.dimension_idx(), 0);

    // 2, 1 → 9
    // ^
    BOOST_CHECK_EQUAL(cursor.current_cell(), (Shape{2, 1}));
    BOOST_CHECK_EQUAL(cursor.linear_idx(), 9);

    // 2, 1 → 9
    //    ^
    cursor.enter_next_dimension();
    BOOST_CHECK_EQUAL(cursor.current_cell(), (Shape{2, 1}));
    BOOST_CHECK_EQUAL(cursor.linear_idx(), 9);

    // 2, 2 → 10
    //    ^
    ++cursor;
    BOOST_CHECK_EQUAL(cursor.current_cell(), (Shape{2, 2}));
    BOOST_CHECK_EQUAL(cursor.linear_idx(), 10);

    // 2, 1 → 9
    // ^
    cursor.leave_current_dimension();
    BOOST_CHECK_EQUAL(cursor.current_cell(), (Shape{2, 1}));
    BOOST_CHECK_EQUAL(cursor.linear_idx(), 9);

    // 3, 1 → 13
    // ^
    ++cursor;
    BOOST_CHECK_EQUAL(cursor.current_cell(), (Shape{3, 1}));
    BOOST_CHECK_EQUAL(cursor.linear_idx(), 13);

    // 3, 1 → 13
    //    ^
    cursor.enter_next_dimension();
    BOOST_CHECK_EQUAL(cursor.current_cell(), (Shape{3, 1}));
    BOOST_CHECK_EQUAL(cursor.linear_idx(), 13);

    // 3, 2 → 14
    //    ^
    ++cursor;
    BOOST_CHECK_EQUAL(cursor.current_cell(), (Shape{3, 2}));
    BOOST_CHECK_EQUAL(cursor.linear_idx(), 14);
}


BOOST_AUTO_TEST_CASE(use_case_1)
{
    Shape shape{{6, 4}};
    Visitor<Shape> visitor{shape};

    Shape begin_indices{{0, 0}};
    Shape end_indices{{6, 4}};
    auto const nr_elements = lue::nr_elements(shape);

    visit_array(begin_indices, end_indices, visitor);

    Visitor<Shape>::CellIndices const& cell_indices = visitor.cell_indices();

    BOOST_REQUIRE_EQUAL(cell_indices.size(), nr_elements);
    BOOST_CHECK_EQUAL(cell_indices[0], 0);
    BOOST_CHECK_EQUAL(cell_indices[1], 1);
    BOOST_CHECK_EQUAL(cell_indices[nr_elements - 2], nr_elements - 2);
    BOOST_CHECK_EQUAL(cell_indices[nr_elements - 1], nr_elements - 1);
}
