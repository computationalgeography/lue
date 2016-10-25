#include "lue/cxx_api/time/shared_constant/property_set.h"
#include "lue/cxx_api/time/omnipresent/space_box_domain.h"
// #include "lue/cxx_api/array.h"
#include "lue/cxx_api/exception.h"
// #include "lue/c_api/define.h"
// #include "lue/c_api/domain.h"
// #include <cmath>


namespace lue {
namespace time {
namespace shared_constant {

PropertySet::PropertySet(
    lue::PropertySet& group)

    : time::PropertySet(group),
      _ids(domain().id(), "ids", LUE_NATIVE_ITEM)  // ,
      // _constant_shape_properties{},
      // _variable_shape_properties{}

{
}


omnipresent::same_shape::Item& PropertySet::reserve_items(
    hsize_t const nr_items)
{
    _ids.reserve_items(nr_items);

    return _ids;
}


// same_shape::Item& PropertySet::ids()
// {
//     return *_ids;
// }
// 
// 
// same_shape::Property& PropertySet::add_property(
//     std::string const& name,
//     hid_t const file_type_id,
//     hid_t const memory_type_id,
//     Shape const& shape,
//     Chunks const& chunks)
// {
//     auto& property = time::PropertySet::add_property(name);
// 
//     same_shape::configure_property(property, file_type_id,
//         memory_type_id, shape, chunks);
// 
//     _constant_shape_properties.emplace_back(
//         std::make_unique<same_shape::Property>(property, memory_type_id));
// 
//     return *_constant_shape_properties.back();
// }
// 
// 
// different_shape::Property& PropertySet::add_property(
//     std::string const& name,
//     hid_t const file_type_id,
//     // hid_t const memory_type_id,
//     size_t const rank)
// {
//     auto& property = time::PropertySet::add_property(name);
// 
//     different_shape::configure_property(property, file_type_id,
//         /* memory_type_id, */ rank);
// 
//     _variable_shape_properties.emplace_back(
//         std::make_unique<different_shape::Property>(property /* , memory_type_id */));
// 
//     return *_variable_shape_properties.back();
// }


void configure_property_set(
    hdf5::Identifier const& location,
    std::string const& name,
    SpaceDomainConfiguration const& domain_configuration)
{
    Domain domain(location);

    // A same_shape property set contains a constant collection of items
    // identifying the items/object/individuals using an id. The shape of
    // these item ids is the same for all items (a single 0D id).
    hid_t const file_type_id = LUE_STD_ITEM;
    hid_t const memory_type_id = LUE_NATIVE_ITEM;

    omnipresent::same_shape::create_item(domain.id(), "ids",
        file_type_id, memory_type_id);


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
                // case SpaceDomainItemType::point: {
                //     throw_unsupported_error("Space item type: point");
                //     break;
                // }
                case SpaceDomainItemType::box: {
                    hid_t const file_type_id = H5T_IEEE_F64LE;
                    hid_t const memory_type_id = H5T_NATIVE_DOUBLE;
                    size_t const rank = 2;

                    omnipresent::configure_space_box_domain(
                        domain.space_domain().id(), file_type_id,
                        memory_type_id, rank);

                    break;
                }
                // case SpaceDomainItemType::line: {
                //     throw_unsupported_error("Space item type: line");
                //     break;
                // }
                // case SpaceDomainItemType::region: {
                //     throw_unsupported_error("Space item type: region");
                //     break;
                // }
                // case SpaceDomainItemType::cell: {
                //     throw_unsupported_error("Space item type: cell");
                //     break;
                // }
            }

            break;
        }
    }
}

}  // namespace shared_constant
}  // namespace time
}  // namespace lue
