#include "lue/array/same_shape/constant_shape/value.hpp"


namespace lue {
namespace data_model {
namespace same_shape {
namespace constant_shape {

/*!
    @brief      Open value @a name in @a parent
*/
Value::Value(
    hdf5::Group const& parent,
    std::string const& name):

    Array{parent, name}

{
}


/*!
    @brief      Open value @a name in @a parent
*/
Value::Value(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype):

    Array{parent, name, memory_datatype}

{
}


/*!
    @brief      Move in @a dataset
*/
Value::Value(
    Array&& array):

    Array{std::move(array)}

{
}


/*!
    @brief      Make space for an additional number of object arrays
    @param      nr_arrays Number of object arrays
*/
void Value::expand(
    Count const nr_arrays)
{
    // Get current shape of the underlying dataset, and update it for
    // the new size
    hdf5::Shape shape{this->shape()};
    shape[0] += nr_arrays;

    // Resize the dataset
    resize(shape);
}


/*!
    @brief      Return number of arrays for which space is reserved
*/
Count Value::nr_arrays() const
{
    return shape()[0];
}


/*!
    @brief      Return shape of each object array

    The shape returned is not the shape of the underlying HDF5 dataset. It
    is the shape of each of the individual object arrays.
*/
hdf5::Shape Value::array_shape() const
{
    hdf5::Shape const& shape{this->shape()};

    assert(!shape.empty());

    return hdf5::Shape{shape.begin() + 1, shape.end()};
}


Rank Value::rank() const
{
    return array_shape().size();
}


hdf5::Hyperslab Value::hyperslab(
    IndexRange const& range) const
{
    hdf5::Shape const& shape{this->shape()};

    hdf5::Offset offset(shape.size(), 0);
    offset[0] = range.begin();

    hdf5::Count count(shape.begin(), shape.end());
    count[0] = range.size();

    return hdf5::Hyperslab{offset, count};
}


hdf5::Hyperslab Value::hyperslab(
    Index const idx) const
{
    return hyperslab({idx, idx + 1});
}


void Value::read(
    Index const idx,
    void* buffer) const
{
    Array::read(hyperslab(idx), buffer);
}


void Value::read(
    IndexRange const& range,
    void* buffer) const
{
    Array::read(hyperslab(range), buffer);
}


void Value::write(
    Index const idx,
    void const* buffer)
{
    Array::write(hyperslab(idx), buffer);
}


void Value::write(
    IndexRange const& range,
    void const* buffer)
{
    Array::write(hyperslab(range), buffer);
}


/*!
    @brief      Create value @a name in @a parent
*/
Value create_value(
    hdf5::Group& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype)
{
    return create_value(parent, name, file_datatype(memory_datatype), memory_datatype, hdf5::Shape{});
}


/*!
    @brief      Create value @a name in @a parent
*/
Value create_value(
    hdf5::Group& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype,
    hdf5::Shape const& array_shape)
{
    return create_value(parent, name, file_datatype(memory_datatype), memory_datatype, array_shape);
}


/*!
    @brief      Create value @a name in @a parent
*/
Value create_value(
    hdf5::Group& parent,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype)
{
    return create_value(parent, name, file_datatype, memory_datatype, hdf5::Shape{});
}


/*!
    @brief      Create value @a name in @a parent

    The @a array_shape passed in is the shape of each of the individual
    object arrays.

    The underlying HDF5 dataset is chunked according to hdf5::chunk_shape().
*/
Value create_value(
    hdf5::Group& parent,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    hdf5::Shape const& array_shape)
{
    // The rank of the underlying dataset is one larger than the rank of the
    // object arrays. Object arrays are stored one after the other.
    hdf5::Shape dimension_sizes{array_shape};
    dimension_sizes.insert(dimension_sizes.begin(), 0);

    hdf5::Shape max_dimension_sizes{array_shape};
    max_dimension_sizes.insert(max_dimension_sizes.begin(), H5S_UNLIMITED);

    hdf5::Dataspace const dataspace{hdf5::create_dataspace(dimension_sizes, max_dimension_sizes)};

    hdf5::Dataset::CreationPropertyList creation_property_list;
    hdf5::Shape const chunk_dimension_sizes{hdf5::chunk_shape(array_shape, file_datatype.size())};
    creation_property_list.set_chunk(chunk_dimension_sizes);

    hdf5::Dataset dataset{
        hdf5::create_dataset(parent.id(), name, file_datatype, dataspace, creation_property_list)};

    return Value{Array{std::move(dataset), memory_datatype}};
}


bool value_exists(
    hdf5::Group const& parent,
    std::string const& name)
{
    return hdf5::dataset_exists(parent.id(), name);
}

}  // namespace constant_shape
}  // namespace same_shape
}  // namespace data_model
}  // namespace lue
