#include "lue/constant_size/time/omnipresent/space_domain.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

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


SpaceDomain::SpaceDomain(
    SpaceDomain const& other)

    : constant_size::SpaceDomain(other)

{
}


SpaceDomain create_space_domain(
    hdf5::Group const& group)
{
    auto domain = constant_size::create_space_domain(group);

    return std::move(domain);
}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
