#include "lue/constant_size/time/located/shared/time_domain.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {

TimeDomain::TimeDomain(
    hdf5::Group const& group)

    : constant_size::time::located::TimeDomain(group)

{
}


TimeDomain::TimeDomain(
    constant_size::time::located::TimeDomain&& domain)

    : constant_size::time::located::TimeDomain{
          std::forward<constant_size::time::located::TimeDomain>(domain)}

{
}


TimeDomain create_time_domain(
    hdf5::Group const& group,
    TimeDomain::Configuration const& configuration)
{
    auto domain = constant_size::time::located::create_time_domain(
        group, configuration);

    return std::move(domain);
}

}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
