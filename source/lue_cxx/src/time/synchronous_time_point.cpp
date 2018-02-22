#include "lue/time/synchronous_time_point.hpp"


namespace lue {

SynchronousTimePoint::SynchronousTimePoint(
    hdf5::Group& parent)

    : SynchronousTimeDomainItem{parent}

{
}


SynchronousTimePoint create_synchronous_time_point(
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

    create_synchronous_time_domain_item(
        parent, file_datatype, memory_datatype, value_shape);

    return SynchronousTimePoint{parent};
}

}  // namespace lue
