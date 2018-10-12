#include "lue/framework/benchmark/time_interval.hpp"
#include <cassert>


namespace lue {
namespace benchmark {

/*!
    @brief      Default construct an instance

    The start and stop time points will be initialized to the high
    resolution clock's epoch.
*/
TimeInterval::TimeInterval()
:
    _start{},
    _stop{}
{
}


/*!
    @brief      Construct an instance, based on @a start and @a stop
                time points
    @warning    The condition @a start <= @a stop must be satisfied
*/
TimeInterval::TimeInterval(
    TimePoint const& start,
    TimePoint const& stop)
:
    _start{start},
    _stop{stop}
{
    assert(start <= stop);
}


/*!
    @brief      Return the time point of the start of the interval
*/
TimeInterval::TimePoint const& TimeInterval::start() const
{
    return _start;
}


/*!
    @brief      Return the time point of the end of the interval
*/
TimeInterval::TimePoint const& TimeInterval::stop() const
{
    return _stop;
}


/*!
    @brief      Return the duration of the interval
*/
TimeInterval::Duration TimeInterval::duration() const
{
    return _stop - _start;
}

}  // namespace benchmark
}  // namespace lue
