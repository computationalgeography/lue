#pragma once
#include "lue/time.hpp"
#include "lue/hdf5/datatype.hpp"


namespace lue {
namespace compound {

class Clock
{

public:

                   Clock               ();

                   Clock               (time::Unit const unit,
                                        time::TickPeriodCount const nr_units);

                   Clock               (Clock const& other)=default;

                   Clock               (Clock&& other)=default;

                   ~Clock              ();

    Clock&         operator=           (Clock const& other)=default;

    Clock&         operator=           (Clock&& other)=default;

    char*          unit_name           () const;

    time::TickPeriodCount nr_units     () const;

// private:

    char*          _unit_name;

    time::TickPeriodCount _nr_units;

};


hdf5::Datatype     create_clock_memory_datatype();

hdf5::Datatype     create_clock_file_datatype();

}  // namespace compound
}  // namespace lue
