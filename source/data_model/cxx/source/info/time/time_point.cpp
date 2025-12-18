#include "lue/info/time/time_point.hpp"


namespace lue::data_model {

    TimePoint::TimePoint(hdf5::Group const& parent):

        LocationInTime{parent}

    {
    }


    TimePoint::TimePoint(LocationInTime&& value):

        LocationInTime{std::move(value)}

    {
    }


    auto TimePoint::nr_points() const -> Count
    {
        return nr_locations();
    }


    auto create_time_point(hdf5::Group& parent) -> TimePoint
    {
        LocationInTime value{create_location_in_time(parent, hdf5::Shape{1})};

        return TimePoint{std::move(value)};
    }

}  // namespace lue::data_model
