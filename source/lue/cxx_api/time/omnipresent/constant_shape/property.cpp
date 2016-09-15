// #include "lue/cxx_api/time/omnipresent/space/omnipresent/property.h"
#include "lue/cxx_api/time/omnipresent/constant_shape/property.h"
#include "lue/cxx_api/value.h"
// #include <iostream>
#include <cassert>


namespace lue {
namespace time {
namespace omnipresent {
namespace constant_shape {

Property::Property(
    lue::Property& group,
    hid_t const type_id)

    : time::Property(group),
      _value(std::make_unique<Value>(open_value(id()))),
      _items(std::make_unique<Array>(open_dataset(_value->id(), "item"),
          type_id))
             // std::move(value_location), type_id);
    // ,
      // _items{}

{
    

    // assert(value_exists(id()));
    // _value = std::make_unique<Value>(open_value(id()));

    // if(!value_exists(this->group().id())) {
    //     throw std::runtime_error("Property value does not exist");
    // }

    // auto value = open_value(this->group());

    // // hdf5::Identifier value_location(open_value(this->group()),
    // //     ::close_value);

    // // if(!value_location.is_valid()) {
    // //     throw std::runtime_error("Property value cannot be opened");
    // // }

    // assert(array_exists(_value, "value"));
    // _items = std::make_unique<Array>(open_array(_value, "value"), type_id);
    //         // std::move(value_location), type_id);
}


Array& Property::reserve_items(
    hsize_t const nr_items)
{
    auto shape = _items->shape();
    shape[0] = nr_items;

    _items->resize(shape);

    return *_items;
}


Array& Property::values()
{
    return *_items;
}


void configure_property(
    lue::Property const& property,
    hid_t const type_id,
    Shape const& shape,
    Chunks const& chunks)
{
    auto const& value = property.value();

    size_t const nr_dimensions = shape.size() + 1;

    std::vector<hsize_t> dimension_sizes(nr_dimensions);
    dimension_sizes[0] = 0;
    for(size_t i = 0; i < shape.size(); ++i) {
        dimension_sizes[i + 1] = shape[i];
    }

    std::vector<hsize_t> max_dimension_sizes(nr_dimensions);
    max_dimension_sizes[0] = H5S_UNLIMITED;
    for(size_t i = 0; i < shape.size(); ++i) {
        max_dimension_sizes[i + 1] = shape[i];
    }

    std::vector<hsize_t> chunk_dimension_sizes(nr_dimensions);
    chunk_dimension_sizes[0] = 1000;
    for(size_t i = 0; i < chunks.size(); ++i) {
        chunk_dimension_sizes[i + 1] = chunks[i];
    }


    auto dataspace = hdf5::create_dataspace(dimension_sizes,
        max_dimension_sizes);

    hdf5::Identifier creation_property_list_location(::H5Pcreate(
        H5P_DATASET_CREATE), ::H5Pclose);

    if(!creation_property_list_location.is_valid()) {
        throw std::runtime_error("Creation property list cannot be created");
    }

    // std::vector<hsize_t> chunk_dimension_sizes{1000};
    herr_t status = ::H5Pset_chunk(creation_property_list_location,
        chunk_dimension_sizes.size(), chunk_dimension_sizes.data());
    assert(status >= 0);

    auto dataset = hdf5::create_dataset(value.id(), "item", type_id,
        dataspace, creation_property_list_location);


    // Array array = create_dataset(value.id(), "item"), LUE_NATIVE_ITEM)},

    // , type_id, shape.size(), shape.data(), chunks.data());

    // // Add our special stuff.
    // hdf5::Identifier value_location(::create_value(property.id(),
    //     type_id, shape.size(), shape.data(), chunks.data()), ::close_value);

    // if(!value_location.is_valid()) {
    //     throw std::runtime_error("Value for property " + name +
    //         " cannot be created");
    // }


}

}  // namespace constant_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace lue
