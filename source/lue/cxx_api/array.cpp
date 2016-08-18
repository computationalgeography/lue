#include "lue/cxx_api/array.h"
#include <algorithm>
// #include <iostream>


namespace lue {

Array::Array(
    hdf5::Identifier&& location,
    hid_t type_id)

    : _dataset{std::forward<hdf5::Identifier>(location)},
      _type_id{type_id}

{
}


hid_t Array::type_id() const
{
    return _type_id;
}


hdf5::Identifier const& Array::dataset_id() const
{
    return _dataset.id();
}


Shape Array::shape() const
{
    auto const dimension_extents = _dataset.dataspace().dimension_extents();

    Shape shape(dimension_extents.size());

    std::copy(dimension_extents.begin(), dimension_extents.end(),
        shape.begin());

    return shape;
}


void Array::read(
    hsize_t const start,
    hsize_t const count,
    hsize_t const stride,
    void* buffer) const
{
    _dataset.read(_type_id, start, count, stride, buffer);
}


void Array::write(
    hsize_t const start,
    hsize_t const count,
    hsize_t const stride,
    void const* buffer)
{
    _dataset.write(_type_id, start, count, stride, buffer);
}

} // namespace lue
