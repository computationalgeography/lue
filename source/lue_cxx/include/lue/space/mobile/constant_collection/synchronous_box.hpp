#pragma once
#include "lue/item/same_shape/constant_shape/constant_collection/synchronous_value.hpp"


namespace lue {
namespace mobile {
namespace constant_collection {

class SynchronousBox:
    public same_shape::constant_shape::constant_collection::SynchronousValue
{

public:

                   SynchronousBox      (hdf5::Group const& parent,
                                        hdf5::Datatype const& memory_datatype);

                   SynchronousBox      (SynchronousBox const&)=delete;

                   SynchronousBox      (SynchronousBox&&)=default;

                   ~SynchronousBox     ()=default;

    SynchronousBox&
                   operator=           (SynchronousBox const&)=delete;

    SynchronousBox&
                   operator=           (SynchronousBox&&)=default;

private:

};


SynchronousBox   create_synchronous_box(hdf5::Group& parent,
                                        hdf5::Datatype const& memory_datatype,
                                        std::size_t rank);

SynchronousBox   create_synchronous_box(hdf5::Group& parent,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        std::size_t rank);

}  // namespace constant_collection
}  // namespace mobile
}  // namespace lue
