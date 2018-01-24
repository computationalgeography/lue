#pragma once
#include "lue/constant_collection/time/located/constant_shape/property.hpp"
#include "lue/constant_collection/time/located/constant_shape/same_shape/value.hpp"
#include "lue/constant_collection/time/located/property_set.hpp"


namespace lue {
namespace constant_collection {
namespace time {
namespace located {
namespace constant_shape {
namespace same_shape {

class Property:
    public constant_shape::Property
{

public:

    static hdf5::Datatype file_datatype(hdf5::Group const& parent);

                   Property            (hdf5::Group&& group,
                                        hdf5::Datatype const& memory_datatype);

                   Property            (constant_shape::Property&& property,
                                        hdf5::Datatype const& memory_datatype);

                   Property            (Property const&)=delete;

                   Property            (Property&&)=default;

                   ~Property           ()=default;

    Property&      operator=           (Property const&)=delete;

    Property&      operator=           (Property&&)=default;

    same_shape::Value const&
                   values              () const;

    same_shape::Value&
                   values              ();

private:

    same_shape::Value _values;

};


Property           create_property     (PropertySet& property_set,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype);

Property           create_property     (PropertySet& property_set,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        hdf5::Shape const& value_shape);

}  // namespace same_shape
}  // namespace constant_shape
}  // namespace located
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
