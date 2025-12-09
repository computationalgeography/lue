#pragma once
#include <cstdint>


namespace lue::data_model::time {

    using TickPeriodCount = uint64_t;


    /*!
        @brief      Class for representing the length of a tick period
        @tparam     Unit Unit of time

        A TickPeriod instance is represented by a number of time units. This determines the resolution of a
        Duration, Clock and of the TimePoint s with which locations in time can be represented.

        Examples:

        | Code                   | Meaning    |
        | ---------------------- | ---------- |
        | TickPeriod<Second>{10} | 10 seconds |
        | TickPeriod<Day>{1}     | 1 day      |
        | TickPeriod<Week>{4}    | 4 weeks    |
        | TickPeriod<Year>{100}  | 100 years  |
    */
    template<typename Unit>
    class TickPeriod
    {

        public:

            using Count = TickPeriodCount;

            TickPeriod();

            explicit TickPeriod(Count nr_units);

            TickPeriod(TickPeriod const&) = default;

            TickPeriod(TickPeriod&&) noexcept = default;

            ~TickPeriod() = default;

            auto operator=(TickPeriod const&) -> TickPeriod& = default;

            auto operator=(TickPeriod&&) noexcept -> TickPeriod& = default;

            auto nr_units() const -> Count;

        private:

            //! Number of Unit units per tick
            Count _nr_units;
    };


    template<typename Unit>
    inline TickPeriod<Unit>::TickPeriod():

        _nr_units{1}

    {
    }


    template<typename Unit>
    inline TickPeriod<Unit>::TickPeriod(Count const nr_units):

        _nr_units{nr_units}

    {
    }


    template<typename Unit>
    inline auto TickPeriod<Unit>::nr_units() const -> typename TickPeriod<Unit>::Count
    {
        return _nr_units;
    }

}  // namespace lue::data_model::time
