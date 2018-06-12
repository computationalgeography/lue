#include "lue/time/synchronous_time_box.hpp"


namespace lue {

SynchronousTimeBox::SynchronousTimeBox(
    hdf5::Group& parent)

    : SynchronousTimeDomainItem{parent}

{
}


SynchronousTimeBox create_synchronous_time_box(
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

    create_synchronous_time_domain_item(
        parent, file_datatype, memory_datatype, value_shape);

    return SynchronousTimeBox{parent};
}

}  // namespace lue
