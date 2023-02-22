#include "lue/info/time/location_in_time.hpp"
#include "lue/core/aspect.hpp"
#include "lue/core/tag.hpp"


namespace lue {
    namespace data_model {

        LocationInTime::LocationInTime(hdf5::Group const& parent):

            same_shape::constant_shape::Value{
                parent, coordinates_tag, hdf5::Datatype{hdf5::NativeDatatypeTraits<Element>::type_id()}}

        {
        }


        LocationInTime::LocationInTime(same_shape::constant_shape::Value&& value):

            same_shape::constant_shape::Value{std::move(value)}

        {
        }


        Count LocationInTime::nr_locations() const
        {
            return nr_arrays();
        }


        LocationInTime create_location_in_time(hdf5::Group& parent, hdf5::Shape const& value_shape)
        {
            // Given a clock, locations in time can be represented by durations
            // since the clock's epoch. Durations can be represented by an
            // amount of ticks, which is just a count.

            hdf5::Datatype const memory_datatype{
                hdf5::NativeDatatypeTraits<ElementT<LocationInTime>>::type_id()};
            hdf5::Datatype const file_datatype{
                hdf5::StandardDatatypeTraits<ElementT<LocationInTime>>::type_id()};

            same_shape::constant_shape::Value value{same_shape::constant_shape::create_value(
                parent, coordinates_tag, file_datatype, memory_datatype, value_shape)};

            return LocationInTime{std::move(value)};
        }

    }  // namespace data_model
}  // namespace lue
