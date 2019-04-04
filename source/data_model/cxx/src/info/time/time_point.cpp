#include "lue/info/time/time_point.hpp"


namespace lue {

TimePoint::TimePoint(
    hdf5::Group& parent):

    LocationInTime{parent}

{
}


TimePoint::TimePoint(
    LocationInTime&& value):

    LocationInTime{std::forward<LocationInTime>(value)}

{
}


Count TimePoint::nr_points() const
{
    return nr_locations();
}


TimePoint create_time_point(
    hdf5::Group& parent)
{
    auto value = create_location_in_time(parent, hdf5::Shape{1});

    return TimePoint{std::move(value)};
}

}  // namespace lue
