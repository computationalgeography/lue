#include "lue/constant_collection/time/omnipresent/same_shape/property.hpp"
#include "lue/tag.hpp"
#include <cassert>


namespace lue {
namespace constant_collection {
namespace time {
namespace omnipresent {
namespace same_shape {

hdf5::Datatype Property::file_datatype(
    hdf5::Group const& parent)
{
    return hdf5::Dataset(parent, value_tag).datatype();
}


Property::Property(
    hdf5::Group&& group,
    hdf5::Datatype const& memory_datatype)

    : omnipresent::Property{std::forward<hdf5::Group>(group)},
      _values(*this, value_tag, memory_datatype)

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


Property create_property(
    PropertySet& property_set,
    std::string const& name,
    hdf5::Datatype const& memory_datatype)
{
    return create_property(
        property_set, name, hdf5::file_datatype(memory_datatype),
        memory_datatype);
}


Property create_property(
    PropertySet& property_set,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype)
{
    auto& properties = property_set.properties();

    ItemConfiguration configuration{
        ShapePerItem::same,
        Occurrence::continuous,
        ShapeVariability::constant,
        CollectionVariability::constant
    };

    auto& property = properties.add(name,
        omnipresent::create_property(properties, name, configuration));
    auto value = create_value(
        property, value_tag, file_datatype, memory_datatype);

    assert(property.id().is_valid());

    return Property{hdf5::Group{property.id()}, memory_datatype};
}


Property create_property(
    PropertySet& property_set,
    std::string const& name,
    hdf5::Datatype const& memory_datatype,
    hdf5::Shape const& value_shape)
{
    return create_property(
        property_set, name, hdf5::file_datatype(memory_datatype),
        memory_datatype, value_shape);
}


Property create_property(
    PropertySet& property_set,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    hdf5::Shape const& value_shape)
{
    auto& properties = property_set.properties();

    ItemConfiguration configuration{
        ShapePerItem::same,
        Occurrence::continuous,
        ShapeVariability::constant,
        CollectionVariability::constant
    };

    auto& property = properties.add(name,
        omnipresent::create_property(properties, name, configuration));
    auto value = create_value(
        property, value_tag, file_datatype, memory_datatype,
        value_shape);

    assert(property.id().is_valid());

    return Property{hdf5::Group{property.id()}, memory_datatype};
}

}  // namespace same_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
