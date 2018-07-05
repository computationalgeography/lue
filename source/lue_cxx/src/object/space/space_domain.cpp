#include "lue/object/space/space_domain.hpp"
#include "lue/info/space.hpp"
#include "lue/core/tag.hpp"


namespace lue {

SpaceDomain::SpaceDomain(
    hdf5::Group& parent):

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
    SpaceConfiguration const& configuration,
    hdf5::Datatype const& datatype,
    std::size_t const rank)
{
    auto group = hdf5::create_group(parent, space_domain_tag);

    switch(configuration.value<Mobility>()) {
        case Mobility::stationary: {
            switch(configuration.value<SpaceDomainItemType>()) {
                case SpaceDomainItemType::point: {
                    create_stationary_space_point(group, datatype, rank);
                    break;
                }
                case SpaceDomainItemType::box: {
                    create_stationary_space_box(group, datatype, rank);
                    break;
                }
            }

            break;
        }
        case Mobility::mobile: {
            switch(configuration.value<SpaceDomainItemType>()) {
                case SpaceDomainItemType::point: {
                    create_mobile_space_point(group, datatype, rank);
                    break;
                }
                case SpaceDomainItemType::box: {
                    create_mobile_space_box(group, datatype, rank);
                    break;
                }
            }

            break;
        }
    }

    configuration.save(group.attributes());

    return SpaceDomain{std::move(group)};
}


bool space_domain_exists(
    hdf5::Group const& parent)
{
    return parent.contains_group(space_domain_tag);
}

}  // namespace lue
