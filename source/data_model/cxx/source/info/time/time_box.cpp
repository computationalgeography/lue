#include "lue/info/time/time_box.hpp"


namespace lue::data_model {

    TimeBox::TimeBox(hdf5::Group const& parent):

        LocationInTime{parent}

    {
    }


    TimeBox::TimeBox(LocationInTime&& value):

        LocationInTime{std::move(value)}

    {
    }


    auto TimeBox::nr_boxes() const -> Count
    {
        return nr_locations();
    }


    auto create_time_box(hdf5::Group& parent) -> TimeBox
    {
        LocationInTime value = create_location_in_time(parent, hdf5::Shape{2});

        return TimeBox{std::move(value)};
    }

}  // namespace lue::data_model
