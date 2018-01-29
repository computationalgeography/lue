#include "lue/space/constant_collection/stationary/box.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace constant_collection {
namespace stationary {

Box::Box(
    hdf5::Group const& parent,
    hdf5::Datatype const& memory_datatype)

    : constant_shape::same_shape::Constant{
        parent, coordinates_tag, memory_datatype}

{
}


Box create_box(
    hdf5::Group& parent,
    hdf5::Datatype const& memory_datatype,
    std::size_t const rank)
{
    return create_box(
        parent, file_datatype(memory_datatype), memory_datatype, rank);
}


Box create_box(
    hdf5::Group& parent,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    std::size_t const rank)
{
    // A box is defined by the coordinates of two opposite points
    // (diagonally). Two of them is enough.
    hdf5::Shape value_shape = {2 * rank };

    constant_shape::same_shape::create_constant(
        parent, coordinates_tag, file_datatype, memory_datatype, value_shape);

    return Box{std::move(parent), memory_datatype};
}

}  // namespace lue
}  // namespace constant_collection
}  // namespace stationary
