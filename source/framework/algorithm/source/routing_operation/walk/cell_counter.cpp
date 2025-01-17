#include "lue/framework/algorithm/routing/walk/cell_counter.hpp"
#include "lue/framework/core/assert.hpp"


namespace lue {

    CellCounter::CellCounter():

        CellCounter{0}

    {
    }


    CellCounter::CellCounter(Count const max_nr_cells_to_visit):

        _nr_cells_visited{0},
        _max_nr_cells_to_visit{max_nr_cells_to_visit}

    {
    }


    auto CellCounter::operator++() -> Count&
    {
        return ++_nr_cells_visited;
    }


    auto CellCounter::keep_walking() const -> bool
    {
        return _nr_cells_visited < _max_nr_cells_to_visit;
    }

}  // namespace lue
