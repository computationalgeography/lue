#include "lue/cxx_api/constant_size/item.h"
// #include "lue/c_api/hdf5/hdf5_group.h"
#include <cassert>
#include <stdexcept>


namespace lue {
namespace constant_size {

// /*!
//     @ingroup    lue_cxx_api_group
// */
// bool item_exists(
//     hdf5::Identifier const& location,
//     std::string const& name)
// {
//     return ::hdf5_group_exists(location, name.c_str()) > 0;
// }


Item::Item()
{
}


// Item::Item(
//     hdf5::Identifier const& location,
//     std::string const& name)
// 
//     : hdf5::Group(hdf5::Identifier(::hdf5_open_group(location,
//         name.c_str()), ::hdf5_close_group))
// 
// {
//     if(!id().is_valid()) {
//         throw std::runtime_error("Item " + name + " cannot be opened");
//     }
// }


// Item::Item(
//     hdf5::Identifier&& location)
// 
//     : hdf5::Group(std::forward<hdf5::Identifier>(location))
// 
// {
//     assert(id().is_valid());
// }


// hdf5::Identifier create_item(
//     hdf5::Identifier const& location,
//     std::string const& name)
// {
//     if(item_exists(location, name)) {
//         throw std::runtime_error("Item " + name + " already exists");
//     }
// 
// 
//     hdf5::Identifier item_location(::hdf5_create_group(location,
//         name.c_str()), ::hdf5_close_group);
// 
//     if(!item_location.is_valid()) {
//         throw std::runtime_error("Item " + name + " cannot be created");
//     }
// 
// 
//     return item_location;
// }

}  // namespace constant_size
}  // namespace lue
