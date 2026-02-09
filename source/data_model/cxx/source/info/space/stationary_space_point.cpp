#include "lue/info/space/stationary_space_point.hpp"
#include "lue/core/tag.hpp"


namespace lue::data_model {

    StationarySpacePoint::StationarySpacePoint(hdf5::Group const& parent):

        same_shape::Value{parent, coordinates_tag}

    {
    }


    StationarySpacePoint::StationarySpacePoint(
        hdf5::Group const& parent, hdf5::Datatype const& memory_datatype):

        same_shape::Value{parent, coordinates_tag, memory_datatype}

    {
    }


    StationarySpacePoint::StationarySpacePoint(same_shape::Value&& value):

        same_shape::Value{std::move(value)}

    {
    }


    auto StationarySpacePoint::nr_points() const -> Count
    {
        return nr_arrays();
    }


    auto create_stationary_space_point(
        hdf5::Group& parent, hdf5::Datatype const& memory_datatype, std::size_t const rank)
        -> StationarySpacePoint
    {
        return create_stationary_space_point(parent, file_datatype(memory_datatype), memory_datatype, rank);
    }


    auto create_stationary_space_point(
        hdf5::Group& parent,
        hdf5::Datatype const& file_datatype,
        hdf5::Datatype const& memory_datatype,
        std::size_t const rank) -> StationarySpacePoint
    {
        // A point is defined by the coordinates along each dimension
        hdf5::Shape const value_shape{rank};

        same_shape::Value value{
            same_shape::create_value(parent, coordinates_tag, file_datatype, memory_datatype, value_shape)};

        return StationarySpacePoint{std::move(value)};
    }

}  // namespace lue::data_model
