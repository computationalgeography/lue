#pragma once
#include "lue/item/constant_collection/constant_shape/same_shape/constant.hpp"


namespace lue {
namespace constant_collection {
namespace stationary {

class Box:
    public constant_shape::same_shape::Constant
{

public:

                   Box                 (hdf5::Group const& parent,
                                        hdf5::Datatype const& memory_datatype);

                   Box                 (Box const&)=delete;

                   Box                 (Box&&)=default;

                   ~Box                ()=default;

    Box&           operator=           (Box const&)=delete;

    Box&           operator=           (Box&&)=default;

private:

};


Box                create_box          (hdf5::Group& parent,
                                        hdf5::Datatype const& memory_datatype,
                                        std::size_t rank);

Box                create_box          (hdf5::Group& parent,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        std::size_t rank);

}  // namespace stationary
}  // namespace constant_collection
}  // namespace lue
