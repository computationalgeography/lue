#include "lue/constant_size/time/located/shared/time_domain.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {

TimeDomain::TimeDomain(
    hdf5::Group const& parent)

    : located::TimeDomain{parent}

{
}


TimeDomain::TimeDomain(
    located::TimeDomain&& domain)

    : located::TimeDomain{
          std::forward<located::TimeDomain>(domain)}

{
}


TimeDomain create_time_domain(
    hdf5::Group const& parent,
    TimeDomain::Configuration const& configuration)
{
    auto domain = located::create_time_domain(parent, configuration);

    return std::move(domain);
}

}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
