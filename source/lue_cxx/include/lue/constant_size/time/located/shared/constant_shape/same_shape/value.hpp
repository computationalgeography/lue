#pragma once
#include "lue/constant_size/value.hpp"
#include "lue/array.hpp"
#include "lue/hdf5/group.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {
namespace constant_shape {
namespace same_shape {

class Value:
    public Array,
    public constant_size::Value
{

public:

                   Value               (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

                   Value               (hdf5::Dataset&& dataset,
                                        hdf5::Datatype const& memory_datatype);

                   Value               (Value const&)=delete;

                   Value               (Value&&)=default;

                   ~Value              ()=default;

    Value&         operator=           (Value const&)=delete;

    Value&         operator=           (Value&&)=default;

    void           reserve             (hsize_t nr_time_domain_items,
                                        hsize_t nr_items);

    hsize_t        nr_items            () const final;

    hdf5::Shape    value_shape         () const;

private:

};


Value              create_value        (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype);

Value              create_value        (hdf5::Group const& parent,
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
