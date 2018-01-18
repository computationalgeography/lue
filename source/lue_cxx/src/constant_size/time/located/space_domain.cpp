#include "lue/constant_size/time/located/space_domain.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {

SpaceDomain::SpaceDomain(
    hdf5::Group const& group)

    : constant_size::SpaceDomain(group)

{
}


SpaceDomain::SpaceDomain(
    constant_size::SpaceDomain&& domain)

    : constant_size::SpaceDomain{
          std::forward<constant_size::SpaceDomain>(domain)}

{
}


SpaceDomain create_space_domain(
    hdf5::Group const& group,
    SpaceDomain::Configuration const& configuration)
{
    auto domain = constant_size::create_space_domain(group, configuration);

    return std::move(domain);
}

}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
