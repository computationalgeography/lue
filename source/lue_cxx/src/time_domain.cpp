#include "lue/time_domain.hpp"
#include "lue/enum_string_bimap.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace {

detail::EnumStringBimap<TimeDomain::Configuration::ItemType> const
        item_type_map = {
    { TimeDomain::Configuration::ItemType::box, "lue_box" }
};


std::string item_type_to_string(
    TimeDomain::Configuration::ItemType const item_type)
{
    return item_type_map.as_string(item_type);
}


TimeDomain::Configuration::ItemType parse_item_type(
    std::string const& string)
{
    if(!item_type_map.contains(string)) {
        throw std::runtime_error("Unknown time domain item type: " + string);
    }

    return item_type_map.as_value(string);
}

}  // Anonymous namespace


TimeDomain::Configuration::Configuration(
    ItemType const item_type)

    : _item_type{item_type}

{
}


TimeDomain::Configuration::Configuration(
    hdf5::Attributes const& attributes)
{
    load(attributes);
}


TimeDomain::Configuration::ItemType
    TimeDomain::Configuration::item_type() const
{
    return _item_type;
}


void TimeDomain::Configuration::save(
    hdf5::Attributes& attributes) const
{
    attributes.write<std::string>(
        time_domain_item_type_tag,
        item_type_to_string(_item_type)
    );
}


void TimeDomain::Configuration::load(
    hdf5::Attributes const& attributes)
{
    _item_type = parse_item_type(
        attributes.read<std::string>(time_domain_item_type_tag));
}


TimeDomain::TimeDomain(
    hdf5::Group const& group)

    : hdf5::Group(group.id(), time_domain_tag),
      _configuration(attributes())

{
}


TimeDomain::TimeDomain(
    hdf5::Group&& group)

    : hdf5::Group{std::forward<hdf5::Group>(group)},
      _configuration(attributes())

{
}


TimeDomain::Configuration const& TimeDomain::configuration() const
{
    return _configuration;
}


TimeDomain create_time_domain(
    hdf5::Group const& group,
    TimeDomain::Configuration const& configuration)
{
    auto domain = hdf5::create_group(group, time_domain_tag);

    configuration.save(domain.attributes());

    return std::move(domain);
}


bool time_domain_exists(
    hdf5::Group const& group)
{
    return group.contains_group(time_domain_tag);
}

}  // namespace lue
