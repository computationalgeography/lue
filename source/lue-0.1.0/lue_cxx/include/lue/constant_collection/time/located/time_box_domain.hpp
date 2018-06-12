#pragma once
#include "lue/item/same_shape/continuous_value.hpp"
#include "lue/constant_collection/property_set.hpp"
#include "lue/constant_collection/time/located/time_domain.hpp"


namespace lue {
namespace constant_collection {
namespace time {
namespace located {

class TimeBoxDomain:
    public TimeDomain
{

public:

    using TimeBoxes = lue::same_shape::ContinuousValue;

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

}  // namespace located
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
