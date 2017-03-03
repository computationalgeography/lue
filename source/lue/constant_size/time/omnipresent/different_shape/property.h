#pragma once
#include "lue/constant_size/time/omnipresent/different_shape/value.h"
#include "lue/constant_size/time/omnipresent/property.h"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {
namespace different_shape {

class Property:
    public omnipresent::Property
{

public:

    static hdf5::Datatype file_datatype(hdf5::Identifier const& id);

                   Property            (omnipresent::Property&& property,
                                        hdf5::Datatype const& memory_datatype);

                   Property            (lue::Property const& property,
                                        hdf5::Datatype const& memory_datatype);

                   Property            (Property const& other)=delete;

                   Property            (Property&& other)=default;

                   ~Property           ()=default;

    Property&      operator=           (Property const& other)=delete;

    Property&      operator=           (Property&& other)=default;

    different_shape::Value const&
                   values              () const;

    different_shape::Value&
                   values              ();

    different_shape::Value&
                   reserve             (hsize_t const nr_items,
                                        hsize_t const* shapes);

private:

    different_shape::Value _values;

};


Property           create_property     (PropertySet& property_set,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        int const rank);

}  // namespace different_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
