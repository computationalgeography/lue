#pragma once
#include "lue/array/same_shape/constant_shape/value.hpp"
#include "lue/core/clock.hpp"


namespace lue {

/*!
    - Zero or more time boxes per item
    - Each time box has a unique location in time
*/
class TimeBox:
    public same_shape::constant_shape::Value
{

public:

                   TimeBox             (hdf5::Group& parent);

                   TimeBox             (TimeBox const&)=delete;

                   TimeBox             (TimeBox&&)=default;

                   TimeBox             (
                                    same_shape::constant_shape::Value&& value);

                   ~TimeBox            ()=default;

    TimeBox&       operator=           (TimeBox const&)=delete;

    TimeBox&       operator=           (TimeBox&&)=default;

private:

};


TimeBox            create_time_box     (hdf5::Group& parent,
                                        lue::Clock const& clock);

}  // namespace lue
