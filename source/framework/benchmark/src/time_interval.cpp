#include "lue/framework/benchmark/time_interval.hpp"
#include "lue/assert.hpp"


namespace lue {
namespace benchmark {

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
    lue_assert(_start <= _stop);
}


void TimeInterval::set_stop(
    TimePoint const& time_point)
{
    _stop = time_point;

    lue_assert(_start <= _stop);
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
