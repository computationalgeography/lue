#pragma once
#include "lue/item/same_shape/constant_shape/asynchronous_value.hpp"


namespace lue {
namespace mobile {

class AsynchronousPoint:
    public same_shape::constant_shape::AsynchronousValue
{

public:

                   AsynchronousPoint   (hdf5::Group const& parent,
                                        hdf5::Datatype const& memory_datatype);

                   AsynchronousPoint   (AsynchronousPoint const&)=delete;

                   AsynchronousPoint   (AsynchronousPoint&&)=default;

                   ~AsynchronousPoint  ()=default;

    AsynchronousPoint&
                   operator=           (AsynchronousPoint const&)=delete;

    AsynchronousPoint&
                   operator=           (AsynchronousPoint&&)=default;

private:

};


AsynchronousPoint  create_asynchronous_point(
                                        hdf5::Group& parent,
                                        hdf5::Datatype const& memory_datatype,
                                        std::size_t rank);

AsynchronousPoint  create_asynchronous_point(
                                        hdf5::Group& parent,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        std::size_t rank);

}  // namespace mobile
}  // namespace lue
