#pragma once
#include "lue/qa/scalability/time_interval.hpp"


namespace lue::qa {

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

            static TimePoint now();

            Stopwatch() = default;

            Stopwatch(Stopwatch const&) = default;

            Stopwatch(Stopwatch&&) = default;

            ~Stopwatch() = default;

            Stopwatch& operator=(Stopwatch const&) = default;

            Stopwatch& operator=(Stopwatch&&) = default;

            void start();

            void stop();

            SystemTimePoint const& start_time_point() const;

            /*!
                @brief      Return the duration casted to @a ToDuration

                Use this function to convert the high resolution duration to some
                courser unit, like seconds.
            */
            template<typename ToDuration>
            ToDuration duration() const
            {
                return _time_interval.duration<ToDuration>();
            }

        private:

            //! Time point according to the system clock at start of measurement
            SystemTimePoint _start;

            //! Amount of time after a start and stop of the stopwatch
            TimeInterval _time_interval;
    };

}  // namespace lue::qa
