#include "lue/info/time/time_cell.hpp"
#include "lue/core/tag.hpp"


namespace lue {

TimeCell::TimeCell(
    hdf5::Group& parent):

    TimeBox{parent},
    _counts{
        parent, time_discretization_tag,
        hdf5::Datatype{hdf5::NativeDatatypeTraits<Count>::type_id()}}

{
}


TimeCell::TimeCell(
    TimeBox&& time_box,
    TimeCell::Counts&& counts):

    TimeBox{std::forward<TimeBox>(time_box)},
    _counts{std::forward<TimeCell::Counts>(counts)}

{
}


Count TimeCell::nr_counts() const
{
    return _counts.nr_arrays();
}


TimeCell::Counts const& TimeCell::counts() const
{
    return _counts;
}


TimeCell::Counts& TimeCell::counts()
{
    return _counts;
}


TimeCell create_time_cell(
    hdf5::Group& parent,
    lue::Clock const& clock)
{
    // A time cell is defined by a time box and a count per box.
    auto time_box = create_time_box(parent, clock);

    hdf5::Datatype memory_datatype{
        hdf5::NativeDatatypeTraits<Count>::type_id()};

    auto counts = same_shape::constant_shape::create_value(
        parent, time_discretization_tag, memory_datatype);

    return TimeCell{std::move(time_box), std::move(counts)};
}

}  // namespace lue
