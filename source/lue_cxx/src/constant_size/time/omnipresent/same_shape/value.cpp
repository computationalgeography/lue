#include "lue/constant_size/time/omnipresent/same_shape/value.hpp"
#include "lue/hdf5/chunk.hpp"
#include <cassert>


namespace lue {
// namespace {
// 
// hdf5::Shape value_chunk(
//     hdf5::Shape const& value_shape)
// {
// }
// 
// }  // Anonymous namespace


namespace constant_size {
namespace time {
namespace omnipresent {
namespace same_shape {

// Value::Value(
//     hdf5::Identifier const& location,
//     std::string const& name)
// 
//     : Array(location, name)
// 
// {
// }


// Value::Value(
//     hdf5::Identifier const& location,
//     std::string const& name,
//     hid_t const type_id)
// 
//     : // omnipresent::Value(),
//       Array(open_array(location, name, type_id))
// 
// {
// }


Value::Value(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype)

    : Array{parent, name, memory_datatype},
      omnipresent::Value{}

{
}


Value::Value(
    hdf5::Dataset&& dataset,
    hdf5::Datatype const& memory_datatype)

    : Array{std::forward<hdf5::Dataset>(dataset), memory_datatype},
      omnipresent::Value{}

{
}


// 
// 
// Value::Value(
//     hdf5::Dataset&& dataset,
//     hid_t const type_id)
// 
//     : // omnipresent::Value(),
//       Array(std::forward<hdf5::Dataset>(dataset), type_id)
// 
// {
// }


void Value::reserve(
    hsize_t const nr_items)
{
    auto shape = this->shape();
    shape[0] = nr_items;

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

    return hdf5::Shape(shape.begin() + 1, shape.end());
}


// bool value_exists(
//     hdf5::Group const& group,
//     std::string const& name)
// {
//     return hdf5::dataset_exists(group.id(), name);
// }


Value create_value(
    hdf5::Group const& group,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype)
{
    return create_value(group, name, file_datatype, memory_datatype,
        hdf5::Shape{});
}


Value create_value(
    hdf5::Group const& group,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    hdf5::Shape const& value_shape)
{
    hdf5::Shape dimension_sizes(value_shape);
    dimension_sizes.insert(dimension_sizes.begin(), 0);

    hdf5::Shape max_dimension_sizes(value_shape);
    max_dimension_sizes.insert(max_dimension_sizes.begin(), H5S_UNLIMITED);

    auto dataspace = hdf5::create_dataspace(dimension_sizes,
        max_dimension_sizes);

    hdf5::Dataset::CreationPropertyList creation_property_list;
    auto chunk_dimension_sizes =
        chunk_shape(value_shape, file_datatype.size());
    // hdf5::Shape chunk_dimension_sizes(value_chunk);
    // chunk_dimension_sizes.insert(chunk_dimension_sizes.begin(), 1000);
    creation_property_list.set_chunk(chunk_dimension_sizes);

    auto dataset = hdf5::create_dataset(group.id(), name, file_datatype,
        dataspace, creation_property_list);

    return Value(std::move(dataset), memory_datatype);
}


// Value create_value(
//     hdf5::Group const& group,
//     std::string const& name,
//     hdf5::Datatype const& file_datatype,
//     hdf5::Datatype const& memory_datatype,
//     hdf5::Shape const& value_shape,
//     hdf5::Shape const& value_chunk)
// {
//     // if(array_exists(location, name)) {
//     //     throw std::runtime_error("Value dataset " + name + " already exists");
//     // }
// 
//     hdf5::Shape dimension_sizes(value_shape);
//     dimension_sizes.insert(dimension_sizes.begin(), 0);
// 
//     hdf5::Shape max_dimension_sizes(value_shape);
//     max_dimension_sizes.insert(max_dimension_sizes.begin(), H5S_UNLIMITED);
// 
//     auto dataspace = hdf5::create_dataspace(dimension_sizes,
//         max_dimension_sizes);
// 
//     hdf5::Dataset::CreationPropertyList creation_property_list;
//     hdf5::Shape chunk_dimension_sizes(value_chunk);
//     chunk_dimension_sizes.insert(chunk_dimension_sizes.begin(), 1000);
//     creation_property_list.set_chunk(chunk_dimension_sizes);
// 
//     auto dataset = hdf5::create_dataset(group.id(), name, file_datatype,
//         dataspace, creation_property_list);
// 
//     return Value(std::move(dataset), memory_datatype);
// }


// Value create_value(
//     hdf5::Identifier const& location,
//     std::string const& name,
//     hid_t const file_type_id,
//     hid_t const memory_type_id)
// {
//     return create_value(location, name, file_type_id, memory_type_id,
//         Shape{}, Shape{});
// }
// 
// 
// Value create_value(
//     hdf5::Identifier const& location,
//     std::string const& name,
//     hid_t const file_type_id,
//     hid_t const memory_type_id,
//     Shape const& shape,
//     Shape const& chunks)
// {
//     if(array_exists(location, name)) {
//         throw std::runtime_error("Value dataset " + name + " already exists");
//     }
// 
//     Shape dimension_sizes(shape);
//     dimension_sizes.insert(dimension_sizes.begin(), 0);
// 
//     Shape max_dimension_sizes(shape);
//     max_dimension_sizes.insert(max_dimension_sizes.begin(), H5S_UNLIMITED);
// 
//     auto dataspace = hdf5::create_dataspace(dimension_sizes,
//         max_dimension_sizes);
// 
//     hdf5::Identifier creation_property_list_location(::H5Pcreate(
//         H5P_DATASET_CREATE), ::H5Pclose);
// 
//     if(!creation_property_list_location.is_valid()) {
//         throw std::runtime_error("Creation property list cannot be created");
//     }
// 
// 
//     Shape chunk_dimension_sizes(chunks);
//     chunk_dimension_sizes.insert(chunk_dimension_sizes.begin(), 1000);
// 
//     herr_t status = ::H5Pset_chunk(creation_property_list_location,
//         chunk_dimension_sizes.size(), chunk_dimension_sizes.data());
//     assert(status >= 0);
// 
//     auto dataset = hdf5::create_dataset(location, name, file_type_id,
//         dataspace, creation_property_list_location);
// 
//     return Value(std::move(dataset), memory_type_id);
// }

}  // namespace same_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
