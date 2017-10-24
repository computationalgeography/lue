#include "lue/clock.hpp"


namespace lue {

Clock::Clock(
    time::Unit const unit,
    time::TickPeriodCount const nr_units)

    : _unit{unit},
      _nr_units{nr_units}

{
}


time::Unit Clock::unit() const
{
    return _unit;
}


time::TickPeriodCount Clock::nr_units() const
{
    return _nr_units;
}

}  // namespace lue
