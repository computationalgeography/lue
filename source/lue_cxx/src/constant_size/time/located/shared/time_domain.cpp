#include "lue/constant_size/time/located/shared/time_domain.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {

TimeDomain::TimeDomain(
    hdf5::Group const& group)

    : located::TimeDomain{group.id()}

{
}


TimeDomain::TimeDomain(
    located::TimeDomain&& domain)

    : located::TimeDomain{
          std::forward<located::TimeDomain>(domain)}

{
}


TimeDomain create_time_domain(
    hdf5::Group const& group,
    TimeDomain::Configuration const& configuration)
{
    auto domain = located::create_time_domain(
        group, configuration);

    return std::move(domain);
}

}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
