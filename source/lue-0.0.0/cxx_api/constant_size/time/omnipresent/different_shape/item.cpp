#include "lue/cxx_api/constant_size/time/omnipresent/different_shape/item.h"
// #include "lue/cxx_api/array.h"
#include "lue/cxx_api/hdf5/datatype.h"
#include <cassert>
#include <iostream>


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {
namespace different_shape {


Item::Item(
    hdf5::Group&& group)

    : hdf5::Group(std::forward<hdf5::Group>(group)),
      _type_id{hdf5::parse_standard_datatype(attributes().read<std::string>(
          "lue_datatype"))},
      _rank{attributes().read<rank_t>("lue_rank")},
      _nr_items{attributes().read<count_t>("lue_nr_items")}

{
}


Item::Item(
    hdf5::Identifier const& location,
    std::string const& name)

    : hdf5::Group(location, name),
      _type_id{hdf5::parse_standard_datatype(attributes().read<std::string>(
          "lue_datatype"))},
      _rank{attributes().read<rank_t>("lue_rank")},
      _nr_items{attributes().read<count_t>("lue_nr_items")}

{
}


// Item::Item(
//     hdf5::Identifier const& location,
//     std::string const& name,
//     hid_t const type_id)
// 
//     : omnipresent::Item()
// 
// {
// }


// Item::Item(
//     hdf5::Identifier&& location)
// 
//     : hdf5::Group(std::forward<hdf5::Identifier>(location)),
//       _type_id{parse_datatype(attributes().read<std::string>("lue_datatype"))},
//       _rank{attributes().read<size_t>("lue_rank")}
// 
// {
// }


hid_t Item::type_id() const
{
    return _type_id;
}


rank_t Item::rank() const
{
    return _rank;
}


count_t Item::nr_items() const
{
    return _nr_items;
}


void Item::reserve_item(
    count_t const idx,
    extent_t const* shape)
{
    // Create dataset with this shape.
    std::string name = std::to_string(idx);

    if(array_exists(id(), name)) {
        throw std::runtime_error("Item dataset " + name + " already exists");
    }

    Shape dimension_sizes(shape, shape + _rank);
    Shape max_dimension_sizes(dimension_sizes);

    auto dataspace = hdf5::create_dataspace(dimension_sizes,
        max_dimension_sizes);

    // No chunking for now...
    auto creation_property_list_location = H5P_DEFAULT;

    hdf5::create_dataset(id(), name, _type_id, dataspace,
        creation_property_list_location);
}


void Item::reserve_items(
    count_t const nr_items,
    extent_t const* shapes)
{
    for(count_t i = 0; i < nr_items; ++i) {
        reserve_item(i, &(shapes[i * _rank]));
    }

    _nr_items = nr_items;
    attributes().write<count_t>("lue_nr_items", nr_items);
}


Array Item::operator[](
    index_t const idx) const
{
    return open_array(id(), std::to_string(idx));
}


Item create_item(
    hdf5::Identifier const& location,
    std::string const& name,
    hid_t const type_id,
    rank_t const rank)
{
    // TODO Pass shapes of the items.

    if(hdf5::group_exists(location, name)) {
        throw std::runtime_error("Item group " + name + " already exists");
    }

    auto group = hdf5::create_group(location, name);

    // hdf5::Attributes attributes(group_location);

    group.attributes().write<std::string>("lue_datatype",
        hdf5::standard_datatype_as_string(type_id));
    group.attributes().write<rank_t>("lue_rank", rank);
    group.attributes().write<count_t>("lue_nr_items", 0);

    return Item(std::move(group));

    // Shape dimension_sizes(shape);
    // dimension_sizes.insert(dimension_sizes.begin(), 0);

    // Shape max_dimension_sizes(shape);
    // max_dimension_sizes.insert(max_dimension_sizes.begin(), H5S_UNLIMITED);

    // auto dataspace = hdf5::create_dataspace(dimension_sizes,
    //     max_dimension_sizes);

    // hdf5::Identifier creation_property_list_location(::H5Pcreate(
    //     H5P_DATASET_CREATE), ::H5Pclose);

    // if(!creation_property_list_location.is_valid()) {
    //     throw std::runtime_error("Creation property list cannot be created");
    // }


    // Chunks chunk_dimension_sizes(chunks);
    // chunk_dimension_sizes.insert(chunk_dimension_sizes.begin(), 1000);

    // herr_t status = ::H5Pset_chunk(creation_property_list_location,
    //     chunk_dimension_sizes.size(), chunk_dimension_sizes.data());
    // assert(status >= 0);

    // auto dataset = hdf5::create_dataset(location, name, file_type_id,
    //     dataspace, creation_property_list_location);

    // return Item(std::move(dataset), memory_type_id);
}

}  // namespace different_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
