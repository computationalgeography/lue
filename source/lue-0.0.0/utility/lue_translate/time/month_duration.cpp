#include "lue/utility/lue_translate/time/month_duration.h"


namespace lue {

MonthDuration::MonthDuration()

    : _count{}

{
}


MonthDuration::MonthDuration(
    Count const count)

    : _count{count}

{
}


MonthDuration& MonthDuration::operator*=(
    size_t const multiplier)
{
    _count *= multiplier;
    return *this;
}


MonthDuration::Count MonthDuration::count() const
{
    return _count;
}


MonthDuration operator*(
    MonthDuration const& duration,
    size_t const multiplier)
{
    MonthDuration new_duration{duration};
    new_duration *= multiplier;
    return new_duration;
}


MonthDuration operator*(
    size_t const multiplier,
    MonthDuration const& duration)
{
    MonthDuration new_duration{duration};
    new_duration *= multiplier;
    return new_duration;
}

}  // namespace lue
