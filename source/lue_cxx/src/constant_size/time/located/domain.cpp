#include "lue/constant_size/time/located/domain.hpp"
#include "lue/enum_string_bimap.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace {

detail::EnumStringBimap<Domain::Configuration::ItemType> const
        item_type_map = {
    { Domain::Configuration::ItemType::box, "lue_box" }
};


std::string item_type_to_string(
    Domain::Configuration::ItemType const item_type)
{
    return item_type_map.as_string(item_type);
}


Domain::Configuration::ItemType parse_item_type(
    std::string const& string)
{
    if(!item_type_map.contains(string)) {
        throw std::runtime_error("Unknown time domain item type: " + string);
    }

    return item_type_map.as_value(string);
}

}  // Anonymous namespace


Domain::Configuration::Configuration(
    ItemType const item_type)

    : _item_type{item_type}

{
}


Domain::Configuration::Configuration(
    hdf5::Attributes const& attributes)
{
    load(attributes);
}


Domain::Configuration::ItemType Domain::Configuration::item_type() const
{
    return _item_type;
}


void Domain::Configuration::save(
    hdf5::Attributes& attributes) const
{
    attributes.write<std::string>(
        time_domain_item_type_tag,
        item_type_to_string(_item_type)
    );
}


void Domain::Configuration::load(
    hdf5::Attributes const& attributes)
{
    _item_type = parse_item_type(
        attributes.read<std::string>(time_domain_item_type_tag));
}


Domain::Domain(
    hdf5::Identifier const& location)

    : constant_size::Domain(location),
      _configuration(attributes())

{
}


Domain::Domain(
    constant_size::Domain&& domain)

    : constant_size::Domain(std::forward<constant_size::Domain>(domain)),
      _configuration(attributes())

{
}


Domain::Configuration const& Domain::configuration() const
{
    return _configuration;
}

}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
