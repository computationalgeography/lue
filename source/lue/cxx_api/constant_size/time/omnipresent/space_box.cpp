#include "lue/cxx_api/constant_size/time/omnipresent/space_box.h"
#include <cassert>


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

SpaceBox::SpaceBox(
    hdf5::Identifier const& location)

    : same_shape::Item(location, "coordinates")

{
}


SpaceBox::SpaceBox(
    hdf5::Identifier const& location,
    hid_t const type_id)

    : same_shape::Item(location, "coordinates", type_id)

{
}


SpaceBox::SpaceBox(
    same_shape::Item&& coordinates)

    : same_shape::Item(std::forward<same_shape::Item>(coordinates))

{
}


/*!
    @ingroup    lue_cxx_api_group
*/
SpaceBox create_space_box(
    hdf5::Identifier const& location,
    hid_t const file_type_id,
    hid_t const memory_type_id,
    size_t rank)
{
    std::string const name = "coordinates";
    // A box is defined by the coordinates of two opposite points
    // (diagonally).
    // Shape shape = { rank * static_cast<hsize_t>(std::pow(2, rank)) };
    Shape shape = { 2 * rank };
    Chunks chunks = { shape[0] };

    auto item = same_shape::create_item(location, name, file_type_id,
        memory_type_id, shape, chunks);

    return SpaceBox(std::move(item));
}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
