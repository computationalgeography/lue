#include "lue/constant_size/time/omnipresent/space_point.hpp"
#include "lue/tag.hpp"
#include <cassert>


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

SpacePoint::SpacePoint(
    hdf5::Identifier const& location,
    hdf5::Datatype const memory_datatype)

    : same_shape::Value(location, coordinates_tag, memory_datatype)

{
}


SpacePoint::SpacePoint(
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
SpacePoint create_space_point(
    hdf5::Group const& group,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    size_t const rank)
{
    // A point is defined by the coordinates along each dimension
    hdf5::Shape value_shape = { rank };

    auto value = same_shape::create_value(group, coordinates_tag,
        file_datatype, memory_datatype, value_shape);

    return SpacePoint(std::move(value));
}

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
