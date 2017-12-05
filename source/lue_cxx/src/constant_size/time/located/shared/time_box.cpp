#include "lue/constant_size/time/located/shared/time_box.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {

TimeBox::TimeBox(
    hdf5::Group const& parent,
    hdf5::Datatype const memory_datatype)

    : constant::SameShape{parent, coordinates_tag, memory_datatype}

{
}


TimeBox::TimeBox(
    constant::SameShape&& collection)

    : constant::SameShape{std::forward<constant::SameShape>(collection)}

{
}


TimeBox create_time_box(
    hdf5::Group const& parent,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype)
{
    // A time box is defined by the start and end time points. Given a clock,
    // time points can be represented by durations since the clock's epoch.
    // Durations can be represented by an amount of ticks, which is just a
    // count.
    hdf5::Shape value_shape = { 2 };

    auto dataset = constant::create_same_shape(
        parent, coordinates_tag, file_datatype, memory_datatype, value_shape);

    return TimeBox{std::move(dataset)};
}

}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
