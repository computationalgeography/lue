#pragma once
#include "lue/core/time/unit.hpp"
#include <string>


namespace lue::data_model::time {

    template<Unit unit>
    struct UnitTraits
    {
            static auto name() -> std::string;
    };


#define UNIT_TRAITS(unit, name_)                                                                             \
    template<>                                                                                               \
    struct UnitTraits<unit>                                                                                  \
    {                                                                                                        \
                                                                                                             \
            inline static auto name() -> std::string                                                         \
            {                                                                                                \
                return #name_;                                                                               \
            }                                                                                                \
    };

    UNIT_TRAITS(Unit::nanosecond, nanosecond)
    UNIT_TRAITS(Unit::microsecond, microsecond)
    UNIT_TRAITS(Unit::millisecond, millisecond)
    UNIT_TRAITS(Unit::second, second)
    UNIT_TRAITS(Unit::minute, minute)
    UNIT_TRAITS(Unit::hour, hour)
    UNIT_TRAITS(Unit::day, day)
    UNIT_TRAITS(Unit::week, week)
    UNIT_TRAITS(Unit::month, month)
    UNIT_TRAITS(Unit::year, year)

#undef UNIT_TRAITS

}  // namespace lue::data_model::time
