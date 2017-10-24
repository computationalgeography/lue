#pragma once


namespace lue {
namespace time {

/*!
    @brief      Class representing the unit for representing seconds
*/
class Second{};

class Minute{};
class Day{};
class Week{};
class Month{};
class Year{};


enum class Unit
{

    second,

    minute,

    hour,

    day,

    week,

    month,

    year

};

}  // namespace time
}  // namespace lue
