#pragma once
#include <cstdint>


namespace lue {
namespace time {

using DurationCount = uint64_t;


/*!
    @brief      Class for representing an amount of time
    @tparam     TickPeriod Type for representing the length of a Clock
                tick period

    A Duration instance is represented by a number of TickPeriod units.
    Note that, without the actual number of units in a tick period, the
    number of units in a duration cannot be known. Both the tick period
    and the duration instances are needed for that. Also, a duration
    doesn't tell you anything about when something happened. It only
    tells you how long it took.

    Examples:

    | Code                             | Meaning               |
    | -------------------------------- | --------------------- |
    | Duration<TickPeriod<Second>>{5}  | 5 ticks of n seconds  |
    | Duration<TickPeriod<Day>>{1}     | 1 tick of n days      |
    | Duration<TickPeriod<Week>>{4}    | 4 ticks of n weeks    |
    | Duration<TickPeriod<Year>>{10}   | 10 ticks of n years   |
*/
template<
    typename TickPeriod>
class Duration
{

public:

    using Count = DurationCount;

                   Duration            (Count const nr_ticks);

                   Duration            (Duration const&)=default;

                   Duration            (Duration&&)=default;

                   ~Duration           ()=default;

    Duration&      operator=           (Duration const&)=default;

    Duration&      operator=           (Duration&&)=default;

    bool           operator==          (Duration const& other) const;

    Count          nr_ticks            () const;

private:

    //! Number of tick period ticks in duration
    Count          _nr_ticks;

};


template<
    typename TickPeriod>
inline Duration<TickPeriod>::Duration(
    Count const nr_ticks)

    : _nr_ticks{nr_ticks}

{
}


template<
    typename TickPeriod>
inline typename Duration<TickPeriod>::Count
    Duration<TickPeriod>::nr_ticks() const
{
    return _nr_ticks;
}


template<
    typename TickPeriod>
inline bool Duration<TickPeriod>::operator==(
    Duration const& other) const
{
    return _nr_ticks == other._nr_ticks;
}

}  // namespace time
}  // namespace lue
