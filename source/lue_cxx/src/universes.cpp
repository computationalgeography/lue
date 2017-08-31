#include "lue/universes.hpp"


namespace lue {

// bool universes_exists(
//     hdf5::Identifier const& location)
// {
//     return collection_exists(location, "lue_universes");
// }


/*!
    @brief      Constructor
    @param      location Identifier of group of group to open
    @exception  std::runtime_error In case the group cannot be opened
*/
Universes::Universes(
    hdf5::Identifier const& location)

    : Collection(location, "lue_universes")

{
}


Universes::Universes(
    hdf5::Identifier&& id)

    : Collection(std::forward<hdf5::Identifier>(id))

{
}


Universes::Universes(
    Collection<Universe>&& collection)

    : Collection(std::forward<Collection<Universe>>(collection))

{
}


/*!
    @brief      Create and add a Universe instance
    @param      name Name of universe
    @sa         Collection.add(), create_universe()
*/
Universe& Universes::add(
    std::string const& name)
{
    return Collection::add(name, create_universe(id(), name));
}


/*!
    @brief      Return new Universes instance
    @param      location Identifier of group of group to create
    @exception  std::runtime_error In case a universes group already exists
    @exception  std::runtime_error In case a universes group cannot
                be created
*/
Universes create_universes(
    hdf5::Identifier const& location)
{
    return create_collection<Universe>(location, "lue_universes");
}

} // namespace lue
