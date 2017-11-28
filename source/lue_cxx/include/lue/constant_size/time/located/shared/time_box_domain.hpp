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

                   TimeBoxDomain       (TimeDomain&& time_domain);

                   TimeBoxDomain       (TimeBoxDomain const&)=delete;

                   TimeBoxDomain       (TimeBoxDomain&&)=default;

                   ~TimeBoxDomain      ()=default;

    TimeBoxDomain& operator=           (TimeBoxDomain const&)=delete;

    TimeBoxDomain& operator=           (TimeBoxDomain&&)=default;

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
