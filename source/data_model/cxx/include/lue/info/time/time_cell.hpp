#pragma once
#include "lue/info/time/time_box.hpp"


namespace lue {
namespace data_model {

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

    using Count = same_shape::constant_shape::Value;

                   TimeCell            (hdf5::Group& parent);

                   TimeCell            (TimeCell const&)=delete;

                   TimeCell            (TimeCell&&)=default;

                   TimeCell            (TimeBox&& time_box,
                                        Count&& count);

                   ~TimeCell           ()=default;

    TimeCell&      operator=           (TimeCell const&)=delete;

    TimeCell&      operator=           (TimeCell&&)=default;

    data_model::Count
                   nr_counts           () const;

    Count const&   count               () const;

    Count&         count               ();

private:

    //! For each time box a count
    Count          _count;

};


TimeCell            create_time_cell   (hdf5::Group& parent);

}  // namespace data_model
}  // namespace lue
