#include "lue/info/property/same_shape/constant_shape/property.hpp"
#include "lue/core/tag.hpp"


namespace lue::data_model::same_shape::constant_shape {

    Property::Property(hdf5::Group const& parent, std::string const& name):

        PropertyGroup{parent, name},
        _value{*this, value_tag}

    {
    }


    Property::Property(
        hdf5::Group const& parent, std::string const& name, hdf5::Datatype const& memory_datatype):

        PropertyGroup{parent, name},
        _value{*this, value_tag, memory_datatype}

    {
    }


    Property::Property(PropertyGroup&& group, Value&& value):

        PropertyGroup{std::move(group)},
        _value{std::move(value)}

    {
    }


    auto Property::value() const -> Property::Value const&
    {
        return _value;
    }


    auto Property::value() -> Property::Value&
    {
        return _value;
    }


    auto create_property(
        hdf5::Group& parent,
        std::string const& name,
        hdf5::Datatype const& memory_datatype,
        std::string const& description) -> Property
    {
        return create_property(
            parent, name, file_datatype(memory_datatype), memory_datatype, hdf5::Shape{}, description);
    }


    auto create_property(
        hdf5::Group& parent,
        std::string const& name,
        hdf5::Datatype const& memory_datatype,
        hdf5::Shape const& array_shape,
        std::string const& description) -> Property
    {
        return create_property(
            parent, name, file_datatype(memory_datatype), memory_datatype, array_shape, description);
    }


    auto create_property(
        hdf5::Group& parent,
        std::string const& name,
        hdf5::Datatype const& file_datatype,
        hdf5::Datatype const& memory_datatype,
        std::string const& description) -> Property
    {
        return create_property(parent, name, file_datatype, memory_datatype, hdf5::Shape{}, description);
    }


    auto create_property(
        hdf5::Group& parent,
        std::string const& name,
        hdf5::Datatype const& file_datatype,
        hdf5::Datatype const& memory_datatype,
        hdf5::Shape const& array_shape,
        std::string const& description) -> Property
    {
        PropertyGroup group{create_property_group(parent, name, description)};
        Value value{create_value(group, value_tag, file_datatype, memory_datatype, array_shape)};

        return Property{std::move(group), std::move(value)};
    }

}  // namespace lue::data_model::same_shape::constant_shape
