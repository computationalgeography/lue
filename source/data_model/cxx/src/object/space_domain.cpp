#include "lue/object/space_domain.hpp"
#include "lue/core/tag.hpp"


namespace lue {
namespace data_model {

/*!
    @brief      Construct instance based on parent group
    @param      parent Parent group of group representing the space domain
*/
SpaceDomain::SpaceDomain(
    hdf5::Group const& parent):

    SpaceDomain{hdf5::Group{parent, space_domain_tag}}

{
}


/*!
    @brief      Construct instance based on group that represents the
                space domain
    @param      group Group representing the space domain
*/
SpaceDomain::SpaceDomain(
    hdf5::Group&& group):

    hdf5::Group{std::move(group)},
    _configuration{attributes()}

{
}


/*!
    @brief      Return space configuration
*/
SpaceConfiguration const& SpaceDomain::configuration() const
{
    return _configuration;
}


bool SpaceDomain::presence_is_discretized() const
{
    return contains_soft_link(presence_discretization_property_tag);
}


void SpaceDomain::set_presence_discretization(
    PropertyGroup& property)
{
    create_soft_link(property.id(), presence_discretization_property_tag);
}


PropertyGroup SpaceDomain::discretized_presence_property()
{
    assert(this->contains_soft_link(presence_discretization_property_tag));

    return PropertyGroup{*this, hdf5::SoftLink{*this, presence_discretization_property_tag}.path()};
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

}  // namespace data_model
}  // namespace lue
