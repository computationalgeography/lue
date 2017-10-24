#include "lue/property.hpp"
#include "lue/enum_string_bimap.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace {

detail::EnumStringBimap<ShapePerItemType> const
        shape_per_item_type_map = {
    { ShapePerItemType::same, "lue_same_shape" },
    { ShapePerItemType::different, "lue_different_shape" }
};


std::string shape_per_item_type_to_string(
    ShapePerItemType const type)
{
    return shape_per_item_type_map.as_string(type);
}


ShapePerItemType parse_shape_per_item_type(
    std::string const& string)
{
    if(!shape_per_item_type_map.contains(string)) {
        throw std::runtime_error("Unknown shape per item type: " + string);
    }

    return shape_per_item_type_map.as_value(string);
}

}  // Anonymous namespace


Property::Configuration::Configuration(
    ShapePerItemType const type)

    : _shape_per_item_type{type}

{
}


Property::Configuration::Configuration(
    hdf5::Attributes const& attributes)
{
    load(attributes);
}


ShapePerItemType Property::Configuration::shape_per_item_type() const
{
    return _shape_per_item_type;
}


void Property::Configuration::save(
    hdf5::Attributes& attributes) const
{
    attributes.write<std::string>(
        shape_per_item_type_tag,
        shape_per_item_type_to_string(_shape_per_item_type)
    );
}


void Property::Configuration::load(
    hdf5::Attributes const& attributes)
{
    _shape_per_item_type =
        parse_shape_per_item_type(
            attributes.read<std::string>(
                shape_per_item_type_tag));
}


/*!
    @brief      Construct an instance based on an existing property
    @param      location Location in dataset of property named @a name
    @param      name Name of property to open
    @exception  std::runtime_error In case property cannot be opened
*/
Property::Property(
    hdf5::Identifier const& location,
    std::string const& name)

    : hdf5::Group(location, name),
      _configuration(attributes())

{
}


Property::Property(
    hdf5::Group&& group)

    : hdf5::Group(std::forward<hdf5::Group>(group)),
      _configuration(attributes())

{
}


Property::Configuration const& Property::configuration() const
{
    return _configuration;
}


// void Property::discretize_time(
//     Property const& property)
// {
//     create_soft_link(property.id(), time_discretization_tag);
// }


void Property::discretize_space(
    Property const& property)
{
    create_soft_link(property.id(), space_discretization_tag);
}


// bool Property::time_is_discretized () const
// {
//     return contains_soft_link(time_discretization_tag);
// }


bool Property::space_is_discretized() const
{
    return contains_soft_link(space_discretization_tag);
}


// Property Property::time_discretization () const
// {
//     return Property(id(), time_discretization_tag);
// }


Property Property::space_discretization() const
{
    return Property(id(), space_discretization_tag);
}


Property create_property(
    hdf5::Group& group,
    std::string const& name,
    Property::Configuration const& configuration)
{
    auto property = hdf5::create_group(group.id(), name);

    configuration.save(property.attributes());

    return Property(std::move(property));
}

}  // namespace lue
