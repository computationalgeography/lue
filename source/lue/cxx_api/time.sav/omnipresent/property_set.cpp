#include "lue/cxx_api/time/omnipresent/property_set.h"


namespace lue {
namespace omnipresent {

PropertySet::PropertySet(
    hdf5::Identifier const& location,
    std::string const& name)

    : lue::PropertySet(location, name)

{
}


PropertySet::PropertySet(
    hdf5::Identifier&& location)

    : lue::PropertySet(std::forward<hdf5::Identifier>(location))

{
}


PropertySet create_property_set(
    hdf5::Identifier const& location,
    std::string const& name)
{
    if(property_set_exists(location, name)) {
        throw std::runtime_error("Property set " + name + " already exists");
    }

    hdf5::Identifier property_set_location = lue::create_property_set(
        location, name);


    // TODO
    // omnipresent property_set type specific stuff


    return PropertySet(std::move(property_set_location));
}

}  // namespace omnipresent
}  // namespace lue
