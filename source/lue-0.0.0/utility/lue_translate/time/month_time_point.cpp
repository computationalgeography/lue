#include "lue/utility/lue_translate/time/month_time_point.h"


namespace lue {

MonthTimePoint::MonthTimePoint()

    : _year{},
      _month{}

{
}


MonthTimePoint::MonthTimePoint(
    Year const& year,
    Month const& month)

    : _year{year},
      _month{month}

{
}


MonthTimePoint& MonthTimePoint::operator+=(
    MonthDuration const& duration)
{
    _month += duration.count();

    // TODO
    while(_month > 11) {
        ++_year;
        _month -= 11;
    }

    return *this;
}


MonthTimePoint::Year MonthTimePoint::year() const
{
    return _year;
}


MonthTimePoint::Month MonthTimePoint::month() const
{
    return _month;
}

}  // namespace lue
