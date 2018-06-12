#pragma once
#include "lue/item/same_shape/constant_shape/constant_collection/synchronous_value.hpp"


namespace lue {
namespace mobile {
namespace constant_collection {

class SynchronousPoint:
    public same_shape::constant_shape::constant_collection::SynchronousValue
{

public:

                   SynchronousPoint    (hdf5::Group const& parent,
                                        hdf5::Datatype const& memory_datatype);

                   SynchronousPoint    (SynchronousPoint const&)=delete;

                   SynchronousPoint    (SynchronousPoint&&)=default;

                   ~SynchronousPoint   ()=default;

    SynchronousPoint&
                   operator=           (SynchronousPoint const&)=delete;

    SynchronousPoint&
                   operator=           (SynchronousPoint&&)=default;

private:

};


SynchronousPoint   create_synchronous_point(
                                        hdf5::Group& parent,
                                        hdf5::Datatype const& memory_datatype,
                                        std::size_t rank);

SynchronousPoint   create_synchronous_point(
                                        hdf5::Group& parent,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        std::size_t rank);

}  // namespace constant_collection
}  // namespace mobile
}  // namespace lue
