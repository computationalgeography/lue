#pragma once
#include "lue/item/constant_size/same_shape.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

class SpacePoint:
    public constant_size::SameShape
{

public:

                   SpacePoint          (hdf5::Group const& parent,
                                        hdf5::Datatype const memory_datatype);

                   SpacePoint          (constant_size::SameShape&& dataset);

                   SpacePoint          (SpacePoint const&)=delete;

                   SpacePoint          (SpacePoint&&)=default;

                   ~SpacePoint         ()=default;

    SpacePoint&    operator=           (SpacePoint const&)=delete;

    SpacePoint&    operator=           (SpacePoint&&)=default;

private:


};


SpacePoint         create_space_point  (hdf5::Group const& parent,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        size_t const rank);

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
