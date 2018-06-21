#include "lue/object/space/space_domain.hpp"
#include "lue/core/tag.hpp"


namespace lue {

SpaceDomain::SpaceDomain(
    hdf5::Group const& parent):

    hdf5::Group{parent, space_domain_tag},
    _configuration{attributes()}

{
}


SpaceDomain::SpaceDomain(
    hdf5::Group&& group):

    hdf5::Group{std::forward<hdf5::Group>(group)},
    _configuration{attributes()}

{
}


SpaceConfiguration const& SpaceDomain::configuration() const
{
    return _configuration;
}


SpaceDomain create_space_domain(
    hdf5::Group& parent,
    SpaceConfiguration const& configuration)
{
    auto group = hdf5::create_group(parent, space_domain_tag);

    configuration.save(group.attributes());

    return SpaceDomain{std::move(group)};
}


bool space_domain_exists(
    hdf5::Group const& parent)
{
    return parent.contains_group(space_domain_tag);
}

}  // namespace lue
