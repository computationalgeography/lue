#include "lue/constant_size/time/omnipresent/different_shape/property.hpp"
#include "lue/tag.hpp"
#include <cassert>


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {
namespace different_shape {

hdf5::Datatype Property::file_datatype(
    hdf5::Group const& parent)
{
    return hdf5::decode_datatype(
        hdf5::Group{parent, value_tag}.attributes()
            .read<std::vector<unsigned char>>(datatype_tag));
}


Property::Property(
    hdf5::Group&& group)

    : omnipresent::Property{std::forward<hdf5::Group>(group)},
      _values{*this, value_tag, memory_datatype(file_datatype(*this))}

{
}


Property::Property(
    hdf5::Group&& group,
    hdf5::Datatype const& memory_datatype)

    : omnipresent::Property{std::forward<hdf5::Group>(group)},
      _values(*this, value_tag, memory_datatype)

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


different_shape::Value& Property::reserve(
    hsize_t const nr_items,
    hsize_t const* shapes)
{
    _values.reserve(nr_items, shapes);

    return _values;
}


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
        omnipresent::create_property(properties, name, configuration));
    auto value = create_value(
        property, value_tag, file_datatype, memory_datatype, rank);

    assert(property.id().is_valid());

    return Property{hdf5::Group{property.id()}, memory_datatype};
}

}  // namespace different_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
