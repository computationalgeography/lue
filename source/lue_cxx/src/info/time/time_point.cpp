#include "lue/info/time/time_point.hpp"
#include "lue/core/tag.hpp"


namespace lue {

TimePoint::TimePoint(
    hdf5::Group const& parent)
:
    same_shape::constant_shape::Value{
        parent, coordinates_tag,
        hdf5::Datatype{
            hdf5::NativeDatatypeTraits<time::DurationCount>::type_id()}}

{
}


TimePoint::TimePoint(
    same_shape::constant_shape::Value&& value)
:
    same_shape::constant_shape::Value{
        std::forward<same_shape::constant_shape::Value>(value)}

{
}


TimePoint create_time_point(
    hdf5::Group& parent,
    lue::Clock const& /* clock */)
{
    // Given a clock, time points can be represented by durations since
    // the clock's epoch. Durations can be represented by an amount of
    // ticks, which is just a count.
    hdf5::Datatype memory_datatype{
        hdf5::NativeDatatypeTraits<time::DurationCount>::type_id()};
    hdf5::Datatype file_datatype{
        hdf5::StandardDatatypeTraits<time::DurationCount>::type_id()};
    hdf5::Shape value_shape{1};

    auto value = same_shape::constant_shape::create_value(
        parent, coordinates_tag, file_datatype, memory_datatype, value_shape);

    return TimePoint{std::move(value)};
}

}  // namespace lue
