#include "lue/info/time/time_cell.hpp"
#include "lue/core/tag.hpp"


namespace lue {

TimeCell::TimeCell(
    hdf5::Group& parent):

    TimeBox{parent},
    _count{
        parent, time_discretization_tag,
        hdf5::Datatype{hdf5::NativeDatatypeTraits<lue::Count>::type_id()}}

{
}


TimeCell::TimeCell(
    TimeBox&& time_box,
    TimeCell::Count&& count):

    TimeBox{std::forward<TimeBox>(time_box)},
    _count{std::forward<TimeCell::Count>(count)}

{
}


lue::Count TimeCell::nr_counts() const
{
    return _count.nr_arrays();
}


TimeCell::Count const& TimeCell::count() const
{
    return _count;
}


TimeCell::Count& TimeCell::count()
{
    return _count;
}


TimeCell create_time_cell(
    hdf5::Group& parent,
    lue::Clock const& clock)
{
    // A time cell is defined by a time box and a count per box.
    auto time_box = create_time_box(parent, clock);

    hdf5::Datatype memory_datatype{
        hdf5::NativeDatatypeTraits<lue::Count>::type_id()};

    auto count = same_shape::constant_shape::create_value(
        parent, time_discretization_tag, memory_datatype);

    return TimeCell{std::move(time_box), std::move(count)};
}

}  // namespace lue
