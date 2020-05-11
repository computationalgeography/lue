#pragma once
#include "lue/array/same_shape/value.hpp"


namespace lue {
namespace data_model {

/*!
    - One space point per item
    - Each space point has a unique location in space
*/
class StationarySpacePoint:
    public same_shape::Value
{

public:

    explicit       StationarySpacePoint(hdf5::Group& parent);

                   StationarySpacePoint(hdf5::Group& parent,
                                        hdf5::Datatype const& memory_datatype);

                   StationarySpacePoint(StationarySpacePoint const&)=delete;

                   StationarySpacePoint(StationarySpacePoint&&)=default;

    explicit       StationarySpacePoint(same_shape::Value&& value);

                   ~StationarySpacePoint() override =default;

    StationarySpacePoint& operator=    (StationarySpacePoint const&)=delete;

    StationarySpacePoint& operator=    (StationarySpacePoint&&)=default;

    Count          nr_points           () const;

private:

};


StationarySpacePoint   create_stationary_space_point(
                                        hdf5::Group& parent,
                                        hdf5::Datatype const& memory_datatype,
                                        std::size_t rank);

StationarySpacePoint   create_stationary_space_point(
                                        hdf5::Group& parent,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        std::size_t rank);

}  // namespace data_model
}  // namespace lue
