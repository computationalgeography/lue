#pragma once
#include "lue/utility/lue_translate/time/month_duration.h"


namespace lue {

class MonthTimePoint
{

public:

    using Year = int;
    using Month = int;

                   MonthTimePoint      ();

                   MonthTimePoint      (Year const& year,
                                        Month const& month);

    MonthTimePoint& operator+=         (MonthDuration const& duration);

    Year           year                () const;

    Month          month               () const;

private:

    Year           _year;

    Month          _month;

};

}  // namespace lue
