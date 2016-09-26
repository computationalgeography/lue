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

    assert(!other._id.is_valid());
}


Dataspace& Dataspace::operator=(
    Dataspace&& other)
{
    _id = std::move(other._id);

    // Invalidate other.
    std::move(other);

    assert(!other._id.is_valid());

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


Dataspace create_dataspace(
    std::vector<extent_t> const& dimension_sizes,
    std::vector<extent_t> const& max_dimension_sizes)
{
    assert(dimension_sizes.size() == max_dimension_sizes.size());

    Identifier dataspace_location(::H5Screate_simple(static_cast<int>(
        dimension_sizes.size()), dimension_sizes.data(),
        max_dimension_sizes.data()), H5Sclose);

    if(!dataspace_location.is_valid()) {
        throw std::runtime_error("Dataspace cannot be created");
    }

    return Dataspace(std::move(dataspace_location));
}

} // namespace hdf5
} // namespace lue
