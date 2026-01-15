#include "lue/core/clock.hpp"


namespace lue::data_model {

    Clock::Clock(time::Unit const unit, time::TickPeriodCount const nr_units):

        Clock{time::Epoch{}, unit, nr_units}

    {
    }


    Clock::Clock(time::Epoch epoch, time::Unit const unit, time::TickPeriodCount const nr_units):

        _epoch{std::move(epoch)},
        _unit{unit},
        _nr_units{nr_units}

    {
    }


    auto Clock::epoch() const -> time::Epoch const&
    {
        return _epoch;
    }


    auto Clock::unit() const -> time::Unit
    {
        return _unit;
    }


    auto Clock::nr_units() const -> time::TickPeriodCount
    {
        return _nr_units;
    }


    auto Clock::operator==(Clock const& other) const -> bool
    {
        return _unit == other._unit && _nr_units == other._nr_units;
    }


    auto Clock::operator!=(Clock const& other) const -> bool
    {
        return !(*this == other);
    }


    auto Clock::operator<(Clock const& other) const -> bool
    {
        return _unit < other._unit || _nr_units < other._nr_units;
    }

}  // namespace lue::data_model
