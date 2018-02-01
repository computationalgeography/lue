#include "lue/space/constant_collection/mobile/synchronous_box.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace constant_collection {
namespace mobile {

SynchronousBox::SynchronousBox(
    hdf5::Group const& parent,
    hdf5::Datatype const& memory_datatype)

    : constant_shape::same_shape::SynchronousVariable{
        parent, coordinates_tag, memory_datatype}

{
}


SynchronousBox create_synchronous_box(
    hdf5::Group& parent,
    hdf5::Datatype const& memory_datatype,
    std::size_t const rank)
{
    return create_synchronous_box(
        parent, file_datatype(memory_datatype), memory_datatype, rank);
}


SynchronousBox create_synchronous_box(
    hdf5::Group& parent,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    std::size_t const rank)
{
    // A box is defined by the coordinates of two opposite points
    // (diagonally). Two of them is enough.
    hdf5::Shape value_shape{2 * rank};

    constant_shape::same_shape::create_synchronous_variable(
        parent, coordinates_tag, file_datatype, memory_datatype, value_shape);

    return SynchronousBox{std::move(parent), memory_datatype};
}

}  // namespace mobile
}  // namespace constant_collection
}  // namespace lue
