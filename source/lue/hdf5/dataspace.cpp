#include "lue/hdf5/dataspace.h"
#include <cassert>


namespace lue {
namespace hdf5 {

Dataspace::Dataspace(
    ::H5S_class_t const type)

    : _id(::H5Screate(type), H5Sclose)

{
    if(!_id.is_valid()) {
        throw std::runtime_error("Cannot create dataspace");
    }
}


Dataspace::Dataspace(
    hid_t const id)

    : _id(id, ::H5Sclose)

{
    if(!_id.is_valid()) {
        throw std::runtime_error("Dataspace is not valid");
    }
}


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
    auto const nr_dimensions{::H5Sget_simple_extent_ndims(_id)};

    if(nr_dimensions < 0) {
        throw std::runtime_error(
            "Cannot determine number of dataspace dimensions");
    }

    return nr_dimensions;
}


Shape Dataspace::dimension_extents() const
{
    auto const nr_dimensions{this->nr_dimensions()};
    auto extents = std::make_unique<hsize_t[]>(nr_dimensions);
    hsize_t* max_extents = nullptr;

    auto const nr_dimensions2{::H5Sget_simple_extent_dims(_id,
        extents.get(), max_extents)};

    if(nr_dimensions2 < 0) {
        throw std::runtime_error("Cannot retrieve dataspace extents");
    }

    return Shape(extents.get(), extents.get() + nr_dimensions);
}


Dataspace create_dataspace(
    Shape const& dimension_sizes)
{
    return create_dataspace(dimension_sizes, dimension_sizes);
}


Dataspace create_dataspace(
    Shape const& dimension_sizes,
    Shape const& max_dimension_sizes)
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
