#pragma once
#include "lue/info/property/property_group.hpp"
#include "lue/array/same_shape/constant_shape/value.hpp"


namespace lue {
namespace same_shape {
namespace constant_shape {

class Property:
    public PropertyGroup
{

public:

                   Property            (hdf5::Group& parent,
                                        std::string const& name);

                   Property            (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

                   Property            (Property const&)=delete;

                   Property            (Property&&)=default;

                   Property            (PropertyGroup&& group,
                                    same_shape::constant_shape::Value&& value);

                   ~Property           ()=default;

    Property&      operator=           (Property const&)=delete;

    Property&      operator=           (Property&&)=default;

    same_shape::constant_shape::Value const&
                   value               () const;

    same_shape::constant_shape::Value&
                   value               ();

private:

    same_shape::constant_shape::Value _value;

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

}  // namespace constant_shape
}  // namespace same_shape
}  // namespace lue
