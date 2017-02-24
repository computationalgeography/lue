#include "lue/cxx_api/properties.h"
#include <cassert>
#include <stdexcept>


namespace lue {

/*!
    @ingroup    lue_cxx_api_group
*/
bool properties_exists(
    hdf5::Identifier const& location)
{
    return collection_exists(location, "lue_properties");
}


// Properties::Properties(
//     hdf5::Identifier const& location)
// 
//     : Collection(location, "lue_properties")
// 
// {
// }
// 
// 
// Property& Properties::add(
//     std::string const& name,
//     hdf5::Identifier const& domain_id,
//     ValueConfiguration const& value_configuration)
// {
//     return Collection::add(name, create_property(id(), name, domain_id,
//         value_configuration));
// }


Properties<> create_properties(
    hdf5::Identifier const& location)
{
    // TODO Ideally we should be able to create a Properties instance based
    // on the already open properties group. But the constructor would
    // conflict with the existing one accepting a parent location.

    create_collection(location, "lue_properties");

    return Properties<>(location);
}

} // namespace lue
