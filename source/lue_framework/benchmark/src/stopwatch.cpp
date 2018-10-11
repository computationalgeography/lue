#include "lue/framework/benchmark/stopwatch.hpp"


namespace lue {
namespace benchmark {

Stopwatch::Stopwatch()
:
    _start{},
    _stop{}
{
}


void Stopwatch::start()
{
    _start = std::chrono::high_resolution_clock::now();
}


void Stopwatch::stop()
{
    _stop = std::chrono::high_resolution_clock::now();
}


Stopwatch::Duration Stopwatch::elapsed() const
{
    return _stop - _start;
}

}  // namespace benchmark
}  // namespace lue
