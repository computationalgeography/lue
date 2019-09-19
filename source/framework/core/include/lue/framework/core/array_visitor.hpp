#pragma once
#include "lue/framework/core/shape.hpp"
#include <algorithm>
#include <cassert>
#include <numeric>


namespace lue {

template<
    typename Shape>
using IndexT = typename Shape::value_type;


template<
    std::size_t rank,
    typename Iterator>
typename std::iterator_traits<Iterator>::value_type linear_idx(
    Iterator cell_idxs_it,
    [[maybe_unused]] Iterator shape_it)
{
    using ValueType = typename std::iterator_traits<Iterator>::value_type;

    if constexpr (rank == 1) {
        return *cell_idxs_it;
    }
    else {
        return
            (*cell_idxs_it * std::accumulate(
                shape_it + 1, shape_it + rank, 1u,
                std::multiplies<ValueType>())) +
            linear_idx<rank-1>(cell_idxs_it + 1, shape_it + 1);
    }
}


template<
    typename Shape>
IndexT<Shape> linear_idx(
    Shape const& cell_idxs,
    Shape const& shape)
{
    constexpr auto rank = std::tuple_size<Shape>();  // rank<Shape>();

    assert(std::size(cell_idxs) == rank);
    assert(std::size(shape) == rank);

    return linear_idx<rank>(cell_idxs.begin(), shape.begin());
}


namespace detail {

/*!
    @brief      Class for representing a cursor in an nD array

    A cursor knows about the shape of the array and has a notion of the
    current location visited. This location can be queried and manipulated
    using the cursor's API.
*/
template<
    typename Shape>
class ArrayVisitorCursor
{

public:

    using Index = IndexT<Shape>;

    ArrayVisitorCursor(
        Shape const& shape):

        _shape{shape},
        _start{},
        _count{},
        _current_cell{},
        _dimension_idx{}

    {
        Shape start{};
        std::fill(start.begin(), start.end(), 0);
        init(start, shape);
    }

    ArrayVisitorCursor(
        Shape const& shape,
        Shape const& start,
        Shape const& count):

        _shape{shape},
        _start{},
        _count{},
        _current_cell{},
        _dimension_idx{}

    {
        init(start, count);
    }

    void init(
        Shape const& start,
        Shape const& count)
    {
        _start = start;
        _count = count;
        _current_cell = start;
        _dimension_idx = 0;

        for(std::size_t i = 0; i < std::tuple_size<Shape>(); ++i) {
            assert(_start[i] + _count[i] <= _shape[i]);
        }
    }

    /*!
        @brief      Enter an additional dimension

        This function does some administration to allow the user to
        figure out which cells are selected later on.
    */
    void enter_next_dimension()
    {
        ++_dimension_idx;

        assert(_dimension_idx < std::tuple_size<Shape>());
    }

    void leave_current_dimension()
    {
        // Reset index of current dimension to start of hyperslab
        _current_cell[_dimension_idx] = _start[_dimension_idx];

        assert(_dimension_idx > 0);

        --_dimension_idx;
    }

    ArrayVisitorCursor& operator++()
    {
        assert(_current_cell[_dimension_idx] < _shape[_dimension_idx]);

        ++_current_cell[_dimension_idx];

        return *this;
    }

    Shape const& start() const
    {
        return _start;
    }

    Shape const& count() const
    {
        return _count;
    }

    Index start_idx() const
    {
        return _start[_dimension_idx];
    }

    Index end_idx() const
    {
        return _start[_dimension_idx] + _count[_dimension_idx];
    }

    Shape const& current_cell() const
    {
        return _current_cell;
    }

    std::size_t dimension_idx() const
    {
        return _dimension_idx;
    }

    /*!
        @brief      Return the linear index of the currently visited cell

        The index returned is probably only useful once the visitor has
        ended up at a selected cell.
    */
    std::size_t linear_idx() const
    {
        return lue::linear_idx(_current_cell, _shape);
    }

private:

    //! Shape in cells of array
    Shape const    _shape;

    //! Cell indices of start of hyperslab
    Shape          _start;

    //! Extent of hyperslab in all dimensions
    Shape          _count;

    //! Cell indices of currently selected cell
    Shape          _current_cell;

    //! Current dimension being visited
    std::size_t    _dimension_idx;

};

}  // namespace detail


/*!
    @brief      Base class for visitors used to visit (a subset of)
                the elements in an array
*/
template<
    typename Shape>
class ArrayVisitor
{

    using Cursor = detail::ArrayVisitorCursor<Shape>;

public:

    /*!
        @brief      Construct an instance based on the array shape
    */
    ArrayVisitor(
        Shape const& shape):

        _cursor{shape}

    {
    }

    /*!
        @brief      Initialize the visit using the start indices and
                    extents of the hyperslab
    */
    void init(
        Shape const& start,
        Shape const& count)
    {
        _cursor.init(start, count);
    }

    /*!
        @sa         Cursor::enter_next_dimension()
    */
    void enter_next_dimension()
    {
        _cursor.enter_next_dimension();
    }

    /*!
        @sa         Cursor::leave_current_dimension()
    */
    void leave_current_dimension()
    {
        _cursor.leave_current_dimension();
    }

    /*!
        @sa         Cursor::operator++()
    */
    ArrayVisitor& operator++()
    {
        ++_cursor;

        return *this;
    }

    /*!
        @brief      Return the cursor
    */
    Cursor const& cursor() const
    {
        return _cursor;
    }

private:

    //! Cursor used during visit
    Cursor         _cursor;

};


namespace detail {

/*!
    @brief      Visit a range of cells defined by indices
    @param      visitor Visitor to manipulate/call during the visit of
                selected cells
*/
template<
    std::size_t rank,
    typename Visitor
>
void visit_array(
    Visitor& visitor)
{
    if constexpr (rank == 0) {

        // We have navigated along all array dimensions and can now
        // notify the visitor that we have arrived at one of the selected
        // cells. It should do its thing, whatever that is.

        visitor();

    }
    else {

        // We are still navigating along array dimensions. The latter
        // dimensions vary fastest: depth-first visit.

        auto idx = visitor.cursor().start_idx();

        for(; idx < visitor.cursor().end_idx(); ++idx) {
            // Recurse into next dimension
            if constexpr (rank > 1) {
                visitor.enter_next_dimension();
            }

            visit_array<rank - 1>(visitor);

            if constexpr (rank > 1) {
                visitor.leave_current_dimension();
            }

            ++visitor;
        }

    }
}

}  // namespace detail


/*!
    @brief      Visit (a subset of) the elements in an array
    @param      begin_indices Begin indices along all dimensions of
                hyperslab to visit
    @param      end_indices End indices along all dimensions of hyperslab
                to visit
*/
template<
    typename Index,
    std::size_t rank,
    typename Visitor
>
void visit_array(
    Shape<Index, rank> const& begin_indices,
    Shape<Index, rank> const& end_indices,
    Visitor& visitor)
{
    Shape<Index, rank> count;

    for(std::size_t i = 0; i < rank; ++i) {
        assert(end_indices[i] > begin_indices[i]);
        count[i] = end_indices[i] - begin_indices[i];
    }

    visitor.init(begin_indices, count);

    detail::visit_array<rank>(visitor);
}

}  // namespace lue
