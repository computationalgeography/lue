#include "lue/qa/scalability/stopwatch.hpp"


namespace lue::qa {

    /*!
        @brief      Return the current value of the clock

        Note that the time point returned is relative to the clock's
        epoch. For example, this might be the number of ticks since the
        last reboot.
    */
    Stopwatch::TimePoint Stopwatch::now()
    {
        return Clock::now();
    }


    void Stopwatch::start()
    {
        _start = SystemClock::now();
        auto const now = Stopwatch::now();
        _time_interval = TimeInterval{now, now};
    }


    void Stopwatch::stop()
    {
        auto const now = Stopwatch::now();

        _time_interval.set_stop(now);
    }


    /*!
        @brief      .
        @param      .
        @return     .
        @exception  .

        Unless start has been pressed on the stopwatch, the value returned
        makes no sense.
    */
    Stopwatch::SystemTimePoint const& Stopwatch::start_time_point() const
    {
        return _start;
    }

}  // namespace lue::qa
