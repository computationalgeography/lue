#pragma once
#include "lue/time/duration.hpp"


namespace lue {
namespace time {

/*!
    @brief      Class for representing a period of time since an epoch
    @tparam     TickPeriod Type for representing the length of a tick period

    The epoch is undefined for now.
*/
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

    template<
        template<typename> class TimePoint>
    typename TickPeriod::Count
                   nr_units            (TimePoint<Clock> const& time_point);

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


/*!
    @brief      Return the time point's number of units since the Clock's epoch
    @tparam     TimePoint Class template for representing time points
                in this clock
*/
template<
    typename TickPeriod>
template<
    template<typename> class TimePoint>
inline typename TickPeriod::Count Clock<TickPeriod>::nr_units(
    TimePoint<Clock> const& time_point)
{
    return time_point.nr_ticks() * _tick_period.nr_units();
}

}  // namespace time
}  // namespace lue
