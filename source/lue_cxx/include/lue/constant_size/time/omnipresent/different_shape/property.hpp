#pragma once
#include "lue/constant_size/time/omnipresent/different_shape/value.hpp"
#include "lue/constant_size/time/omnipresent/property.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {
namespace different_shape {

class Property:
    public omnipresent::Property
{

public:

    static hdf5::Datatype file_datatype(hdf5::Group const& parent);

                   Property            (hdf5::Group&& group);

                   Property            (hdf5::Group&& group,
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

    different_shape::Value&
                   reserve             (hsize_t nr_items,
                                        hsize_t const* shapes);

private:

    different_shape::Value _values;

};


Property           create_property     (PropertySet& property_set,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype,
                                        int rank);

Property           create_property     (PropertySet& property_set,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        int rank);

}  // namespace different_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
