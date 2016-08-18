#include "lue/cxx_api/hdf5/dataspace.h"
#include <cassert>


namespace lue {
namespace hdf5 {

Dataspace::Dataspace(
    Identifier&& id)

    : _id{std::forward<Identifier>(id)}

{
}


Dataspace::Dataspace(
    Dataspace&& other)

    : _id{std::move(other._id)}

{
    // Invalidate other.
    std::move(other);
}


Dataspace& Dataspace::operator=(
    Dataspace&& other)
{
    _id = std::move(other._id);

    // Invalidate other.
    std::move(other);

    return *this;
}


Identifier const& Dataspace::id() const
{
    return _id;
}


int Dataspace::nr_dimensions() const
{
    int nr_dimensions{H5Sget_simple_extent_ndims(_id)};

    if(nr_dimensions < 0) {
        throw std::runtime_error(
            "Cannot obtain number of dataspace dimensions");
    }

    return nr_dimensions;
}



std::vector<hsize_t> Dataspace::dimension_extents() const
{
    int const nr_dimensions{this->nr_dimensions()};
    hsize_t extents[nr_dimensions];
    hsize_t* max_extents = nullptr;

    int nr_dimensions2{H5Sget_simple_extent_dims(_id, extents, max_extents)};

    if(nr_dimensions2 < 0) {
        throw std::runtime_error("Cannot obtain number of dataspace extents");
    }

    return std::vector<hsize_t>(extents, extents + nr_dimensions);
}

} // namespace hdf5
} // namespace lue
