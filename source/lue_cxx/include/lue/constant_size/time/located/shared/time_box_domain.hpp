#pragma once
#include "lue/constant_size/time/located/shared/property_set.hpp"
#include "lue/constant_size/time/located/shared/time_box.hpp"
#include "lue/constant_size/time/located/shared/time_domain.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {

class TimeBoxDomain:
    public TimeDomain
{

public:

    // static hdf5::Datatype file_datatype(hdf5::Identifier const& id);

                   TimeBoxDomain       (TimeDomain const& time_domain);

                   // TimeBoxDomain       (TimeDomain const& time_domain,
                   //                      hdf5::Datatype const& memory_datatype);

                   TimeBoxDomain       (TimeDomain&& time_domain);
                                        // hdf5::Datatype const& memory_datatype);

                   TimeBoxDomain       (TimeBoxDomain const& other)=delete;

                   TimeBoxDomain       (TimeBoxDomain&& other)=default;

                   ~TimeBoxDomain      ()=default;

    TimeBoxDomain& operator=           (TimeBoxDomain const& other)=delete;

    TimeBoxDomain& operator=           (TimeBoxDomain&& other)=default;

    TimeBox const& items               () const;

    TimeBox&       items               ();

    TimeBox&       reserve             (hsize_t const nr_items);

private:

    TimeBox        _items;

};


TimeBoxDomain      create_time_box_domain(
                                        PropertySet& property_set,
                                        Clock const& clock);

}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
