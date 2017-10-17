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

UNIT_TRAITS(Unit::second, s)
UNIT_TRAITS(Unit::minute, m)
UNIT_TRAITS(Unit::hour, h)
UNIT_TRAITS(Unit::day, d)
UNIT_TRAITS(Unit::week, w)
UNIT_TRAITS(Unit::month, m)
UNIT_TRAITS(Unit::year, y)

#undef UNIT_TRAITS


}  // namespace time
}  // namespace lue
