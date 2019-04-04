#include "lue/core/clock.hpp"


namespace lue {

Clock::Clock(
    time::Unit const unit,
    time::TickPeriodCount const nr_units):

    Clock{time::Epoch{}, unit, nr_units}

{
}


Clock::Clock(
    time::Epoch const& epoch,
    time::Unit const unit,
    time::TickPeriodCount const nr_units):

    _epoch{epoch},
    _unit{unit},
    _nr_units{nr_units}

{
}


time::Epoch const& Clock::epoch() const
{
    return _epoch;
}


time::Unit Clock::unit() const
{
    return _unit;
}


time::TickPeriodCount Clock::nr_units() const
{
    return _nr_units;
}


bool Clock::operator==(
    Clock const& other) const
{
    return
        _unit == other._unit &&
        _nr_units == other._nr_units
        ;
}


bool Clock::operator!=(
    Clock const& other) const
{
    return !(*this == other);
}


bool Clock::operator<(
    Clock const& other) const
{
    return
        _unit < other._unit ||
        _nr_units < other._nr_units
        ;
}

}  // namespace lue
