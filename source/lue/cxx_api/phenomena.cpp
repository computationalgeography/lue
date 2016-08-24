#include "lue/cxx_api/phenomena.h"
// #include "lue/c_api/phenomena.h"
#include <cassert>
#include <stdexcept>


namespace lue {

/*!
    @ingroup    lue_cxx_api_group
*/
bool phenomena_exists(
    hdf5::Identifier const& location)
{
    return collection_exists(location, "phenomena");
}


/*!
    @brief      Construct an instance
    @param      location Location where collection is stored
*/
Phenomena::Phenomena(
    hdf5::Identifier const& location)

    : Collection(location, "phenomena")

{
}


/*!
    @brief      Add new phenomenon to collection
    @param      Name name of phenomenon
    @sa         Collection.add(), create_phenomenon()
*/
Phenomenon& Phenomena::add(
    std::string const& name)
{
    return Collection::add(name, create_phenomenon(id(), name));
}


Phenomena create_phenomena(
    hdf5::Identifier const& location)
{
    // TODO Ideally we should be able to create a Phenomena instance based
    // on the already open phenomena group. But the constructor would
    // conflict with the existing one accepting a parent location.

    create_collection(location, "phenomena");

    return Phenomena(location);
}

} // namespace lue
