#include "lue/info/time/time_box.hpp"


namespace lue {
    namespace data_model {

        TimeBox::TimeBox(hdf5::Group const& parent):

            LocationInTime{parent}

        {
        }


        TimeBox::TimeBox(LocationInTime&& value):

            LocationInTime{std::move(value)}

        {
        }


        Count TimeBox::nr_boxes() const
        {
            return nr_locations();
        }


        TimeBox create_time_box(hdf5::Group& parent)
        {
            LocationInTime value = create_location_in_time(parent, hdf5::Shape{2});

            return TimeBox{std::move(value)};
        }

    }  // namespace data_model
}  // namespace lue
