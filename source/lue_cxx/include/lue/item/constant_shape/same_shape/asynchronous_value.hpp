#pragma once
#include "lue/item/constant_shape/same_shape/value.hpp"


namespace lue {
namespace constant_shape {
namespace same_shape {

class AsynchronousValue:
    public Value
{

public:

                   AsynchronousValue   (hdf5::Group const& parent,
                                        std::string const& name);

                   AsynchronousValue   (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

                   AsynchronousValue   (hdf5::Group&& group,
                                        hdf5::Datatype const& memory_datatype);

                   AsynchronousValue   (AsynchronousValue const&)=delete;

                   AsynchronousValue   (AsynchronousValue&&)=default;

                   ~AsynchronousValue  ()=default;

    AsynchronousValue& operator=       (AsynchronousValue const&)=delete;

    AsynchronousValue& operator=       (AsynchronousValue&&)=default;

private:

};


AsynchronousValue create_asynchronous_value(
                                        hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype,
                                        hdf5::Shape const& value_shape);

AsynchronousValue create_asynchronous_value(
                                        hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        hdf5::Shape const& value_shape);

}  // namespace same_shape
}  // namespace constant_shape
}  // namespace lue
