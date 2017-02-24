#include "lue/cxx_api/space/stationary/space_box.h"
#include <cassert>


namespace lue {
namespace space {
namespace stationary {

SpaceBox::SpaceBox(
    hdf5::Identifier const& location)

    : constant_size::time::omnipresent::same_shape::Item(location,
        "coordinates")

{
}


SpaceBox::SpaceBox(
    hdf5::Identifier const& location,
    hid_t const type_id)

    : constant_size::time::omnipresent::same_shape::Item(location,
        "coordinates", type_id)

{
}


SpaceBox::SpaceBox(
    constant_size::time::omnipresent::same_shape::Item&& coordinates)

    : constant_size::time::omnipresent::same_shape::Item(
        std::forward<constant_size::time::omnipresent::same_shape::Item>(
            coordinates))

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

    auto item = constant_size::time::omnipresent::same_shape::create_item(
        location, name, file_type_id, memory_type_id, shape, chunks);

    return SpaceBox(std::move(item));
}

}  // namespace stationary
}  // namespace space
}  // namespace lue
