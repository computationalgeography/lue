#pragma once
#include "lue/constant_size/time/omnipresent/same_shape/value.h"
#include "lue/constant_size/time/omnipresent/property.h"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {
namespace same_shape {

class Property:
    public omnipresent::Property
{

public:

    static hdf5::Datatype file_datatype(hdf5::Identifier const& id);

                   // Property            (hdf5::Identifier const& location,
                   //                      std::string const& name);

                   Property            (omnipresent::Property&& property,
                                        hdf5::Datatype const& memory_datatype);

                   Property            (lue::Property const& property,
                                        hdf5::Datatype const& memory_datatype);

                   // Property            (lue::Property& group,
                   //                      hid_t const type_id);

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
                                        hdf5::Datatype const& memory_datatype,
                                        hdf5::Shape const& value_shape,
                                        hdf5::Shape const& value_chunk);

// void               configure_property  (lue::Property const& location,
//                                         hid_t const file_type_id,
//                                         hid_t const memory_type_id,
//                                         Shape const& shape,
//                                         Chunks const& chunks);

}  // namespace same_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
