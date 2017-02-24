#pragma once
#include "lue/cxx_api/constant_size/time/shared/time_domain.h"
#include "lue/cxx_api/constant_size/time/shared/time_period.h"


namespace lue {
namespace constant_size {
namespace time {
namespace shared {

/*!
    @ingroup    lue_cxx_api_group
*/
class TimePeriodDomain:
    public TimeDomain
{

public:

                   TimePeriodDomain    (lue::TimeDomain& group);

                   TimePeriodDomain    (lue::TimeDomain& group,
                                        hid_t const type_id);

                   TimePeriodDomain    (TimePeriodDomain const& other)=delete;

                   TimePeriodDomain    (TimePeriodDomain&& other)=default;

                   ~TimePeriodDomain   ()=default;

    TimePeriodDomain& operator=        (TimePeriodDomain const& other)=delete;

    TimePeriodDomain& operator=        (TimePeriodDomain&& other)=default;

    TimePeriod&    periods             ();

    TimePeriod&    reserve_items       (hsize_t const nr_items);

private:

    TimePeriod     _periods;

};


void               configure_time_period_domain(
                                        hdf5::Identifier const& location,
                                        hid_t const file_type_id,
                                        hid_t const memory_type_id);

}  // namespace shared
}  // namespace time
}  // namespace constant_size
}  // namespace lue
