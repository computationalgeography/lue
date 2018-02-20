#include "lue/space/mobile/asynchronous_box.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace mobile {

AsynchronousBox::AsynchronousBox(
    hdf5::Group const& parent,
    hdf5::Datatype const& memory_datatype)

    : constant_shape::same_shape::AsynchronousValue{
        parent, coordinates_tag, memory_datatype}

{
}


AsynchronousBox create_asynchronous_box(
    hdf5::Group& parent,
    hdf5::Datatype const& memory_datatype,
    std::size_t const rank)
{
    return create_asynchronous_box(
        parent, file_datatype(memory_datatype), memory_datatype, rank);
}


AsynchronousBox create_asynchronous_box(
    hdf5::Group& parent,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    std::size_t const rank)
{
    // A box is defined by the coordinates of two opposite points
    // (diagonally). Two of them is enough.
    hdf5::Shape value_shape{2 * rank};

    constant_shape::same_shape::create_asynchronous_value(
        parent, coordinates_tag, file_datatype, memory_datatype, value_shape);

    return AsynchronousBox{std::move(parent), memory_datatype};
}

}  // namespace mobile
}  // namespace lue
