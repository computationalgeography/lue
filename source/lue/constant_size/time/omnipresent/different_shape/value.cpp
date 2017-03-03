#include "lue/constant_size/time/omnipresent/different_shape/value.h"
#include "lue/tag.h"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {
namespace different_shape {

// // Value::Value(
// //     hdf5::Identifier const& location,
// //     std::string const& name)
// // 
// //     : Array(location, name)
// // 
// // {
// // }
// 
// 
// // Value::Value(
// //     hdf5::Identifier const& location,
// //     std::string const& name,
// //     hid_t const type_id)
// // 
// //     : // omnipresent::Value(),
// //       Array(open_array(location, name, type_id))
// // 
// // {
// // }


Value::Value(
    hdf5::Identifier const& location,
    std::string const& name,
    hdf5::Datatype const& memory_datatype)

    : Group(location, name),
      _nr_items{attributes().read<hsize_t>(nr_items_tag)},
      _rank{attributes().read<int>(rank_tag)},
      _file_datatype{hdf5::decode_datatype(
          attributes().read<std::vector<unsigned char>>(datatype_tag))},
      _memory_datatype{memory_datatype}

{
}


Value::Value(
    hdf5::Group&& group,
    hdf5::Datatype const& memory_datatype)

    : Group(std::forward<hdf5::Group>(group)),
      _nr_items{attributes().read<hsize_t>(nr_items_tag)},
      _rank{attributes().read<int>(rank_tag)},
      _file_datatype{hdf5::decode_datatype(
          attributes().read<std::vector<unsigned char>>(datatype_tag))},
      _memory_datatype{memory_datatype}

{
}


// // 
// // 
// // Value::Value(
// //     hdf5::Dataset&& dataset,
// //     hid_t const type_id)
// // 
// //     : // omnipresent::Value(),
// //       Array(std::forward<hdf5::Dataset>(dataset), type_id)
// // 
// // {
// // }


hsize_t Value::nr_items() const
{
    return _nr_items;
}


int Value::rank() const
{
    return _rank;
}


hdf5::Datatype const& Value::file_datatype() const
{
    return _file_datatype;
}


hdf5::Datatype const& Value::memory_datatype() const
{
    return _memory_datatype;
}


void Value::reserve_value(
    hsize_t const idx,
    hsize_t const* shape)
{
    // Create dataset with this shape.
    std::string name = std::to_string(idx);

    hdf5::Shape dimension_sizes(shape, shape + _rank);
    hdf5::Shape max_dimension_sizes(dimension_sizes);

    auto dataspace = hdf5::create_dataspace(dimension_sizes,
        max_dimension_sizes);

    // No chunking for now...
    auto creation_property_list_location = H5P_DEFAULT;
    auto dataset = hdf5::create_dataset(id(), name, _file_datatype,
        dataspace, creation_property_list_location);
}


void Value::reserve(
    hsize_t const nr_items,
    hsize_t const* shapes)
{
    for(hsize_t i = 0; i < nr_items; ++i) {
        reserve_value(i, &(shapes[i * _rank]));
    }

    _nr_items = nr_items;

    attributes().write<hsize_t>(nr_items_tag, nr_items);
}


Array Value::operator[](
    size_t const idx) const
{
    return Array(id(), std::to_string(idx), _memory_datatype);
}


Value create_value(
    hdf5::Identifier const& location,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    int const rank)
{

    auto group = hdf5::create_group(location, name);

    group.attributes().write<std::vector<unsigned char>>(datatype_tag,
        hdf5::encode_datatype(file_datatype));
    group.attributes().write<int>(rank_tag, rank);
    group.attributes().write<hsize_t>(nr_items_tag, 0);

    return Value(std::move(group), memory_datatype);






    // hdf5::Shape dimension_sizes(value_shape);
    // dimension_sizes.insert(dimension_sizes.begin(), 0);

    // hdf5::Shape max_dimension_sizes(value_shape);
    // max_dimension_sizes.insert(max_dimension_sizes.begin(), H5S_UNLIMITED);

    // auto dataspace = hdf5::create_dataspace(dimension_sizes,
    //     max_dimension_sizes);

    // hdf5::Identifier creation_property_list_location(::H5Pcreate(
    //     H5P_DATASET_CREATE), ::H5Pclose);

    // if(!creation_property_list_location.is_valid()) {
    //     throw std::runtime_error("Creation property list cannot be created");
    // }


    // hdf5::Shape chunk_dimension_sizes(value_chunk);
    // chunk_dimension_sizes.insert(chunk_dimension_sizes.begin(), 1000);

    // auto status = ::H5Pset_chunk(creation_property_list_location,
    //     chunk_dimension_sizes.size(), chunk_dimension_sizes.data());
    // assert(status >= 0);

    // auto dataset = hdf5::create_dataset(location, name, file_datatype,
    //     dataspace, creation_property_list_location);

    // return Value(std::move(dataset), memory_datatype);
}


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

}  // namespace different_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
