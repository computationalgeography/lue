#pragma once


namespace lue {
    namespace data_model {
        namespace time {

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


            enum class Unit {

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

        }  // namespace time
    }  // namespace data_model
}  // namespace lue
