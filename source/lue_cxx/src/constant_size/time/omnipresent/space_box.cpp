#include "lue/constant_size/time/omnipresent/space_box.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

SpaceBox::SpaceBox(
    hdf5::Group const& parent,
    hdf5::Datatype const memory_datatype)

    : constant::same_shape::Collection{parent, coordinates_tag, memory_datatype}

{
}


SpaceBox::SpaceBox(
    constant::same_shape::Collection&& collection)

    : constant::same_shape::Collection{
          std::forward<constant::same_shape::Collection>(collection)}

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

    auto collection = constant::same_shape::create_collection(
        parent, coordinates_tag, file_datatype, memory_datatype, value_shape);

    return SpaceBox{std::move(collection)};
}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
