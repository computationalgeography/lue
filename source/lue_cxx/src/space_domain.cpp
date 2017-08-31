#include "lue/space_domain.hpp"
#include "lue/tag.hpp"


namespace lue {

SpaceDomain::SpaceDomain(
    hdf5::Group const& group)

    : hdf5::Group(group.id(), space_domain_tag)

{
}


SpaceDomain::SpaceDomain(
    hdf5::Group&& group)

    : hdf5::Group(std::forward<hdf5::Group>(group))

{
}


SpaceDomain::SpaceDomain(
    SpaceDomain const& other)

    : hdf5::Group(other)

{
}


SpaceDomain create_space_domain(
    hdf5::Group const& group)
{
    auto domain = hdf5::create_group(group, space_domain_tag);

    return std::move(domain);
}

}  // namespace lue
