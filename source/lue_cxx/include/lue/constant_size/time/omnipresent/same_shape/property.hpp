#pragma once
#include "lue/constant_size/time/omnipresent/same_shape/value.hpp"
#include "lue/constant_size/time/omnipresent/property.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {
namespace same_shape {

class Property:
    public omnipresent::Property
{

public:

    static hdf5::Datatype file_datatype(hdf5::Group const& parent);

                   Property            (hdf5::Group&& group,
                                        hdf5::Datatype const& memory_datatype);

                   Property            (Property const& other)=delete;

                   Property            (Property&& other)=default;

                   ~Property           ()=default;

    Property&      operator=           (Property const& other)=delete;

    Property&      operator=           (Property&& other)=default;

    same_shape::Value const&
                   values              () const;

    same_shape::Value&
                   values              ();

    same_shape::Value&
                   reserve             (hsize_t const nr_items);

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
}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
