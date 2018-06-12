#include "lue/universes.hpp"
#include "lue/tag.hpp"


namespace lue {

/*!
    @brief      Constructor
    @param      parent Parent group of group to open
    @exception  std::runtime_error In case the group cannot be opened
*/
Universes::Universes(
    hdf5::Group const& parent)

    : Collection(parent, universes_tag)

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
    return Collection::add(name, create_universe(*this, name));
}


/*!
    @brief      Return new Universes instance
    @param      group Group to create collection in
    @exception  std::runtime_error In case a universes group already exists
    @exception  std::runtime_error In case a universes group cannot
                be created
*/
Universes create_universes(
    hdf5::Group const& group)
{
    return create_collection<Universe>(group, universes_tag);
}

} // namespace lue
