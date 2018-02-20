#pragma once
#include "lue/item/same_shape/constant_shape/asynchronous_value.hpp"


namespace lue {
namespace mobile {

class AsynchronousBox:
    public same_shape::constant_shape::AsynchronousValue
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
}  // namespace lue
