#pragma once
#include "lue/time/duration.hpp"


namespace lue {
namespace time {

template<
    typename TickPeriod>
class Clock
{

public:

    using Duration = time::Duration<TickPeriod>;

                   Clock               (TickPeriod const& tick_period);

                   Clock               ()=default;

                   Clock               (Clock const&)=default;

                   Clock               (Clock&&)=default;

                   ~Clock              ()=default;

    Clock&         operator=           (Clock const&)=default;

    Clock&         operator=           (Clock&&)=default;

    TickPeriod const& tick_period      () const;

private:

    TickPeriod     _tick_period;

};


template<
    typename TickPeriod>
inline Clock<TickPeriod>::Clock(
    TickPeriod const& tick_period)

    : _tick_period{tick_period}

{
}


template<
    typename TickPeriod>
inline TickPeriod const& Clock<TickPeriod>::tick_period() const
{
    return _tick_period;
}

}  // namespace time
}  // namespace lue
