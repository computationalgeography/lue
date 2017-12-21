#include "lue/constant_size/time/located/shared/constant_shape/different_shape/property.hpp"
#include "lue/tag.hpp"
#include <cassert>


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {
namespace constant_shape {
namespace different_shape {

hdf5::Datatype Property::file_datatype(
    hdf5::Group const& parent)
{
    return hdf5::decode_datatype(
        hdf5::Group{parent, value_tag}.attributes()
            .read<std::vector<unsigned char>>(datatype_tag));
}


Property::Property(
    hdf5::Group&& parent)

    : constant_shape::Property{std::forward<hdf5::Group>(parent)},
      _values{*this, value_tag}

{
}


Property::Property(
    hdf5::Group&& parent,
    hdf5::Datatype const& memory_datatype)

    : constant_shape::Property{std::forward<hdf5::Group>(parent)},
      _values{*this, value_tag, memory_datatype}

{
}


Property::Property(
    constant_shape::Property&& property,
    hdf5::Datatype const& memory_datatype)

    : constant_shape::Property{
            std::forward<constant_shape::Property>(property)},
      _values{*this, value_tag, memory_datatype}

{
}


different_shape::Value const& Property::values() const
{
    return _values;
}


different_shape::Value& Property::values()
{
    return _values;
}


// different_shape::Value& Property::reserve(
//     hsize_t const nr_time_domain_items,
//     hsize_t const nr_items,
//     hsize_t const* shapes)
// {
//     _values.reserve(nr_time_domain_items, nr_items, shapes);
// 
//     return _values;
// }


Property create_property(
    PropertySet& property_set,
    std::string const& name,
    hdf5::Datatype const& memory_datatype,
    int const rank)
{
    return create_property(property_set, name,
        hdf5::file_datatype(memory_datatype), memory_datatype, rank);
}


Property create_property(
    PropertySet& property_set,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    int const rank)
{
    auto& properties = property_set.properties();
    Property::Configuration configuration(ShapePerItemType::different);
    auto& property = properties.add(name,
        constant_shape::create_property(properties, name, configuration));
    auto value = create_value(property, value_tag, file_datatype,
        memory_datatype, rank);

    assert(property.id().is_valid());

    return Property(hdf5::Group{property.id()}, memory_datatype);
}

}  // namespace different_shape
}  // namespace constant_shape
}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
