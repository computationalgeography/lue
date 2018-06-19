#pragma once
#include "lue/array/same_shape/constant_shape/value.hpp"


namespace lue {

/*!
    - Zero or more space boxs per item
    - Each space box has a unique location in space
*/
class MobileSpaceBox:
    public same_shape::constant_shape::Value
{

public:

                   MobileSpaceBox      (hdf5::Group const& parent,
                                        hdf5::Datatype const& memory_datatype);

                   MobileSpaceBox      (MobileSpaceBox const&)=delete;

                   MobileSpaceBox      (MobileSpaceBox&&)=default;

                   MobileSpaceBox      (same_shape::constant_shape::Value&&
                                            value);

                   ~MobileSpaceBox     ()=default;

    MobileSpaceBox& operator=          (MobileSpaceBox const&)=delete;

    MobileSpaceBox& operator=          (MobileSpaceBox&&)=default;

private:

};


MobileSpaceBox     create_mobile_space_box(
                                        hdf5::Group& parent,
                                        hdf5::Datatype const& memory_datatype,
                                        std::size_t rank);

MobileSpaceBox     create_mobile_space_box(
                                        hdf5::Group& parent,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        std::size_t rank);

}  // namespace lue
