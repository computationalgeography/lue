#include "lue/constant_size/time/located/shared/space_box.hpp"
#include "lue/tag.hpp"
#include <cassert>


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {

SpaceBox::SpaceBox(
    hdf5::Group const& parent,
    hdf5::Datatype const memory_datatype)

    : constant_shape::same_shape::Value{
            parent, coordinates_tag, memory_datatype}

{
}


SpaceBox::SpaceBox(
    constant_shape::same_shape::Value&& value)

    : constant_shape::same_shape::Value{
            std::forward<constant_shape::same_shape::Value>(value)}

{
}


/*!
    @brief      .
    @param      rank Dimensionality of the space domain
    @return     .
    @exception  .
*/
SpaceBox create_space_box(
    hdf5::Group const& parent,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    size_t const rank)
{
    // A box is defined by the coordinates of two opposite points
    // (diagonally). Two of them is enough.
    hdf5::Shape value_shape = { 2 * rank };

    auto value = constant_shape::same_shape::create_value(
        parent, coordinates_tag, file_datatype, memory_datatype, value_shape);

    return SpaceBox(std::move(value));
}

}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
