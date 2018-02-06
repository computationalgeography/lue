#include "lue/constant_collection/time/located/time_domain.hpp"


namespace lue {
namespace constant_collection {
namespace time {
namespace located {

TimeDomain::TimeDomain(
    hdf5::Group const& parent)

    : lue::TimeDomain{parent}

{
}


TimeDomain::TimeDomain(
    lue::TimeDomain&& domain)

    : lue::TimeDomain{std::forward<lue::TimeDomain>(domain)}

{
}


TimeDomain create_time_domain(
    hdf5::Group& parent,
    Clock const& clock,
    TimeConfiguration const& configuration)
{
    auto domain = lue::create_time_domain(parent, clock, configuration);

    return std::move(domain);
}

}  // namespace located
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
