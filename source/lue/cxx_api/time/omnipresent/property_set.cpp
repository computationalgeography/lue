#include "lue/cxx_api/time/omnipresent/property_set.h"
#include "lue/cxx_api/array.h"
#include "lue/cxx_api/exception.h"
#include "lue/c_api/define.h"
#include "lue/c_api/domain.h"
#include <cmath>


namespace lue {
namespace time {
namespace omnipresent {
namespace {

/*!
    @brief      .
    @param      type_id Id of in-file type to use for storing coordinates
    @param      rank Rank of space to store boxes for
    @return     .
    @exception  .

    A dataset named 'item' is created with shape (nr_items, rank * 2**rank).
    The first dimension is configured to be H5S_UNLIMITED.
*/
void configure_box(
    SpaceDomain const& space_domain,
    hid_t const type_id,
    size_t const rank)
{
    size_t const nr_dimensions = 2;

    std::vector<hsize_t> dimension_sizes(nr_dimensions);
    dimension_sizes[0] = 0;
    dimension_sizes[1] = rank * static_cast<hsize_t>(std::pow(2, rank));

    std::vector<hsize_t> max_dimension_sizes(nr_dimensions);
    max_dimension_sizes[0] = H5S_UNLIMITED;
    max_dimension_sizes[1] = dimension_sizes[1];

    std::vector<hsize_t> chunk_dimension_sizes(nr_dimensions);
    chunk_dimension_sizes[0] = 1000;
    chunk_dimension_sizes[1] = dimension_sizes[1];

    auto dataspace = hdf5::create_dataspace(dimension_sizes,
        max_dimension_sizes);

    hdf5::Identifier creation_property_list_location(::H5Pcreate(
        H5P_DATASET_CREATE), ::H5Pclose);

    if(!creation_property_list_location.is_valid()) {
        throw std::runtime_error("Creation property list cannot be created");
    }

    herr_t status = ::H5Pset_chunk(creation_property_list_location,
        chunk_dimension_sizes.size(), chunk_dimension_sizes.data());
    assert(status >= 0);

    auto dataset = hdf5::create_dataset(space_domain.id(), "item", type_id,
        dataspace, creation_property_list_location);
}

}  // Anonymous namespace


PropertySet::PropertySet(
    lue::PropertySet& group)

    : time::PropertySet(group),
      // _items{std::make_unique<Array>(std::move(hdf5::open_dataset(
      //     domain().id(), "item")), LUE_NATIVE_ITEM)},
      _ids{std::make_unique<constant_shape::Item>(domain().id(), "ids",
          LUE_NATIVE_ITEM)},
      _constant_shape_properties{},
      _variable_shape_properties{}

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


// Array& PropertySet::reserve_items(
//     hsize_t const nr_items)
// {
//     _items->resize({nr_items});
// 
//     return *_items;
// }
// 
// 
// Array& PropertySet::items()
// {
//     return *_items;
// }


constant_shape::Item& PropertySet::reserve_items(
    hsize_t const nr_items)
{
    _ids->reserve_items(nr_items);

    return *_ids;
}


constant_shape::Item& PropertySet::ids()
{
    return *_ids;
}


constant_shape::Property& PropertySet::add_property(
    std::string const& name,
    hid_t const file_type_id,
    hid_t const memory_type_id,
    Shape const& shape,
    Chunks const& chunks)
{
    auto& property = time::PropertySet::add_property(name);

    constant_shape::configure_property(property, file_type_id,
        memory_type_id, shape, chunks);

    _constant_shape_properties.emplace_back(
        std::make_unique<constant_shape::Property>(property, memory_type_id));

    return *_constant_shape_properties.back();
}


variable_shape::Property& PropertySet::add_property(
    std::string const& name,
    hid_t const file_type_id,
    hid_t const memory_type_id)
{
    auto& property = time::PropertySet::add_property(name);

    variable_shape::configure_property(property, file_type_id, memory_type_id);

    _variable_shape_properties.emplace_back(
        std::make_unique<variable_shape::Property>(property, memory_type_id));

    return *_variable_shape_properties.back();
}


void configure_property_set(
    hdf5::Identifier const& location,
    std::string const& name,
    SpaceDomainConfiguration const& domain_configuration)
{
    hdf5::Identifier domain_location(::open_domain(location), ::close_domain);

    if(!domain_location.is_valid()) {
        throw std::runtime_error("Domain cannot be opened");
    }

    // An omnipresent property set contains a constant collection of items
    // identifying the items/object/individuals using an id. The shape of
    // these item ids is the same for all items (a single 0D id).
    hid_t const file_type_id = LUE_STD_ITEM;
    hid_t const memory_type_id = LUE_NATIVE_ITEM;

    constant_shape::create_item(domain_location, "ids", file_type_id,
        memory_type_id);


    auto space_domain = open_space_domain(domain_location);

    switch(domain_configuration.type()) {
        case SpaceDomainType::omnipresent: {
            break;
        }
        case SpaceDomainType::located: {
            switch(domain_configuration.item_type()) {
                case SpaceDomainItemType::none: {
                    throw_unsupported_error("Space item type: none");
                    break;
                }
                case SpaceDomainItemType::point: {
                    throw_unsupported_error("Space item type: point");
                    break;
                }
                case SpaceDomainItemType::box: {
                    // TODO
                    // - Create/configure space for a box per item. This is
                    //   similar to a property with constant shape. Reuse that
                    //   code. The result must be a dataset called 'item' with
                    //   dimensions (nr_items, rank * 2**rank)
                    //   - assume rank is 2
                    //   - assume value type is float64

                    hid_t const type_id = H5T_IEEE_F64LE;
                    size_t const rank = 2;

                    configure_box(space_domain, type_id, rank);
                    break;
                }
                case SpaceDomainItemType::line: {
                    throw_unsupported_error("Space item type: line");
                    break;
                }
                case SpaceDomainItemType::region: {
                    throw_unsupported_error("Space item type: region");
                    break;
                }
                case SpaceDomainItemType::cell: {
                    throw_unsupported_error("Space item type: cell");
                    break;
                }
            }

            break;
        }
    }
}

}  // namespace omnipresent
}  // namespace time
}  // namespace lue
