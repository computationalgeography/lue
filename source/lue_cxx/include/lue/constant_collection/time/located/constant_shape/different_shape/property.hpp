#pragma once
#include "lue/constant_collection/time/located/constant_shape/different_shape/value.hpp"
#include "lue/constant_collection/time/located/constant_shape/property.hpp"
#include "lue/constant_collection/property_set.hpp"


namespace lue {
namespace constant_collection {
namespace time {
namespace located {
namespace constant_shape {
namespace different_shape {

class Property:
    public constant_shape::Property
{

public:

    static hdf5::Datatype file_datatype(hdf5::Group const& parent);

                   Property            (hdf5::Group&& parent);

                   Property            (hdf5::Group&& parent,
                                        hdf5::Datatype const& memory_datatype);

                   Property            (constant_shape::Property&& property,
                                        hdf5::Datatype const& memory_datatype);

                   Property            (Property const&)=delete;

                   Property            (Property&&)=default;

                   ~Property           ()=default;

    Property&      operator=           (Property const&)=delete;

    Property&      operator=           (Property&&)=default;

    different_shape::Value const&
                   values              () const;

    different_shape::Value&
                   values              ();

private:

    different_shape::Value _values;

};


Property           create_property     (PropertySet& property_set,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype,
                                        int const rank);

Property           create_property     (PropertySet& property_set,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        int const rank);

}  // namespace different_shape
}  // namespace constant_shape
}  // namespace located
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
