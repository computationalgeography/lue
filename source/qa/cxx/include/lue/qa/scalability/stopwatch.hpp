#pragma once
#include "lue/qa/scalability/time_interval.hpp"


namespace lue::qa {

    /*!
        @brief      Class for timing things
        @sa         TimeInterval
    */
    class Stopwatch
    {

        public:

            using Clock = TimeInterval::Clock;

            using TimePoint = TimeInterval::TimePoint;

            using Duration = TimeInterval::Duration;

            using SystemClock = std::chrono::system_clock;

            using SystemTimePoint = SystemClock::time_point;

            static auto now() -> TimePoint;

            Stopwatch() = default;

            Stopwatch(Stopwatch const&) = default;

            Stopwatch(Stopwatch&&) = default;

            ~Stopwatch() = default;

            auto operator=(Stopwatch const&) -> Stopwatch& = default;

            auto operator=(Stopwatch&&) -> Stopwatch& = default;

            void start();

            void stop();

            [[nodiscard]] auto start_time_point() const -> SystemTimePoint const&;

            /*!
                @brief      Return the duration casted to @a ToDuration

                Use this function to convert the high resolution duration to some
                courser unit, like seconds.
            */
            template<typename ToDuration>
            [[nodiscard]] auto duration() const -> ToDuration
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
