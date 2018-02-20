#include "lue/time_box.hpp"
#include "lue/tag.hpp"


namespace lue {

TimeBox::TimeBox(
    hdf5::Group const& parent)

    : same_shape::constant_shape::AsynchronousValue{
        parent, coordinates_tag,
        hdf5::Datatype{
            hdf5::NativeDatatypeTraits<time::DurationCount>::type_id()}}

{
}


TimeBox create_time_box(
    hdf5::Group& parent,
    Clock const& /* clock */)
{
    // A time box is defined by the start and end time points.
    // Given a clock, time points can be represented by durations since
    // the clock's epoch. Durations can be represented by an amount of
    // ticks, which is just a count.
    hdf5::Datatype memory_datatype{
        hdf5::NativeDatatypeTraits<time::DurationCount>::type_id()};
    hdf5::Datatype file_datatype{
        hdf5::StandardDatatypeTraits<time::DurationCount>::type_id()};
    hdf5::Shape value_shape{2};

    same_shape::constant_shape::create_asynchronous_value(
        parent, coordinates_tag, file_datatype, memory_datatype, value_shape);

    return TimeBox{parent};
}

}  // namespace lue
