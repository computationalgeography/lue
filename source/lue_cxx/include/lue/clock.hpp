#pragma once
#include "lue/time.hpp"
#include "lue/hdf5/datatype.hpp"


namespace lue {

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

// private:

    char*          _unit_name;

    time::TickPeriodCount _nr_units;

};


// template<
//     typename Unit>
// inline Clock::Clock(
//     Unit const unit,
//     time::TickPeriodCount const nr_units)
// 
//     : _unit_name{time::UnitTypeTraits<Unit>::name()},
//       _nr_units{nr_units}
// 
// {
// }


hdf5::Datatype     create_clock_memory_datatype();

hdf5::Datatype     create_clock_file_datatype();

}  // namespace lue
