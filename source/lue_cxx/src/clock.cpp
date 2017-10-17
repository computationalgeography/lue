#include "lue/clock.hpp"
#include "lue/hdf5/datatype_traits.hpp"
#include "lue/tag.hpp"
#include <cassert>
#include <cstring>
#include <type_traits>


namespace lue {
namespace {




}  // Anonymous namespace


Clock::Clock()

    : _unit_name{nullptr},
      _nr_units{0}

{
}


template<
    time::Unit unit>
char* create_unit_name()
{
    auto const name = time::UnitTraits<unit>::name();
    assert(!name.empty());
    char* result = static_cast<char*>(std::malloc(name.size() + 1));
    assert(result);
    std::strcpy(result, name.c_str());
    assert(std::strlen(result) == name.size());

    return result;
}


#define CREATE_UNIT_NAME_CASE(              \
    unit)                                   \
case unit: {                                \
    _unit_name = create_unit_name<unit>();  \
    break;                                  \
}


Clock::Clock(
    time::Unit const unit,
    time::TickPeriodCount const nr_units)

    : _unit_name{nullptr},
      _nr_units{nr_units}

{
    switch(unit) {
        CREATE_UNIT_NAME_CASE(time::Unit::second)
        CREATE_UNIT_NAME_CASE(time::Unit::minute)
        CREATE_UNIT_NAME_CASE(time::Unit::hour)
        CREATE_UNIT_NAME_CASE(time::Unit::day)
        CREATE_UNIT_NAME_CASE(time::Unit::week)
        CREATE_UNIT_NAME_CASE(time::Unit::month)
        CREATE_UNIT_NAME_CASE(time::Unit::year)
    }

    assert(_unit_name);
}


Clock::~Clock()
{
    std::free(_unit_name);
}


hdf5::Datatype create_clock_memory_datatype()
{
    // Otherwise HOFFSET (offsetoff) doesn't work
    static_assert(std::is_standard_layout<Clock>::value, "");

    hdf5::Datatype const string_datatype = hdf5::create_string_datatype();
    hdf5::Datatype const count_datatype =
        hdf5::NativeDatatypeTraits<time::TickPeriodCount>::type_id();
    std::size_t const nr_bytes = sizeof(Clock);
    auto datatype = hdf5::create_compound_datatype(nr_bytes);

    std::size_t offset = HOFFSET(Clock, _unit_name);
    datatype.insert(
        unit_tag,
        offset,
        string_datatype);

    offset = HOFFSET(Clock, _nr_units);
    datatype.insert(
        nr_units_tag,
        offset,
        count_datatype);

    return datatype;
}


hdf5::Datatype create_clock_file_datatype()
{
    hdf5::Datatype const string_datatype = hdf5::create_string_datatype();
    hdf5::Datatype const count_datatype =
        hdf5::StandardDatatypeTraits<time::TickPeriodCount>::type_id();

    assert(
        count_datatype.size() ==
        hdf5::StandardDatatypeTraits<time::TickPeriodCount>::nr_bytes());

    auto const size_of_unit = sizeof(hvl_t);
    auto const size_of_nr_units = count_datatype.size();

    std::size_t const nr_bytes =
        size_of_unit +
        size_of_nr_units
        ;

    auto datatype = hdf5::create_compound_datatype(nr_bytes);

    std::size_t offset = 0;
    datatype.insert(
        unit_tag,
        offset,
        string_datatype);

    offset += size_of_unit;
    datatype.insert(
        nr_units_tag,
        offset,
        count_datatype);

    return datatype;
}

}  // namespace lue
