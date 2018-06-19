#include "lue/info/space/mobile_space_point.hpp"
#include "lue/core/tag.hpp"


namespace lue {

MobileSpacePoint::MobileSpacePoint(
    hdf5::Group const& parent,
    hdf5::Datatype const& memory_datatype):

    same_shape::constant_shape::Value{
        parent, coordinates_tag, memory_datatype}

{
}


MobileSpacePoint::MobileSpacePoint(
    same_shape::constant_shape::Value&& value):

    same_shape::constant_shape::Value{
        std::forward<same_shape::constant_shape::Value>(value)}

{
}


MobileSpacePoint create_mobile_space_point(
    hdf5::Group& parent,
    hdf5::Datatype const& memory_datatype,
    std::size_t const rank)
{
    return create_mobile_space_point(
        parent, file_datatype(memory_datatype), memory_datatype, rank);
}


MobileSpacePoint create_mobile_space_point(
    hdf5::Group& parent,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    std::size_t const rank)
{
    // A point is defined by the coordinates along each dimension
    hdf5::Shape value_shape{rank};

    auto value = same_shape::constant_shape::create_value(
        parent, coordinates_tag, file_datatype, memory_datatype, value_shape);

    return MobileSpacePoint{std::move(value)};
}

}  // namespace lue
