#pragma once
#include <utility>

#include "lue/core/time/duration.hpp"
#include "lue/core/time/epoch.hpp"


namespace lue::data_model::time {

    /*!
        @brief      Class for representing a period of time since an epoch
        @tparam     TickPeriod Type for representing the length of a tick period
    */
    template<typename TickPeriod>
    class Clock
    {

        public:

            using Duration = time::Duration<TickPeriod>;

            Clock() = default;

            Clock(Epoch epoch, TickPeriod const& tick_period);

            Clock(Clock const& other) = default;

            Clock(Clock&& other) noexcept = default;

            ~Clock() = default;

            auto operator=(Clock const& other) -> Clock& = default;

            auto operator=(Clock&& other) noexcept -> Clock& = default;

            auto epoch() const -> Epoch const&;

            auto tick_period() const -> TickPeriod const&;

            template<template<typename> class TimePoint>
            auto nr_units(TimePoint<Clock> const& time_point) -> typename TickPeriod::Count;

        private:

            Epoch _epoch;

            TickPeriod _tick_period;
    };


    template<typename TickPeriod>
    inline Clock<TickPeriod>::Clock(Epoch epoch, TickPeriod const& tick_period):

        _epoch{std::move(epoch)},
        _tick_period{tick_period}

    {
    }


    template<typename TickPeriod>
    inline auto Clock<TickPeriod>::epoch() const -> Epoch const&
    {
        return _epoch;
    }


    template<typename TickPeriod>
    inline auto Clock<TickPeriod>::tick_period() const -> TickPeriod const&
    {
        return _tick_period;
    }


    /*!
        @brief      Return the time point's number of units since the
                    Clock's epoch
        @tparam     TimePoint Class template for representing time points
                    in this clock
    */
    template<typename TickPeriod>
    template<template<typename> class TimePoint>
    inline auto Clock<TickPeriod>::nr_units(TimePoint<Clock> const& time_point) -> typename TickPeriod::Count
    {
        return time_point.nr_ticks() * _tick_period.nr_units();
    }

}  // namespace lue::data_model::time
