#pragma once
#include "lue/constant_size/time/omnipresent/same_shape/value.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {

/*!
    @brief      Class for representing time boxes shared by all items

    A time box is defined by a start and end time point. For each time
    domain item a pair of time points is stored.

    From the perspective of a time domain, the items are omnipresent:
    time domain information doesn't change over time(!).
    Therefore, this class inherits from omnipresent::same_shape::Value.
*/
class TimeBox:
    public omnipresent::same_shape::Value
{

public:

                   TimeBox             (hdf5::Identifier const& location,
                                        hdf5::Datatype const memory_datatype);

                   TimeBox             (omnipresent::same_shape::Value&&
                                            value);

                   TimeBox             (TimeBox const& other)=delete;

                   TimeBox             (TimeBox&& other)=default;

                   ~TimeBox            ()=default;

    TimeBox&       operator=           (TimeBox const& other)=delete;

    TimeBox&       operator=           (TimeBox&& other)=default;

private:


};


TimeBox            create_time_box     (hdf5::Group const& group,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype);

}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
