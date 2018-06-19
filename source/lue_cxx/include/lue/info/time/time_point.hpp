#pragma once
#include "lue/array/same_shape/constant_shape/value.hpp"
#include "lue/core/clock.hpp"


namespace lue {

/*!
    - Zero or more time points per item
    - Each time point has a unique location in time
*/
class TimePoint:
    public same_shape::constant_shape::Value
{

public:

                   TimePoint           (hdf5::Group const& parent);

                   TimePoint           (TimePoint const&)=delete;

                   TimePoint           (TimePoint&&)=default;

                   TimePoint           (
                                    same_shape::constant_shape::Value&& value);

                   ~TimePoint          ()=default;

    TimePoint&     operator=           (TimePoint const&)=delete;

    TimePoint&     operator=           (TimePoint&&)=default;

private:

};


TimePoint          create_time_point   (hdf5::Group& parent,
                                        lue::Clock const& clock);

}  // namespace lue
