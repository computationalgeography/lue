#pragma once
#include "lue/core/time.hpp"


namespace lue {
namespace data_model {

class Clock
{

public:

                   Clock               (time::Unit unit,
                                        time::TickPeriodCount nr_units);

                   Clock               (time::Epoch const& epoch,
                                        time::Unit unit,
                                        time::TickPeriodCount nr_units);

                   Clock               (Clock const&)=default;

                   Clock               (Clock&&)=default;

                   ~Clock              ()=default;

    Clock&         operator=           (Clock const&)=default;

    Clock&         operator=           (Clock&&)=default;

    bool           operator==          (Clock const& other) const;

    bool           operator!=          (Clock const& other) const;

    bool           operator<           (Clock const& other) const;

    time::Epoch const& epoch           () const;

    time::Unit     unit                () const;

    time::TickPeriodCount nr_units     () const;

private:

    time::Epoch    _epoch;

    time::Unit     _unit;

    time::TickPeriodCount _nr_units;

};

}  // namespace data_model
}  // namespace lue
