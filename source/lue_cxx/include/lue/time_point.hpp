#pragma once
#include "lue/item/constant_collection/constant_shape/same_shape/asynchronous_variable.hpp"
#include "lue/clock.hpp"


namespace lue {

class TimePoint:
    public constant_collection::constant_shape::same_shape::AsynchronousVariable
{

public:

                   TimePoint           (hdf5::Group const& parent);

                   TimePoint           (TimePoint const&)=delete;

                   TimePoint           (TimePoint&&)=default;

                   ~TimePoint          ()=default;

    TimePoint&     operator=           (TimePoint const&)=delete;

    TimePoint&     operator=           (TimePoint&&)=default;

private:

};


TimePoint          create_time_point   (hdf5::Group& parent,
                                        Clock const& clock);

}  // namespace lue
