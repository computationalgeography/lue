#include "lue/space/mobile/asynchronous_point.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace mobile {

AsynchronousPoint::AsynchronousPoint(
    hdf5::Group const& parent,
    hdf5::Datatype const& memory_datatype)

    : same_shape::constant_shape::AsynchronousValue{
        parent, coordinates_tag, memory_datatype}

{
}


AsynchronousPoint create_asynchronous_point(
    hdf5::Group& parent,
    hdf5::Datatype const& memory_datatype,
    std::size_t const rank)
{
    return create_asynchronous_point(
        parent, file_datatype(memory_datatype), memory_datatype, rank);
}


AsynchronousPoint create_asynchronous_point(
    hdf5::Group& parent,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    std::size_t const rank)
{
    // A point is defined by the coordinates along each dimension
    hdf5::Shape value_shape{rank};

    same_shape::constant_shape::create_asynchronous_value(
        parent, coordinates_tag, file_datatype, memory_datatype, value_shape);

    return AsynchronousPoint{std::move(parent), memory_datatype};
}

}  // namespace mobile
}  // namespace lue
