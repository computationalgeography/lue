#pragma once
#include "lue/constant_size/time/located/shared/constant_shape/property.hpp"
#include "lue/constant_size/time/located/shared/constant_shape/same_shape/value.hpp"
#include "lue/property_set.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {
namespace constant_shape {
namespace same_shape {

class Property:
    public constant_shape::Property
{

public:

    static hdf5::Datatype file_datatype(hdf5::Identifier const& id);

                   Property            (hdf5::Identifier const& id,
                                        hdf5::Datatype const& memory_datatype);

                   Property            (constant_shape::Property&& property,
                                        hdf5::Datatype const& memory_datatype);

    //                Property            (lue::Property const& property,
    //                                     hdf5::Datatype const& memory_datatype);

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
                   reserve             (hsize_t const nr_time_domain_items,
                                        hsize_t const nr_items);

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
}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
