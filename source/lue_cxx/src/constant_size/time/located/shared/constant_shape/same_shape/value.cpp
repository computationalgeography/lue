#include "lue/constant_size/time/located/shared/constant_shape/same_shape/value.hpp"
#include "lue/hdf5/chunk.hpp"
#include <cassert>


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {
namespace constant_shape {
namespace same_shape {

Value::Value(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype)

    : Array(parent, name, memory_datatype),
      constant_size::Value()

{
}


Value::Value(
    hdf5::Dataset&& dataset,
    hdf5::Datatype const& memory_datatype)

    : Array(std::forward<hdf5::Dataset>(dataset), memory_datatype),
      constant_size::Value()

{
}


void Value::reserve(
    hsize_t const nr_time_domain_items,
    hsize_t const nr_items)
{
    auto shape = this->shape();
    shape[0] = nr_time_domain_items;
    shape[1] = nr_items;

    resize(shape);
}


hsize_t Value::nr_items() const
{
    return shape()[0];
}


hdf5::Shape Value::value_shape() const
{
    auto const shape = this->shape();

    assert(shape.begin() != shape.end());
    assert(shape.begin() + 1 != shape.end());

    return hdf5::Shape(shape.begin() + 2, shape.end());
}


Value create_value(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype)
{
    return create_value(parent, name, file_datatype, memory_datatype,
        hdf5::Shape{});
}


Value create_value(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    hdf5::Shape const& value_shape)
{
    hdf5::Shape dimension_sizes(value_shape);
    dimension_sizes.insert(dimension_sizes.begin(), 2, 0);

    hdf5::Shape max_dimension_sizes(value_shape);
    max_dimension_sizes.insert(max_dimension_sizes.begin(), 2, H5S_UNLIMITED);

    auto dataspace = hdf5::create_dataspace(
        dimension_sizes, max_dimension_sizes);

    hdf5::Dataset::CreationPropertyList creation_property_list;
    auto chunk_dimension_sizes = chunk_shape(
        value_shape, file_datatype.size());
    chunk_dimension_sizes.insert(chunk_dimension_sizes.begin(), 1);
    creation_property_list.set_chunk(chunk_dimension_sizes);

    auto dataset = hdf5::create_dataset(
        parent.id(), name, file_datatype, dataspace, creation_property_list);

    return Value(std::move(dataset), memory_datatype);
}

}  // namespace same_shape
}  // namespace constant_shape
}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
