#pragma once
#include "lue/framework/benchmark/time_interval.hpp"


namespace lue {
namespace benchmark {

/*!
    @brief      Class for timing things
*/
class Stopwatch
{

public:

    using Clock = TimeInterval::Clock;

    using TimePoint = TimeInterval::TimePoint;

    using Duration = TimeInterval::Duration;

    using SystemClock = std::chrono::system_clock;

    using SystemTimePoint = SystemClock::time_point;

    static TimePoint now               ();

                   Stopwatch           ()=default;

                   Stopwatch           (Stopwatch const&)=default;

                   Stopwatch           (Stopwatch&&)=default;

                   ~Stopwatch          ()=default;

    Stopwatch&     operator=           (Stopwatch const&)=default;

    Stopwatch&     operator=           (Stopwatch&&)=default;

    void           start               ();

    void           stop                ();

    SystemTimePoint const& start       () const;

    // Duration       duration            () const;

    template<
        typename ToDuration>
    ToDuration     duration            () const;

private:

    //! Time point according to the system clock at start of measurement
    SystemTimePoint _start;

    //! Amount of time after a start and stop of the stopwatch
    TimeInterval   _time_interval;

};


/*!
    @brief      Return the current value of the clock

    Note that the time point returned is relative to the clock's
    epoch. For example, this might be the number of ticks since the
    last reboot.
*/
inline Stopwatch::TimePoint Stopwatch::now()
{
    return Clock::now();
}


inline void Stopwatch::start()
{
    _start = SystemClock::now();
    auto const now = Stopwatch::now();
    _time_interval = TimeInterval{now, now};
}


inline void Stopwatch::stop()
{
    auto const now = Stopwatch::now();
    _time_interval.set_stop(now);
}


/*!
    @brief      Return the duration casted to @a ToDuration

    Use this function to convert the high resolution duration to some
    courser unit, like seconds.
*/
template<
    typename ToDuration>
inline ToDuration Stopwatch::duration() const
{
    return std::chrono::duration_cast<ToDuration>(_time_interval.duration());
}

}  // namespace benchmark
}  // namespace lue
