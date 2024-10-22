#pragma once
#include <chrono>


namespace lue::qa {

    /*!
        @brief      Class for representing time intervals
    */
    class TimeInterval
    {

        public:

            // We need a steady clock for measuring time intervals. The time
            // points cannot be related to wall clock time, though. But they
            // are fine for measuring intervals.
            using Clock = std::chrono::steady_clock;
            static_assert(Clock::is_steady);

            using TimePoint = Clock::time_point;

            using Duration = Clock::duration;

            TimeInterval() = default;

            TimeInterval(TimePoint const& start, TimePoint const& stop);

            TimeInterval(TimeInterval const&) = default;

            TimeInterval(TimeInterval&&) = default;

            ~TimeInterval() = default;

            auto operator=(TimeInterval const&) -> TimeInterval& = default;

            auto operator=(TimeInterval&&) -> TimeInterval& = default;

            void set_stop(TimePoint const& time_point);

            [[nodiscard]] auto duration() const -> Duration;

            /*!
                @brief      Return the duration casted to @a ToDuration

                Use this function to convert the duration to some courser unit,
                like seconds.
            */
            template<typename ToDuration>
            [[nodiscard]] auto duration() const -> ToDuration
            {
                return std::chrono::duration_cast<ToDuration>(duration());
            }

        private:

            //! Start of interval
            TimePoint _start;

            //! End of interval
            TimePoint _stop;
    };

}  // namespace lue::qa
