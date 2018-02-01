#pragma once
#include "lue/item/constant_collection/constant_shape/same_shape/asynchronous_variable.hpp"


namespace lue {
namespace constant_collection {
namespace mobile {

class AsynchronousPoint:
    public constant_shape::same_shape::AsynchronousVariable
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
