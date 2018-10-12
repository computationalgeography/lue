#pragma once
#include <chrono>
#include "lue/framework/benchmark/time_interval.hpp"


namespace lue {
namespace benchmark {

/*!
    @brief      Class for timing things
*/
class Stopwatch
{

public:

    using TimePoint = TimeInterval::TimePoint;
    using Duration = TimeInterval::Duration;

    static TimePoint now               ();

                   Stopwatch           ();

                   Stopwatch           (Stopwatch const&)=default;

                   Stopwatch           (Stopwatch&&)=default;

                   ~Stopwatch          ()=default;

    Stopwatch&     operator=           (Stopwatch const&)=default;

    Stopwatch&     operator=           (Stopwatch&&)=default;

    void           start               ();

    void           stop                ();

    Duration       elapsed             () const;

    template<
        typename ToDuration>
    ToDuration     elapsed             () const;

private:

    TimePoint      _start;

    TimePoint      _stop;

};


/*!
    @brief      Return the duration casted to @a ToDuration

    Use this function to convert the high resolution duration to some
    courser unit, like seconds.
*/
template<
    typename ToDuration>
inline ToDuration Stopwatch::elapsed() const
{
    return std::chrono::duration_cast<ToDuration>(elapsed());
}

}  // namespace benchmark
}  // namespace lue
