#include "lue/constant_size/time/located/shared/constant_shape/same_shape/property.hpp"
#include "lue/tag.hpp"
#include <cassert>


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {
namespace constant_shape {
namespace same_shape {

hdf5::Datatype Property::file_datatype(
    hdf5::Identifier const& id)
{
    return hdf5::Dataset(id, value_tag).datatype();
}


Property::Property(
    hdf5::Identifier const& id,
    hdf5::Datatype const& memory_datatype)

    : constant_shape::Property(id),
      _values(this->id(), value_tag, memory_datatype)

{
}


Property::Property(
    constant_shape::Property&& property,
    hdf5::Datatype const& memory_datatype)

    : constant_shape::Property(std::forward<constant_shape::Property>(property)),
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
    hsize_t const nr_time_domain_items,
    hsize_t const nr_items)
{
    _values.reserve(nr_time_domain_items, nr_items);

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
        constant_shape::create_property(properties, name, configuration));
    auto value = create_value(
        property.id(), value_tag, file_datatype, memory_datatype);

    assert(property.id().is_valid());

    // return Property(std::move(property), memory_datatype);
    return Property(property.id(), memory_datatype);
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
        constant_shape::create_property(properties, name, configuration));
    auto value = create_value(
        property.id(), value_tag, file_datatype, memory_datatype,
        value_shape);

    assert(property.id().is_valid());

    // return Property(std::move(property), memory_datatype);
    return Property(property.id(), memory_datatype);
}

}  // namespace same_shape
}  // namespace constant_shape
}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
