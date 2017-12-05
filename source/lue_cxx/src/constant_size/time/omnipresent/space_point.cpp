#include "lue/constant_size/time/omnipresent/space_point.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

SpacePoint::SpacePoint(
    hdf5::Group const& parent,
    hdf5::Datatype const memory_datatype)

    : constant::same_shape::Collection{parent, coordinates_tag, memory_datatype}

{
}


SpacePoint::SpacePoint(
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
SpacePoint create_space_point(
    hdf5::Group const& parent,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    size_t const rank)
{
    // A point is defined by the coordinates along each dimension
    hdf5::Shape value_shape = { rank };

    auto collection = constant::same_shape::create_collection(
        parent, coordinates_tag, file_datatype, memory_datatype, value_shape);

    return SpacePoint{std::move(collection)};
}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
