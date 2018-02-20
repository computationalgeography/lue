#include "lue/space/stationary/box.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace stationary {

Box::Box(
    hdf5::Group const& parent,
    hdf5::Datatype const& memory_datatype)

    : same_shape::constant_shape::ContinuousValue{
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
    hdf5::Shape value_shape = {2 * rank};

    same_shape::constant_shape::create_continuous_value(
        parent, coordinates_tag, file_datatype, memory_datatype, value_shape);

    return Box{std::move(parent), memory_datatype};
}

}  // namespace lue
}  // namespace stationary
