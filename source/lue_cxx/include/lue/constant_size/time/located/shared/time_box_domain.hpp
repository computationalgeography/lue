#pragma once
#include "lue/item/constant_size/constant/same_shape/collection.hpp"
#include "lue/constant_size/time/located/shared/property_set.hpp"
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

    using TimeBoxes = constant::same_shape::Collection;

                   TimeBoxDomain       (TimeDomain const& time_domain);

                   TimeBoxDomain       (TimeDomain&& time_domain);

                   TimeBoxDomain       (TimeBoxDomain const&)=delete;

                   TimeBoxDomain       (TimeBoxDomain&&)=default;

                   ~TimeBoxDomain      ()=default;

    TimeBoxDomain& operator=           (TimeBoxDomain const&)=delete;

    TimeBoxDomain& operator=           (TimeBoxDomain&&)=default;

    TimeBoxes const& items             () const;

    TimeBoxes&     items               ();

    TimeBoxes&     reserve             (hsize_t nr_items);

private:

    TimeBoxes      _items;

};


TimeBoxDomain      create_time_box_domain(
                                        PropertySet& property_set,
                                        Clock const& clock);

}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
