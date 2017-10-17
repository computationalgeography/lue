#pragma once
#include "lue/time/unit.hpp"
#include <string>


namespace lue {
namespace time {

template<
    Unit unit>
struct UnitTraits
{

    static std::string name            ();

};


#define UNIT_TRAITS(                               \
    unit,                                          \
    name_)                                         \
template<>                                         \
struct UnitTraits<unit>                            \
{                                                  \
                                                   \
    static std::string name() { return #name_; };  \
                                                   \
};

UNIT_TRAITS(Unit::second, second)
UNIT_TRAITS(Unit::minute, minute)
UNIT_TRAITS(Unit::hour, hour)
UNIT_TRAITS(Unit::day, day)
UNIT_TRAITS(Unit::week, week)
UNIT_TRAITS(Unit::month, month)
UNIT_TRAITS(Unit::year, year)

#undef UNIT_TRAITS


}  // namespace time
}  // namespace lue
