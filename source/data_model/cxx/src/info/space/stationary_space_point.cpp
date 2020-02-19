#include "lue/info/space/stationary_space_point.hpp"
#include "lue/core/tag.hpp"


namespace lue {
namespace data_model {

StationarySpacePoint::StationarySpacePoint(
    hdf5::Group& parent):

    same_shape::Value{parent, coordinates_tag}

{
}


StationarySpacePoint::StationarySpacePoint(
    hdf5::Group& parent,
    hdf5::Datatype const& memory_datatype):

    same_shape::Value{parent, coordinates_tag, memory_datatype}

{
}


StationarySpacePoint::StationarySpacePoint(
    same_shape::Value&& value):

    same_shape::Value{std::forward<same_shape::Value>(value)}

{
}


Count StationarySpacePoint::nr_points() const
{
    return nr_arrays();
}


StationarySpacePoint create_stationary_space_point(
    hdf5::Group& parent,
    hdf5::Datatype const& memory_datatype,
    std::size_t const rank)
{
    return create_stationary_space_point(
        parent, file_datatype(memory_datatype), memory_datatype, rank);
}


StationarySpacePoint create_stationary_space_point(
    hdf5::Group& parent,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    std::size_t const rank)
{
    // A point is defined by the coordinates along each dimension
    hdf5::Shape value_shape{rank};

    auto value = same_shape::create_value(
        parent, coordinates_tag, file_datatype, memory_datatype, value_shape);

    return StationarySpacePoint{std::move(value)};
}

}  // namespace data_model
}  // namespace lue
