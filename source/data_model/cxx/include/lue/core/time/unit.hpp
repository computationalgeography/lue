#pragma once
#include <cstdint>


namespace lue::data_model::time {

    /*!
        @brief      Class representing the unit for representing seconds
    */
    class Second
    {
    };

    class Minute
    {
    };

    class Day
    {
    };

    class Week
    {
    };

    class Month
    {
    };

    class Year
    {
    };


    enum class Unit : std::uint8_t {

        nanosecond,

        microsecond,

        millisecond,

        second,

        minute,

        hour,

        day,

        week,

        month,

        year

    };

}  // namespace lue::data_model::time
