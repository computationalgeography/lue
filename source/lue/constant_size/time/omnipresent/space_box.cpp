#include "lue/constant_size/time/omnipresent/space_box.h"
#include "lue/tag.h"
#include <cassert>


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

// SpaceBox::SpaceBox(
//     hdf5::Identifier const& location)
// 
//     : same_shape::Value(location, "coordinates")
// 
// {
// }


SpaceBox::SpaceBox(
    hdf5::Identifier const& location,
    hdf5::Datatype const memory_datatype)

    : same_shape::Value(location, coordinates_tag, memory_datatype)

{
}


SpaceBox::SpaceBox(
    same_shape::Value&& value)

    : same_shape::Value(std::forward<same_shape::Value>(value))

{
}


/*!
    @brief      .
    @param      rank Dimensionality of the space domain
    @return     .
    @exception  .
*/
SpaceBox create_space_box(
    hdf5::Group const& group,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    size_t const rank)
{
    // A box is defined by the coordinates of two opposite points
    // (diagonally). Two of them is enough.
    hdf5::Shape value_shape = { 2 * rank };

    auto value = same_shape::create_value(group, coordinates_tag,
        file_datatype, memory_datatype, value_shape);

    return SpaceBox(std::move(value));
}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
