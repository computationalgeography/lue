#pragma once
#include "lue/item/constant_size/constant/same_shape.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {

/*!
    @brief      Class for representing time boxes shared by all items

    A time box is defined by a start and end time point. For each time
    domain item a pair of time points is stored.
*/
class TimeBox:
    public constant::same_shape::Collection
{

public:

                   TimeBox             (hdf5::Group const& parent,
                                        hdf5::Datatype const memory_datatype);

                   TimeBox             (constant::same_shape::Collection&&
                                            collection);

                   TimeBox             (TimeBox const&)=delete;

                   TimeBox             (TimeBox&&)=default;

                   ~TimeBox            ()=default;

    TimeBox&       operator=           (TimeBox const&)=delete;

    TimeBox&       operator=           (TimeBox&&)=default;

private:

};


TimeBox            create_time_box     (hdf5::Group const& parent,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype);

}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
