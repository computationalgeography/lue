#pragma once
#include "lue/info/property/property_group.hpp"
#include "lue/info/property/property_traits.hpp"
#include "lue/array/same_shape/value.hpp"


namespace lue {
namespace data_model {
namespace same_shape {

class Property:
    public PropertyGroup
{

public:

    using Value = same_shape::Value;

                   Property            (hdf5::Group& parent,
                                        std::string const& name);

                   Property            (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

                   Property            (Property const&)=delete;

                   Property            (Property&&)=default;

                   Property            (PropertyGroup&& group,
                                        Value&& value);

                   ~Property           ()=default;

    Property&      operator=           (Property const&)=delete;

    Property&      operator=           (Property&&)=default;

    Value const&   value               () const;

    Value&         value               ();

private:

    Value          _value;

};


Property           create_property     (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

Property           create_property     (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype,
                                        hdf5::Shape const& array_shape);

Property           create_property     (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype);

Property           create_property     (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        hdf5::Shape const& array_shape);

}  // namespace same_shape


template<>
class PropertyTraits<same_shape::Property>
{

public:

    using Value = same_shape::Property::Value;

};

}  // namespace data_model
}  // namespace lue
