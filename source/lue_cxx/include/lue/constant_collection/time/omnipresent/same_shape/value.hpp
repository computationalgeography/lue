#pragma once
#include "lue/constant_collection/time/omnipresent/value.hpp"
#include "lue/item/same_shape/constant_shape/continuous_value.hpp"


namespace lue {
namespace constant_collection {
namespace time {
namespace omnipresent {
namespace same_shape {

class Value:
    public lue::same_shape::constant_shape::ContinuousValue,
    public omnipresent::Value
{

public:

                   Value               (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

                   Value               (
                            lue::same_shape::constant_shape::ContinuousValue&&
                                collection);

                   Value               (Value const&)=delete;

                   Value               (Value&&)=default;

                   ~Value              ()=default;

    Value&         operator=           (Value const&)=delete;

    Value&         operator=           (Value&&)=default;

    hsize_t        nr_items            () const final;

private:

};


Value              create_value        (hdf5::Group& group,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype);

Value              create_value        (hdf5::Group& group,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        hdf5::Shape const& value_shape);

}  // namespace same_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
