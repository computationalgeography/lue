#include "lue/time_unit_util.hpp"
#include "lue/enum_string_bimap.hpp"
#include "lue/time/unit_traits.hpp"


namespace lue {
namespace {

detail::EnumStringBimap<time::Unit> const
        unit_map = {
    { time::Unit::second, "second" },
    { time::Unit::minute, "minute" },
    { time::Unit::hour, "hour" },
    { time::Unit::day, "day" },
    { time::Unit::week, "week" },
    { time::Unit::month, "month" },
    { time::Unit::year, "year" }
};

}  // Anonymous namespace


#define UNIT_NAME_CASE(                       \
    unit)                                     \
case unit: {                                  \
    result = time::UnitTraits<unit>::name();  \
    break;                                    \
}

std::string unit_to_string(
    time::Unit const unit)
{
    std::string result;

    switch(unit) {
        UNIT_NAME_CASE(time::Unit::second)
        UNIT_NAME_CASE(time::Unit::minute)
        UNIT_NAME_CASE(time::Unit::hour)
        UNIT_NAME_CASE(time::Unit::day)
        UNIT_NAME_CASE(time::Unit::week)
        UNIT_NAME_CASE(time::Unit::month)
        UNIT_NAME_CASE(time::Unit::year)
    }

    assert(!result.empty());

    return result;
}

#undef UNIT_NAME_CASE


time::Unit string_to_unit(
    std::string const& string)
{
    if(!unit_map.contains(string)) {
        throw std::runtime_error("Unknown time unit: " + string);
    }

    return unit_map.as_value(string);
}

}  // namespace lue
