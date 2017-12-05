#pragma once
#include "lue/item/constant_size/constant/same_shape.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

class SpaceBox:
    public constant::SameShape
{

public:

                   SpaceBox            (hdf5::Group const& parent,
                                        hdf5::Datatype const memory_datatype);

                   SpaceBox            (constant::SameShape&& collection);

                   SpaceBox            (SpaceBox const&)=delete;

                   SpaceBox            (SpaceBox&&)=default;

                   ~SpaceBox           ()=default;

    SpaceBox&      operator=           (SpaceBox const&)=delete;

    SpaceBox&      operator=           (SpaceBox&&)=default;

private:


};


SpaceBox           create_space_box    (hdf5::Group const& parent,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        size_t rank);

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
