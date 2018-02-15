#pragma once
#include "lue/item/constant_shape/same_shape/asynchronous_value.hpp"


namespace lue {
namespace constant_collection {
namespace mobile {

class AsynchronousPoint:
    public lue::constant_shape::same_shape::AsynchronousValue
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
}  // namespace constant_collection
}  // namespace lue
