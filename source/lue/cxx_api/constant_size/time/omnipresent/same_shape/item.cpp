#include "lue/cxx_api/constant_size/time/omnipresent/same_shape/item.h"
#include "lue/cxx_api/array.h"
#include <cassert>


namespace lue {
namespace time {
namespace omnipresent {
namespace same_shape {

Item::Item(
    hdf5::Identifier const& location,
    std::string const& name)

    : // omnipresent::Item(),
      Array(open_array(location, name))

{
}


Item::Item(
    hdf5::Identifier const& location,
    std::string const& name,
    hid_t const type_id)

    : // omnipresent::Item(),
      Array(open_array(location, name, type_id))

{
}


Item::Item(
    hdf5::Identifier&& location,
    hid_t const type_id)

    : // omnipresent::Item(),
      Array(std::forward<hdf5::Identifier>(location), type_id)

{
}


Item::Item(
    hdf5::Dataset&& dataset,
    hid_t const type_id)

    : // omnipresent::Item(),
      Array(std::forward<hdf5::Dataset>(dataset), type_id)

{
}


void Item::reserve_items(
    hsize_t const nr_items)
{
    auto shape = this->shape();
    shape[0] = nr_items;

    resize(shape);
}


Item create_item(
    hdf5::Identifier const& location,
    std::string const& name,
    hid_t const file_type_id,
    hid_t const memory_type_id)
{
    return create_item(location, name, file_type_id, memory_type_id,
        Shape{}, Chunks{});
}


Item create_item(
    hdf5::Identifier const& location,
    std::string const& name,
    hid_t const file_type_id,
    hid_t const memory_type_id,
    Shape const& shape,
    Chunks const& chunks)
{
    if(array_exists(location, name)) {
        throw std::runtime_error("Item dataset " + name + " already exists");
    }

    Shape dimension_sizes(shape);
    dimension_sizes.insert(dimension_sizes.begin(), 0);

    Shape max_dimension_sizes(shape);
    max_dimension_sizes.insert(max_dimension_sizes.begin(), H5S_UNLIMITED);

    auto dataspace = hdf5::create_dataspace(dimension_sizes,
        max_dimension_sizes);

    hdf5::Identifier creation_property_list_location(::H5Pcreate(
        H5P_DATASET_CREATE), ::H5Pclose);

    if(!creation_property_list_location.is_valid()) {
        throw std::runtime_error("Creation property list cannot be created");
    }


    Chunks chunk_dimension_sizes(chunks);
    chunk_dimension_sizes.insert(chunk_dimension_sizes.begin(), 1000);

    herr_t status = ::H5Pset_chunk(creation_property_list_location,
        chunk_dimension_sizes.size(), chunk_dimension_sizes.data());
    assert(status >= 0);

    auto dataset = hdf5::create_dataset(location, name, file_type_id,
        dataspace, creation_property_list_location);

    return Item(std::move(dataset), memory_type_id);
}

}  // namespace same_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace lue
