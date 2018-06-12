#pragma once
#include "lue/time.hpp"


namespace lue {

class Clock
{

public:

                   Clock               (time::Unit const unit,
                                        time::TickPeriodCount const nr_units);

    bool           operator==          (Clock const& other) const;

    bool           operator<           (Clock const& other) const;

    time::Unit     unit                () const;

    time::TickPeriodCount nr_units     () const;

private:

    time::Unit _unit;

    time::TickPeriodCount _nr_units;

};


}  // namespace lue
