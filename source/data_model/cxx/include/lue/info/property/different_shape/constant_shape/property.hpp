#pragma once
#include "lue/info/property/property_group.hpp"
#include "lue/info/property/property_traits.hpp"
#include "lue/array/different_shape/constant_shape/value.hpp"


namespace lue {
namespace data_model {
namespace different_shape {
namespace constant_shape {

class Property:
    public PropertyGroup
{

public:

    using Value = different_shape::constant_shape::Value;

                   Property            (hdf5::Group& parent,
                                        std::string const& name);

                   Property            (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

                   Property            (Property const&)=delete;

                   Property            (Property&&)=default;

                   Property            (PropertyGroup&& group,
                                        Value&& value);

                   ~Property           () override =default;

    Property&      operator=           (Property const&)=delete;

    Property&      operator=           (Property&&)=default;

    Value const&   value               () const;

    Value&         value               ();

private:

    Value          _value;

};


Property           create_property     (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype,
                                        Rank rank,
                                        std::string const& description="");

Property           create_property     (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        Rank rank,
                                        std::string const& description="");

}  // namespace constant_shape
}  // namespace different_shape


template<>
class PropertyTraits<different_shape::constant_shape::Property>
{

public:

    using Value = different_shape::constant_shape::Property::Value;

};

}  // namespace data_model
}  // namespace lue
