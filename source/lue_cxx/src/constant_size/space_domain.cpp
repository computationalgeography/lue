#include "lue/constant_size/space_domain.h"


namespace lue {
namespace constant_size {


SpaceDomain::SpaceDomain(
    hdf5::Group const& group)

    : lue::SpaceDomain(group)

{
}


SpaceDomain::SpaceDomain(
    lue::SpaceDomain&& domain)

    : lue::SpaceDomain{std::forward<lue::SpaceDomain>(domain)}

{
}


SpaceDomain::SpaceDomain(
    SpaceDomain const& other)

    : lue::SpaceDomain(other)

{
}


SpaceDomain create_space_domain(
    hdf5::Group const& group)
{
    auto domain = lue::create_space_domain(group);

    return std::move(domain);
}

}  // namespace constant_size
}  // namespace lue
