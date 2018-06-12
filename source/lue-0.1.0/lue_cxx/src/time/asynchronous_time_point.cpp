#include "lue/time/asynchronous_time_point.hpp"
#include "lue/tag.hpp"


namespace lue {

AsynchronousTimePoint::AsynchronousTimePoint(
    hdf5::Group const& parent)

    : same_shape::constant_shape::AsynchronousValue{
        parent, coordinates_tag,
        hdf5::Datatype{
            hdf5::NativeDatatypeTraits<time::DurationCount>::type_id()}}

{
}


AsynchronousTimePoint create_asynchronous_time_point(
    hdf5::Group& parent,
    Clock const& /* clock */)
{
    // Given a clock, time points can be represented by durations since
    // the clock's epoch. Durations can be represented by an amount of
    // ticks, which is just a count.
    hdf5::Datatype memory_datatype{
        hdf5::NativeDatatypeTraits<time::DurationCount>::type_id()};
    hdf5::Datatype file_datatype{
        hdf5::StandardDatatypeTraits<time::DurationCount>::type_id()};
    hdf5::Shape value_shape{1};

    same_shape::constant_shape::create_asynchronous_value(
        parent, coordinates_tag, file_datatype, memory_datatype, value_shape);

    return AsynchronousTimePoint{parent};
}

}  // namespace lue
