#include "lue/space/mobile/constant_collection/synchronous_box.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace mobile {
namespace constant_collection {

SynchronousBox::SynchronousBox(
    hdf5::Group const& parent,
    hdf5::Datatype const& memory_datatype)

    : same_shape::constant_shape::constant_collection::SynchronousValue{
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

    same_shape::constant_shape::constant_collection::create_synchronous_value(
        parent, coordinates_tag, file_datatype, memory_datatype, value_shape);

    return SynchronousBox{std::move(parent), memory_datatype};
}

}  // namespace constant_collection
}  // namespace mobile
}  // namespace lue
