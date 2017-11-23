#pragma once
#include "lue/time.hpp"


namespace lue {

class Clock
{

public:

                   Clock               (time::Unit const unit,
                                        time::TickPeriodCount const nr_units);

    time::Unit     unit                () const;

    time::TickPeriodCount nr_units     () const;

private:

    time::Unit _unit;

    time::TickPeriodCount _nr_units;

};


bool               operator==          (Clock const& lhs,
                                        Clock const& rhs);

bool               operator<           (Clock const& lhs,
                                        Clock const& rhs);

}  // namespace lue
