#include "lue/cxx_api/time/property_set.h"
#include "lue/c_api/hdf5/hdf5_group.h"
#include <cassert>
#include <stdexcept>


namespace lue {
namespace time {

// /*!
//     @ingroup    lue_cxx_api_group
// */
// bool property_set_exists(
//     hdf5::Identifier const& location,
//     std::string const& name)
// {
//     return ::hdf5_group_exists(location, name.c_str()) > 0;
// }


PropertySet::PropertySet(
    hdf5::Identifier const& location,
    std::string const& name)

    : hdf5::Group(hdf5::Identifier(::hdf5_open_group(location,
        name.c_str()), ::hdf5_close_group))

{
    if(!id().is_valid()) {
        throw std::runtime_error("Property set " + name + " cannot be opened");
    }
}


PropertySet::PropertySet(
    hdf5::Identifier&& location)

    : hdf5::Group(std::forward<hdf5::Identifier>(location))

{
    assert(id().is_valid());
}


// hdf5::Identifier create_property_set(
//     hdf5::Identifier const& location,
//     std::string const& name)
// {
//     if(property_set_exists(location, name)) {
//         throw std::runtime_error("Property set " + name + " already exists");
//     }
// 
// 
//     hdf5::Identifier property_set_location(::hdf5_create_group(location,
//         name.c_str()), ::hdf5_close_group);
// 
//     if(!property_set_location.is_valid()) {
//         throw std::runtime_error("Property set " + name + " cannot be created");
//     }
// 
// 
//     return property_set_location;
// }

}  // namespace time
}  // namespace lue
