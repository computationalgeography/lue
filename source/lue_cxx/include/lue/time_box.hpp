#pragma once
#include "lue/item/constant_shape/same_shape/asynchronous_value.hpp"
#include "lue/clock.hpp"


namespace lue {

class TimeBox:
    public constant_shape::same_shape::AsynchronousValue
{

public:

                   TimeBox             (hdf5::Group const& parent);

                   TimeBox             (TimeBox const&)=delete;

                   TimeBox             (TimeBox&&)=default;

                   ~TimeBox            ()=default;

    TimeBox&       operator=           (TimeBox const&)=delete;

    TimeBox&       operator=           (TimeBox&&)=default;

private:

};


TimeBox            create_time_box     (hdf5::Group& parent,
                                        Clock const& clock);

}  // namespace lue
