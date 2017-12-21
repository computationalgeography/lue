#pragma once
#include "lue/constant_size/value.hpp"
#include "lue/item/constant_size/variable/constant_shape/different_shape/collection.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {
namespace constant_shape {
namespace different_shape {

class Value:
    public variable::constant_shape::different_shape::Collection,
    public constant_size::Value
{

public:

                   Value               (hdf5::Group const& parent,
                                        std::string const& name);

                   Value               (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

                   Value               (
                        variable::constant_shape::different_shape::Collection&&
                            collection);

                   Value               (Value const&)=delete;

                   Value               (Value&&)=default;

                   ~Value              ()=default;

    Value&         operator=           (Value const&)=delete;

    Value&         operator=           (Value&&)=default;

    hsize_t        nr_items            () const final;

private:

};


Value              create_value        (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        int rank);

}  // namespace different_shape
}  // namespace constant_shape
}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
