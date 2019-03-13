#include "lue/info/time/time_box.hpp"
#include "lue/core/tag.hpp"


namespace lue {

TimeBox::TimeBox(
    hdf5::Group& parent)
:
    same_shape::constant_shape::Value{
        parent, coordinates_tag,
        hdf5::Datatype{
            hdf5::NativeDatatypeTraits<time::DurationCount>::type_id()}}

{
}


TimeBox::TimeBox(
    same_shape::constant_shape::Value&& value)
:
    same_shape::constant_shape::Value{
        std::forward<same_shape::constant_shape::Value>(value)}

{
}


Count TimeBox::nr_boxes() const
{
    return nr_arrays();
}


TimeBox create_time_box(
    hdf5::Group& parent,
    lue::Clock const& /* clock */)
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

    auto value = same_shape::constant_shape::create_value(
        parent, coordinates_tag, file_datatype, memory_datatype, value_shape);

    return TimeBox{std::move(value)};
}

}  // namespace lue
