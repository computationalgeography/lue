#include "lue/cxx_api/universes.h"
#include <cassert>
#include <stdexcept>


namespace lue {

/*!
    @ingroup    lue_cxx_api_group
*/
bool universes_exists(
    hdf5::Identifier const& location)
{
    return collection_exists(location, "universes");
}


/*!
    @brief      Constructor
    @param      location Identifier of file or group of group to open
    @exception  std::runtime_error In case the group cannot be opened
*/
Universes::Universes(
    hdf5::Identifier const& location)

    : Collection(location, "universes")

{
}


Universe& Universes::add(
    std::string const& name)
{
    return Collection::add(name, create_universe(id(), name));
}


/*!
    @ingroup    lue_cxx_api_group
    @brief      Return new universes instance
    @param      location Identifier of file or group of group to create
    @exception  std::runtime_error In case a universes group already exists
    @exception  std::runtime_error In case a universes group cannot
                be created
*/
Universes create_universes(
    hdf5::Identifier const& location)
{
    // TODO Ideally we should be able to create a Universes instance based
    // on the already open universes group. But the constructor would
    // conflict with the existing one accepting a parent location.

    create_collection(location, "universes");

    return Universes(location);
}

} // namespace lue
