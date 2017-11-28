#pragma once
#include "lue/constant_size/time/omnipresent/same_shape/value.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

class SpaceBox:
    public same_shape::Value
{

public:

                   // SpaceBox            (hdf5::Identifier const& location);

                   SpaceBox            (hdf5::Group const& parent,
                                        hdf5::Datatype const memory_datatype);

                   SpaceBox            (same_shape::Value&& value);

                   SpaceBox            (SpaceBox const&)=delete;

                   SpaceBox            (SpaceBox&&)=default;

                   ~SpaceBox           ()=default;

    SpaceBox&      operator=           (SpaceBox const&)=delete;

    SpaceBox&      operator=           (SpaceBox&&)=default;

private:


};


// bool               space_box_exists    (hdf5::Group const& group);

SpaceBox           create_space_box    (hdf5::Group const& parent,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        size_t const rank);

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
