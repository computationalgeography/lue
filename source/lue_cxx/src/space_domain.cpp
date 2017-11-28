#include "lue/space_domain.hpp"
#include "lue/enum_string_bimap.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace {

detail::EnumStringBimap<SpaceDomain::Configuration::DomainType> const
        domain_type_map = {
    { SpaceDomain::Configuration::DomainType::located, "lue_located" }
};


std::string domain_type_to_string(
    SpaceDomain::Configuration::DomainType const domain_type)
{
    return domain_type_map.as_string(domain_type);
}


SpaceDomain::Configuration::DomainType parse_domain_type(
    std::string const& string)
{
    if(!domain_type_map.contains(string)) {
        throw std::runtime_error("Unknown space domain type: " + string);
    }

    return domain_type_map.as_value(string);
}


detail::EnumStringBimap<SpaceDomain::Configuration::ItemType> const
        item_type_map = {
    { SpaceDomain::Configuration::ItemType::box, "lue_box" },
    { SpaceDomain::Configuration::ItemType::point, "lue_point" }
};


std::string item_type_to_string(
    SpaceDomain::Configuration::ItemType const item_type)
{
    return item_type_map.as_string(item_type);
}


SpaceDomain::Configuration::ItemType parse_item_type(
    std::string const& string)
{
    if(!item_type_map.contains(string)) {
        throw std::runtime_error("Unknown space domain item type: " + string);
    }

    return item_type_map.as_value(string);
}

}  // Anonymous namespace


SpaceDomain::Configuration::Configuration(
    DomainType const domain_type,
    ItemType const item_type)

    : _domain_type{domain_type},
      _item_type{item_type}

{
}


SpaceDomain::Configuration::Configuration(
    hdf5::Attributes const& attributes)
{
    load(attributes);
}


SpaceDomain::Configuration::DomainType
    SpaceDomain::Configuration::domain_type() const
{
    return _domain_type;
}


SpaceDomain::Configuration::ItemType
    SpaceDomain::Configuration::item_type() const
{
    return _item_type;
}


void SpaceDomain::Configuration::save(
    hdf5::Attributes& attributes) const
{
    attributes.write<std::string>(
        space_domain_type_tag,
        domain_type_to_string(_domain_type)
    );
    attributes.write<std::string>(
        space_domain_item_type_tag,
        item_type_to_string(_item_type)
    );
}


void SpaceDomain::Configuration::load(
    hdf5::Attributes const& attributes)
{
    _domain_type = parse_domain_type(
        attributes.read<std::string>(space_domain_type_tag));
    _item_type = parse_item_type(
        attributes.read<std::string>(space_domain_item_type_tag));
}


SpaceDomain::SpaceDomain(
    hdf5::Group const& parent)

    : hdf5::Group(parent, space_domain_tag),
      _configuration(attributes())

{
}


SpaceDomain::SpaceDomain(
    hdf5::Group&& group)

    : hdf5::Group{std::forward<hdf5::Group>(group)},
      _configuration(attributes())

{
}


// SpaceDomain::SpaceDomain(
//     SpaceDomain const& other)
// 
//     : hdf5::Group(other)
// 
// {
// }


SpaceDomain::Configuration const& SpaceDomain::configuration() const
{
    return _configuration;
}


SpaceDomain create_space_domain(
    hdf5::Group const& parent,
    SpaceDomain::Configuration const& configuration)
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
