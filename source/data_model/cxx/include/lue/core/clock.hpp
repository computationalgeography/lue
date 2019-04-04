#pragma once
#include "lue/core/time.hpp"


namespace lue {

class Clock
{

public:

                   Clock               (time::Unit const unit,
                                        time::TickPeriodCount const nr_units);

                   Clock               (time::Epoch const& epoch,
                                        time::Unit const unit,
                                        time::TickPeriodCount const nr_units);

    bool           operator==          (Clock const& other) const;

    bool           operator!=          (Clock const& other) const;

    bool           operator<           (Clock const& other) const;

    time::Epoch const& epoch           () const;

    time::Unit     unit                () const;

    time::TickPeriodCount nr_units     () const;

private:

    time::Epoch const _epoch;

    time::Unit const _unit;

    time::TickPeriodCount const _nr_units;

};

}  // namespace lue
