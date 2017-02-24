#include "lue/utility/lue_utility/stopwatch.h"
#include <cassert>
#include <ctime>


namespace lue {

Stopwatch::Stopwatch()
{
}


void Stopwatch::start()
{
    _start = std::chrono::system_clock::now();
    _end = _start;

    assert(_start == _end);
}


void Stopwatch::stop()
{
    _end = std::chrono::system_clock::now();

    assert(_start <= _end);
}


Stopwatch::TimePoint const& Stopwatch::start() const
{
    return _start;
}


Stopwatch::TimePoint const& Stopwatch::end() const
{
    return _end;
}


double Stopwatch::elapsed_seconds() const
{
    std::chrono::duration<double> elapsed_seconds = _end - _start;

    return elapsed_seconds.count();
}


std::string to_string(
    Stopwatch::TimePoint const& time_point)
{
    std::time_t const time = std::chrono::system_clock::to_time_t(time_point);
    std::string string = std::ctime(&time);

    // Remove trailing '\n', which ctime adds to the end...
    if(!string.empty()) {
        assert(string.back() == '\n');
        string.pop_back();
    }

    return string;
}

}  // namespace lue
