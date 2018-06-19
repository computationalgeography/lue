#pragma once
#include "lue/info/property/property_group.hpp"
#include "lue/array/same_shape/variable_shape/value.hpp"


namespace lue {
namespace same_shape {
namespace variable_shape {

class Property:
    public PropertyGroup
{

public:

                   Property            (hdf5::Group const& parent,
                                        std::string const& name);

                   Property            (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

                   Property            (Property const&)=delete;

                   Property            (Property&&)=default;

                   Property            (PropertyGroup&& group,
                                    same_shape::variable_shape::Value&& value);

                   ~Property           ()=default;

    Property&      operator=           (Property const&)=delete;

    Property&      operator=           (Property&&)=default;

    same_shape::variable_shape::Value const&
                   value               () const;

    same_shape::variable_shape::Value&
                   value               ();

private:

    same_shape::variable_shape::Value _value;

};


Property           create_property     (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype,
                                        Rank rank);

Property           create_property     (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        Rank rank);

}  // namespace variable_shape
}  // namespace same_shape
}  // namespace lue
