#pragma once
#include "lue/framework/algorithm/focal_operation.hpp"
#include <limits>


namespace lue {
namespace detail {

template<
    typename Element>
class FocalMax
{

public:

    using OutputElement = Element;

    FocalMax()
    {
    }

    constexpr OutputElement fill_value() const
    {
        return std::numeric_limits<OutputElement>::min();
    }

    template<
        typename Subspan,
        typename Kernel>
    // OutputElement operator()(
    OutputElement inner(
        Subspan const& window,
        Kernel const& kernel) const
    {
        static_assert(std::is_convertible_v<ElementT<Kernel>, bool>);

        OutputElement result = 0;
        bool result_initialized = false;

        static_assert(rank<Kernel> == 2);

        if constexpr(rank<Kernel> == 2) {
            assert(window.extent(0) == kernel.size());
            assert(window.extent(1) == kernel.size());

            for(Index r = 0; r < window.extent(0); ++r) {
                for(Index c = 0; c < window.extent(1); ++c) {
                    if(kernel(r, c)) {
                        if(!result_initialized) {
                            result = window(r, c);
                            result_initialized = true;
                        }
                        else {
                            result = std::max(result, window(r, c));
                        }
                    }
                }
            }
        }

        return result;
    }

    template<
        typename Subspans,
        typename Kernel>
    // OutputElement operator()(
    OutputElement border(
        Subspans const& windows,
        Kernel const& kernel) const
    {
        static_assert(std::is_convertible_v<ElementT<Kernel>, bool>);
        static_assert(rank<Kernel> == rank<Subspans>);

        OutputElement result = 0;
        bool result_initialized = false;

        static_assert(rank<Kernel> == 2);

        if constexpr(rank<Kernel> == 2) {

#ifndef NDEBUG

            // Although the cells are scattered over multiple partitions,
            // we must see the same number of cells as present in the kernel

            // Check number of columns in each row of partitions
            for(Count r = 0; r < windows.shape()[0]; ++r) {
                Count count = 0;

                for(Count c = 0; c < windows.shape()[1]; ++c) {
                    assert(windows(r, c).extent(1) > 0);
                    count += windows(r, c).extent(1);
                }

                assert(count == kernel.size());
            }

            // Check number of rows in each column of partitions
            for(Count c = 0; c < windows.shape()[1]; ++c) {
                Count count = 0;

                for(Count r = 0; r < windows.shape()[0]; ++r) {
                    assert(windows(r, c).extent(0) > 0);
                    count += windows(r, c).extent(0);
                }

                assert(count == kernel.size());
            }

            // Within a row of partitions, the number of rows per
            // partition must be the same
            for(Count r = 0; r < windows.shape()[0]; ++r) {
                Count const nr_rows = windows(r, 0).extent(0);

                for(Count c = 0; c < windows.shape()[1]; ++c) {

                    assert(windows(r, c).extent(0) == nr_rows);
                }
            }

            // Within a column of partitions, the number of columns per
            // partition must be the same
            for(Count c = 0; c < windows.shape()[1]; ++c) {
                Count const nr_cols = windows(0, c).extent(1);

                for(Count r = 0; r < windows.shape()[0]; ++r) {
                    assert(windows(r, c).extent(1) == nr_cols);
                }
            }

#endif

            // {r,c}w : {row,col} index window

            Index rk = 0;

            // Iterate over all views into partitions
            for(Count rw = 0; rw < windows.shape()[0]; ++rw) {

                Index ck = 0;

                for(Count cw = 0; cw < windows.shape()[1]; ++cw) {

                    auto const& window = windows(rw, cw);

                    assert(window.extent(0) <= kernel.size());
                    assert(window.extent(1) <= kernel.size());

                    for(Index r = 0; r < window.extent(0); ++r, ++rk) {
                        for(Index c = 0; c < window.extent(1); ++c, ++ck) {

                            assert(rk < std::get<0>(kernel.shape()));
                            assert(ck < std::get<1>(kernel.shape()));

                            if(kernel(r, c)) {
                                if(!result_initialized) {
                                    result = window(r, c);
                                    result_initialized = true;
                                }
                                else {
                                    result = std::max(result, window(r, c));
                                }
                            }

                        }

                        ck -= window.extent(1);  // Carriage return
                    }

                    // Entering next partition in row

                    // Reset kernel row index to start row
                    rk -= window.extent(0);

                    // Offset kernel col indices by previous partition's cols
                    ck += window.extent(1);

                }

                // Offset kernel row indices by previous partition's rows
                rk += windows(rw, 0).extent(0);
            }
        }

        return result;
    }

};

}  // namespace detail


// focal_max: given
// - a partitioned array
//     - Element type is not relevant. As long as the values support
//       being compared using operator<.
// - a kernel
//     - Element type is not relevant. As long as the values support
//       being evaluated as boolean.
// iterate kernel over array and store max value found in kernel window
// in focal cell

// Implementation calls a more generic algorithm accepting a partitioned
// array, a kernel and a functor: focal operation
// This algorithm handles the halo partitions around the array, the
// partitioning of the algorithm, and calling the functor, passing in
// the kernel and the (view on the) elements

template<
    typename Array,
    typename Kernel>
Array focal_max(
    Array const& array,
    Kernel const& kernel)
{
    return focal_operation(array, kernel, detail::FocalMax<ElementT<Array>>{});
}

}  // namespace lue
