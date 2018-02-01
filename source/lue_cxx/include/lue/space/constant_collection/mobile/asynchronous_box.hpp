#pragma once
#include "lue/item/constant_collection/constant_shape/same_shape/asynchronous_variable.hpp"


namespace lue {
namespace constant_collection {
namespace mobile {

class AsynchronousBox:
    public constant_shape::same_shape::AsynchronousVariable
{

public:

                   AsynchronousBox     (hdf5::Group const& parent,
                                        hdf5::Datatype const& memory_datatype);

                   AsynchronousBox     (AsynchronousBox const&)=delete;

                   AsynchronousBox     (AsynchronousBox&&)=default;

                   ~AsynchronousBox    ()=default;

    AsynchronousBox&
                   operator=           (AsynchronousBox const&)=delete;

    AsynchronousBox&
                   operator=           (AsynchronousBox&&)=default;

private:

};


AsynchronousBox  create_asynchronous_box(
                                        hdf5::Group& parent,
                                        hdf5::Datatype const& memory_datatype,
                                        std::size_t rank);

AsynchronousBox  create_asynchronous_box(
                                        hdf5::Group& parent,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        std::size_t rank);

}  // namespace mobile
}  // namespace constant_collection
}  // namespace lue
