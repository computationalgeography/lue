#include "lue/space/constant_collection/stationary/point.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace constant_collection {
namespace stationary {

Point::Point(
    hdf5::Group const& parent,
    hdf5::Datatype const& memory_datatype)

    : constant_shape::same_shape::Constant{
        parent, coordinates_tag, memory_datatype}

{
}


Point create_point(
    hdf5::Group& parent,
    hdf5::Datatype const& memory_datatype,
    std::size_t const rank)
{
    return create_point(
        parent, file_datatype(memory_datatype), memory_datatype, rank);
}


Point create_point(
    hdf5::Group& parent,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    std::size_t const rank)
{
    // A point is defined by the coordinates along each dimension
    hdf5::Shape value_shape = { rank };

    constant_shape::same_shape::create_constant(
        parent, coordinates_tag, file_datatype, memory_datatype, value_shape);

    return Point{std::move(parent), memory_datatype};
}

}  // namespace stationary
}  // namespace constant_collection
}  // namespace lue
