#pragma once
#include "lue/info/property/property_group.hpp"
#include "lue/array/different_shape/value.hpp"


namespace lue {
namespace data_model {
namespace different_shape {

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
                                        different_shape::Value&& value);

                   ~Property           ()=default;

    Property&      operator=           (Property const&)=delete;

    Property&      operator=           (Property&&)=default;

    different_shape::Value const&
                   value               () const;

    different_shape::Value&
                   value               ();

private:

    different_shape::Value _value;

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

}  // namespace different_shape
}  // namespace data_model
}  // namespace lue
