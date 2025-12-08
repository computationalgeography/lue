#pragma once


namespace lue::data_model::time {

    /*!
        @brief      Class for representing a location along the time dimension
        @tparam     Clock Class for representing all locations along a time dimension since an epoch

        With a clock a period of time since an epoch can be represented, including the resolution with which
        locations can be represented. A TimePoint can be used to represent one of these locations.

        Examples:

        @code
        // Types for tracking days
        using TickPeriod = TickPeriod<Day>;
        using Clock = Clock<TickPeriod>;
        using Duration = Clock::Duration;
        using TimePoint = TimePoint<Clock>;

        // Clock with a resolution of 2 days
        TickPeriod tick_period{2};
        Clock clock(tick_period);

        // Select day 6 (3 * 2 days)
        Duration duration{3};
        TimePoint day_6{duration};

        assert(clock.nr_units(day_6), 6);
        @endcode
    */
    template<typename Clock>
    class TimePoint
    {

        public:

            using Duration = typename Clock::Duration;

            explicit TimePoint(Duration const& duration);

            TimePoint(TimePoint const& other) = default;

            TimePoint(TimePoint&& other) noexcept = default;

            ~TimePoint() = default;

            auto operator=(TimePoint const& other) -> TimePoint& = default;

            auto operator=(TimePoint&& other) noexcept -> TimePoint& = default;

            auto duration() const -> Duration const&;

            auto nr_ticks() const -> typename Duration::Count;

        private:

            //! Duration since epoch
            Duration _duration;
    };


    /*!
        @brief      Construct an instance based on a duration
        @param      duration Duration since the Clock's epoch
    */
    template<typename Clock>
    inline TimePoint<Clock>::TimePoint(Duration const& duration):

        _duration{duration}

    {
    }


    /*!
        @brief      Return duration
    */
    template<typename Clock>
    inline auto TimePoint<Clock>::duration() const -> typename Clock::Duration const&
    {
        return _duration;
    }


    /*!
        @brief      Return number of clock ticks since the Clock's epoch
    */
    template<typename Clock>
    inline auto TimePoint<Clock>::nr_ticks() const -> typename Clock::Duration::Count
    {
        return _duration.nr_ticks();
    }

}  // namespace lue::data_model::time
