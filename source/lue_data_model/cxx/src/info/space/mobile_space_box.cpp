#include "lue/info/space/mobile_space_box.hpp"
#include "lue/core/tag.hpp"


namespace lue {

MobileSpaceBox::MobileSpaceBox(
    hdf5::Group& parent):

    same_shape::constant_shape::Value{parent, coordinates_tag}

{
}


MobileSpaceBox::MobileSpaceBox(
    hdf5::Group& parent,
    hdf5::Datatype const& memory_datatype):

    same_shape::constant_shape::Value{
        parent, coordinates_tag, memory_datatype}

{
}


MobileSpaceBox::MobileSpaceBox(
    same_shape::constant_shape::Value&& value):

    same_shape::constant_shape::Value{
        std::forward<same_shape::constant_shape::Value>(value)}

{
}


Count MobileSpaceBox::nr_boxes() const
{
    return nr_arrays();
}


MobileSpaceBox create_mobile_space_box(
    hdf5::Group& parent,
    hdf5::Datatype const& memory_datatype,
    std::size_t const rank)
{
    return create_mobile_space_box(
        parent, file_datatype(memory_datatype), memory_datatype, rank);
}


MobileSpaceBox create_mobile_space_box(
    hdf5::Group& parent,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    std::size_t const rank)
{
    // A box is defined by the coordinates of two opposite points
    // (diagonally). Two of them is enough.
    hdf5::Shape value_shape{2 * rank};

    auto value = same_shape::constant_shape::create_value(
        parent, coordinates_tag, file_datatype, memory_datatype, value_shape);

    return MobileSpaceBox{std::move(value)};
}

}  // namespace lue
