#pragma once
#include "lue/constant_size/time/located/shared/constant_shape/same_shape/value.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {

class SpaceBox:
    public constant_shape::same_shape::Value
{

public:

                   SpaceBox            (hdf5::Identifier const& location,
                                        hdf5::Datatype const memory_datatype);

                   SpaceBox            (constant_shape::same_shape::Value&&
                                            value);

                   SpaceBox            (SpaceBox const& other)=delete;

                   SpaceBox            (SpaceBox&& other)=default;

                   ~SpaceBox           ()=default;

    SpaceBox&      operator=           (SpaceBox const& other)=delete;

    SpaceBox&      operator=           (SpaceBox&& other)=default;

private:


};


SpaceBox           create_space_box    (hdf5::Group const& group,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        size_t const rank);

}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
