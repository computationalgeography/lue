#pragma once
#include "lue/info/time/time_box.hpp"


namespace lue {

/*!
    @brief      Class for representing time box domains that are
                discretized using regular grids

    Time cells are time boxes with additional information about the
    number of cells in which each box is discretized.
*/
class TimeCell:
    public TimeBox
{

public:

    using Counts = same_shape::constant_shape::Value;

                   TimeCell            (hdf5::Group& parent);

                   TimeCell            (TimeCell const&)=delete;

                   TimeCell            (TimeCell&&)=default;

                   TimeCell            (TimeBox&& time_box,
                                        Counts&& counts);

                   ~TimeCell           ()=default;

    TimeCell&      operator=           (TimeCell const&)=delete;

    TimeCell&      operator=           (TimeCell&&)=default;

    Count          nr_counts           () const;

    Counts const&  counts              () const;

    Counts&        counts              ();

private:

    //! For each time box a count
    Counts _counts;

};


TimeCell            create_time_cell   (hdf5::Group& parent,
                                        lue::Clock const& clock);

}  // namespace lue
