#include "lue/phenomena.hpp"
#include "lue/tag.hpp"


namespace lue {

// bool phenomena_exists(
//     hdf5::Identifier const& location)
// {
//     return collection_exists(location, phenomena_tag);
// }


/*!
    @brief      Construct an instance
    @param      location Location where collection is stored
    @exception  std::runtime_error In case the group cannot be opened
*/
Phenomena::Phenomena(
    hdf5::Identifier const& location)

    : Collection(location, phenomena_tag)

{
}


Phenomena::Phenomena(
    hdf5::Identifier&& id)

    : Collection(std::forward<hdf5::Identifier>(id))

{
}


Phenomena::Phenomena(
    Collection<Phenomenon>&& collection)

    : Collection(std::forward<Collection<Phenomenon>>(collection))

{
}


/*!
    @brief      Create and add a Phenomenon instance
    @param      name Name of phenomenon
    @sa         Collection.add(), create_phenomenon()
*/
Phenomenon& Phenomena::add(
    std::string const& name)
{
    return Collection::add(name, create_phenomenon(id(), name));
}


/*!
    @brief      Return new Phenomena instance
    @param      location Identifier of group of group to create
    @exception  std::runtime_error In case a phenomena group already exists
    @exception  std::runtime_error In case a phenomena group cannot
                be created
*/
Phenomena create_phenomena(
    hdf5::Identifier const& location)
{
    return create_collection<Phenomenon>(location, phenomena_tag);
}

} // namespace lue
