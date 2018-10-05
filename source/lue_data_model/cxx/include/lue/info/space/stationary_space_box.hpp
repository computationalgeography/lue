#pragma once
#include "lue/array/same_shape/value.hpp"


namespace lue {

/*!
    - One space box per item
    - Each space box has a unique location in space
*/
class StationarySpaceBox:
    public same_shape::Value
{

public:

                   StationarySpaceBox  (hdf5::Group& parent);

                   StationarySpaceBox  (hdf5::Group& parent,
                                        hdf5::Datatype const& memory_datatype);

                   StationarySpaceBox  (StationarySpaceBox const&)=delete;

                   StationarySpaceBox  (StationarySpaceBox&&)=default;

                   StationarySpaceBox  (same_shape::Value&& value);

                   ~StationarySpaceBox ()=default;

    StationarySpaceBox& operator=      (StationarySpaceBox const&)=delete;

    StationarySpaceBox& operator=      (StationarySpaceBox&&)=default;

    Count          nr_boxes            () const;

private:

};


StationarySpaceBox create_stationary_space_box(
                                        hdf5::Group& parent,
                                        hdf5::Datatype const& memory_datatype,
                                        std::size_t rank);

StationarySpaceBox create_stationary_space_box(
                                        hdf5::Group& parent,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        std::size_t rank);

}  // namespace lue
