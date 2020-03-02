#include "lue/info/property/different_shape/property.hpp"
#include "lue/core/tag.hpp"


namespace lue {
namespace data_model {
namespace different_shape {

Property::Property(
    hdf5::Group& parent,
    std::string const& name):

    PropertyGroup{parent, name},
    _value{*this, value_tag}

{
}


Property::Property(
    hdf5::Group& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype):

    PropertyGroup{parent, name},
    _value{*this, value_tag, memory_datatype}

{
}


Property::Property(
    PropertyGroup&& group,
    Value&& value):

    PropertyGroup{std::forward<PropertyGroup>(group)},
    _value{std::forward<Value>(value)}

{
}


Property::Value const& Property::value() const
{
    return _value;
}


Property::Value& Property::value()
{
    return _value;
}


Property create_property(
    hdf5::Group& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype,
    Rank const rank)
{
    return create_property(
        parent, name,
        file_datatype(memory_datatype), memory_datatype,
        rank);
}


Property create_property(
    hdf5::Group& parent,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    Rank const rank)
{
    auto group = create_property_group(parent, name);
    auto value = create_value(
        group, value_tag, file_datatype, memory_datatype, rank);

    return Property{std::move(group), std::move(value)};
}

}  // namespace different_shape
}  // namespace data_model
}  // namespace lue
