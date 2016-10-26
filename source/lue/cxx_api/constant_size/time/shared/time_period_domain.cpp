#include "lue/cxx_api/constant_size/time/shared/time_period_domain.h"


namespace lue {
namespace time {
namespace shared {

TimePeriodDomain::TimePeriodDomain(
    lue::TimeDomain& group)

    : TimeDomain(group),
      _periods(group.id())

{
}


TimePeriodDomain::TimePeriodDomain(
    lue::TimeDomain& group,
    hid_t const type_id)

    : TimeDomain(group),
      _periods(group.id(), type_id)

{
}


TimePeriod& TimePeriodDomain::periods()
{
    return _periods;
}


TimePeriod& TimePeriodDomain::reserve_items(
    hsize_t const nr_items)
{
    _periods.reserve_items(nr_items);

    return _periods;
}


void configure_time_period_domain(
    hdf5::Identifier const& location,
    hid_t const file_type_id,
    hid_t const memory_type_id)
{
    create_time_period(location, file_type_id, memory_type_id);
}

}  // namespace shared
}  // namespace time
}  // namespace lue
