#include "lue/cxx_api/array.h"
#include <algorithm>
#include <cassert>


namespace lue {

bool array_exists(
    hdf5::Identifier const& location,
    std::string const& name)
{
    return hdf5::dataset_exists(location, name);
}


Array::Array(
    hdf5::Identifier&& location,
    hid_t const type_id)

    : Dataset(std::forward<hdf5::Identifier>(location)),
      _type_id{type_id}

{
}


Array::Array(
    hdf5::Dataset&& dataset,
    hid_t const type_id)

    : Dataset(std::forward<hdf5::Dataset>(dataset)),
      _type_id{type_id}

{
}


Shape Array::shape() const
{
    auto const dimension_extents = dataspace().dimension_extents();

    Shape shape(dimension_extents.size());

    std::copy(dimension_extents.begin(), dimension_extents.end(),
        shape.begin());

    return shape;
}


void Array::read(
    std::vector<hsize_t> const& start,
    std::vector<hsize_t> const& count,
    std::vector<hsize_t> const& stride,
    void* buffer) const
{
    Dataset::read(_type_id, start, count, stride, buffer);
}


void Array::write(
    std::vector<hsize_t> const& start,
    std::vector<hsize_t> const& count,
    std::vector<hsize_t> const& stride,
    void const* buffer)
{
    Dataset::write(_type_id, start, count, stride, buffer);
}


// Array create_array(
//     hdf5::Identifier const& location,
//     std::string const& name,
//     hid_t const datatype,
//     hid_t const dataspace)
// {
//     return Array(std::move(create_dataset(location, name, datatype,
//         dataspace)));
// }


// Array open_array(
//     hdf5::Identifier const& location,
//     std::string const& name)
// {
//     return Array(std::move(open_dataset(location, name)));
// }

} // namespace lue
