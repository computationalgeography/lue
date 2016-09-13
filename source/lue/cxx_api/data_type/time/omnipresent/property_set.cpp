#include "lue/cxx_api/data_type/time/omnipresent/property_set.h"
#include "lue/cxx_api/array.h"
#include "lue/c_api/define.h"
#include "lue/c_api/domain.h"


namespace lue {
namespace api {
namespace time {
namespace omnipresent {

PropertySet::PropertySet(
    lue::PropertySet& group)

    : api::PropertySet(group),
      _items{std::make_unique<Array>(std::move(hdf5::open_dataset(
          domain().id(), "item")), LUE_NATIVE_ITEM)},
      _constant_shape_properties{}

{
    // if(!array_exists(domain().id(), "item")) {
    //     throw std::runtime_error("Item collection does not exist");
    // }

    // hdf5::Identifier dataset_location(::hdf5_open_dataset(domain().id(),
    //     "item"), ::hdf5_close_dataset);

    // if(!dataset_location.is_valid()) {
    //     throw std::runtime_error("Item collection cannot be opened");
    // }

    // _items = std::make_unique<Array>(std::move(dataset_location),
    //     LUE_NATIVE_ITEM);
}


Array& PropertySet::reserve_items(
    hsize_t const nr_items)
{
    _items->resize({nr_items});

    return *_items;
}


Array& PropertySet::items()
{
    return *_items;
}


constant_shape::Property& PropertySet::add_property(
    std::string const& name,
    hid_t const type_id,
    Shape const& shape,
    Chunks const& chunks)
{
    auto& property = api::PropertySet::add_property(name);

    constant_shape::configure_property(property, type_id, shape, chunks);

    _constant_shape_properties.emplace_back(
        std::make_unique<constant_shape::Property>(property, type_id));

    return *_constant_shape_properties.back();
}


void configure_property_set(
    hdf5::Identifier const& location,
    std::string const& name,
    SpaceDomainConfiguration const& domain_configuration)
{
    hdf5::Identifier domain(::open_domain(location), ::close_domain);

    if(!domain.is_valid()) {
        throw std::runtime_error("Domain cannot be opened");
    }

    if(array_exists(domain, "item")) {
        throw std::runtime_error("Item collection already exists");
    }

    auto dataspace = hdf5::create_dataspace({0}, {H5S_UNLIMITED});


    hdf5::Identifier creation_property_list_location(::H5Pcreate(
        H5P_DATASET_CREATE), ::H5Pclose);

    if(!creation_property_list_location.is_valid()) {
        throw std::runtime_error("Creation property list cannot be created");
    }

    std::vector<hsize_t> chunk_dimension_sizes{1000};
    herr_t status = ::H5Pset_chunk(creation_property_list_location, 1,
        chunk_dimension_sizes.data());
    assert(status >= 0);

    auto dataset = hdf5::create_dataset(domain, "item", LUE_STD_ITEM,
        dataspace, creation_property_list_location);
}

}  // namespace omnipresent
}  // namespace time
}  // namespace api
}  // namespace lue
