#include "lue/hdf5/dataspace.hpp"
#include <cassert>


namespace lue {
namespace hdf5 {

/*!
    @brief      Construct an instance of a particular @a type
    @param      type Dataspace type
    @exception  std::runtime_error In case the dataspace cannot be created
    @sa         [H5Screate](https://support.hdfgroup.org/HDF5/doc/RM/RM_H5S.html#Dataspace-Create)
*/
Dataspace::Dataspace(
    ::H5S_class_t const type)

    : _id(::H5Screate(type), H5Sclose)

{
    if(!_id.is_valid()) {
        throw std::runtime_error("Cannot create dataspace");
    }
}


/*!
    @brief      Create an instance based on a dataspace identifier
    @param      id Identifier of created dataspace
    @exception  std::runtime_error In case the @a id is not valid
*/
Dataspace::Dataspace(
    hid_t const id)

    : _id(id, ::H5Sclose)

{
    if(!_id.is_valid()) {
        throw std::runtime_error("Dataspace is not valid");
    }
}


/*!
    @brief      Create an instance based on a dataspace identifier
    @param      id Identifier of created dataspace
    @exception  std::runtime_error In case the @a id is not valid
*/
Dataspace::Dataspace(
    Identifier&& id)

    : _id{std::forward<Identifier>(id)}

{
    if(!_id.is_valid()) {
        throw std::runtime_error("Dataspace is not valid");
    }
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


/*!
    @brief      Return the identifier
*/
Identifier const& Dataspace::id() const
{
    return _id;
}


/*!
    @brief      Return the number of dimensions
    @exception  std::runtime_error In case the number of dimensions cannot
                be determined
*/
int Dataspace::nr_dimensions() const
{
    auto const nr_dimensions = ::H5Sget_simple_extent_ndims(_id);

    if(nr_dimensions < 0) {
        throw std::runtime_error(
            "Cannot determine number of dataspace dimensions");
    }

    return nr_dimensions;
}


/*!
    @brief      Return the dimension extents
    @exception  std::runtime_error In case the dimension extents cannot
                be determined
*/
Shape Dataspace::dimension_extents() const
{
    auto const nr_dimensions = this->nr_dimensions();
    auto extents = std::make_unique<hsize_t[]>(nr_dimensions);
    hsize_t* max_extents = nullptr;

    auto const nr_dimensions2 = ::H5Sget_simple_extent_dims(_id,
        extents.get(), max_extents);

    if(nr_dimensions2 < 0) {
        throw std::runtime_error("Cannot retrieve dataspace extents");
    }

    assert(nr_dimensions2 == nr_dimensions);

    return Shape(extents.get(), extents.get() + nr_dimensions);
}


/*!
    @brief      Create a new simple dataspace instance
    @param      dimension_sizes Size of each dimension. This value will
                also be used as the upper limit on the size of each
                dimension
    @sa         create_dataspace(Shape const&, Shape const&)
*/
Dataspace create_dataspace(
    Shape const& dimension_sizes)
{
    return create_dataspace(dimension_sizes, dimension_sizes);
}


/*!
    @brief      Create a new simple dataspace instance
    @param      dimension_sizes Size of each dimension
    @param      max_dimension_sizes Upper limit on the size of each dimension
    @exception  std::runtime_error In case the dataspace cannot be
                created
    @sa         [H5Screate_simple](https://support.hdfgroup.org/HDF5/doc/RM/RM_H5S.html#Dataspace-CreateSimple)
*/
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
