#include "lue/framework/benchmark/stopwatch.hpp"


namespace lue {
namespace benchmark {

/*!
    @brief      Default construct an instance

    The start and stop time points will be initialized to the high
    resolution clock's epoch.
*/
Stopwatch::Stopwatch()
:
    _time_interval{}
{
}


TimeInterval const& Stopwatch::time_interval() const
{
    return _time_interval;
}


Stopwatch::Duration Stopwatch::elapsed() const
{
    return time_interval().duration();
}

}  // namespace benchmark
}  // namespace lue
