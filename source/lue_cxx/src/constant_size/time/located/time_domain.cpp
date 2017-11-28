#include "lue/constant_size/time/located/time_domain.hpp"


namespace lue {
namespace constant_size {
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
    hdf5::Group const& parent,
    TimeDomain::Configuration const& configuration)
{
    auto domain = lue::create_time_domain(parent, configuration);

    return std::move(domain);
}

}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
