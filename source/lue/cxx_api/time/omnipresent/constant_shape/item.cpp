#include "lue/cxx_api/time/omnipresent/constant_shape/item.h"
// #include "lue/cxx_api/time/omnipresent/space/omnipresent/property.h"
// #include "lue/cxx_api/value.h"
// // #include <iostream>
// #include <cassert>


namespace lue {
namespace omnipresent {
namespace constant_shape {

// /*!
//     @ingroup    lue_cxx_api_group
// */
// bool item_exists(
//     hdf5::Identifier const& location,
//     std::string const& name)
// {
//     return lue::omnipresent::item_exists(location, name);
// }


Item::Item(
    hdf5::Identifier const& location,
    std::string const& name)

    : lue::omnipresent::Item(location, name)

{
}


Item::Item(
    hdf5::Identifier&& location)

    : lue::omnipresent::Item(std::forward<hdf5::Identifier>(location))

{
}


Item create_item(
    hdf5::Identifier const& location,
    std::string const& name)
{
    if(item_exists(location, name)) {
        throw std::runtime_error("Item " + name + " already exists");
    }

    hdf5::Identifier item_location = lue::create_item(location, name);

    // TODO
    // constant shape item type specific stuff

    return Item(std::move(item_location));
}

}  // namespace constant_shape
}  // namespace omnipresent
}  // namespace lue
