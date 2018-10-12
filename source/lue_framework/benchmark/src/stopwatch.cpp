#include "lue/framework/benchmark/stopwatch.hpp"


namespace lue {
namespace benchmark {

/*!
    @brief      Return the current value of the high resolution clock
*/
Stopwatch::TimePoint Stopwatch::now()
{
    return std::chrono::high_resolution_clock::now();
}


/*!
    @brief      Default construct an instance

    The start and stop time points will be initialized to the high
    resolution clock's epoch.
*/
Stopwatch::Stopwatch()
:
    _start{},
    _stop{}
{
}


void Stopwatch::start()
{
    _start = now();
}


void Stopwatch::stop()
{
    _stop = now();
}


Stopwatch::Duration Stopwatch::elapsed() const
{
    return _stop - _start;
}

}  // namespace benchmark
}  // namespace lue
