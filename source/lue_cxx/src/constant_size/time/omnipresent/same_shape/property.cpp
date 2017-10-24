#include "lue/constant_size/time/omnipresent/same_shape/property.hpp"
#include "lue/tag.hpp"
#include <cassert>


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {
namespace same_shape {

hdf5::Datatype Property::file_datatype(
    hdf5::Identifier const& id)
{
    return hdf5::Dataset(id, value_tag).datatype();
}


Property::Property(
    omnipresent::Property&& property,
    hdf5::Datatype const& memory_datatype)

    : omnipresent::Property(std::forward<omnipresent::Property>(property)),
      _values(id(), value_tag, memory_datatype)

{
}


Property::Property(
    lue::Property const& property,
    hdf5::Datatype const& memory_datatype)

    : omnipresent::Property(property),
      _values(id(), value_tag, memory_datatype)

{
}


same_shape::Value const& Property::values() const
{
    return _values;
}


same_shape::Value& Property::values()
{
    return _values;
}


same_shape::Value& Property::reserve(
    hsize_t const nr_items)
{
    _values.reserve(nr_items);

    return _values;
}


Property create_property(
    PropertySet& property_set,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype)
{
    auto& properties = property_set.properties();
    Property::Configuration configuration(ShapePerItemType::same);
    auto& property = properties.add(name,
        omnipresent::create_property(properties, name, configuration));
    auto value = create_value(
        property.id(), value_tag, file_datatype, memory_datatype);

    assert(property.id().is_valid());

    return Property(std::move(property), memory_datatype);
}


Property create_property(
    PropertySet& property_set,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    hdf5::Shape const& value_shape)
{
    auto& properties = property_set.properties();
    Property::Configuration configuration(ShapePerItemType::same);
    auto& property = properties.add(name,
        omnipresent::create_property(properties, name, configuration));
    auto value = create_value(
        property.id(), value_tag, file_datatype, memory_datatype,
        value_shape);

    assert(property.id().is_valid());

    return Property(std::move(property), memory_datatype);
}

}  // namespace same_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
