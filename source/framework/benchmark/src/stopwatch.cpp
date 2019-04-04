#include "lue/framework/benchmark/stopwatch.hpp"


namespace lue {
namespace benchmark {

/*!
    @brief      .
    @param      .
    @return     .
    @exception  .

    Unless start has been pressed on the stopwatch, the value returned
    makes no sense.
*/
Stopwatch::SystemTimePoint const& Stopwatch::start() const
{
    return _start;
}


// Stopwatch::Duration Stopwatch::duration() const
// {
//     return _time_interval.duration();
// }

}  // namespace benchmark
}  // namespace lue
