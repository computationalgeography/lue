#include "lue/phenomena.hpp"
#include "lue/tag.hpp"


namespace lue {

/*!
    @brief      Construct an instance
    @param      parent Parent group where collection is stored
    @exception  std::runtime_error In case the group cannot be opened
*/
Phenomena::Phenomena(
    hdf5::Group const& parent)

    : Collection(parent, phenomena_tag)

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
    return Collection::add(name, create_phenomenon(*this, name));
}


// /*!
//     @brief      Return new Phenomena instance
//     @param      location Identifier of group of group to create
//     @exception  std::runtime_error In case a phenomena group already exists
//     @exception  std::runtime_error In case a phenomena group cannot
//                 be created
// */
// Phenomena create_phenomena(
//     hdf5::Identifier const& location)
// {
//     return create_collection<Phenomenon>(location, phenomena_tag);
// }


/*!
    @brief      Return new Phenomena instance
    @param      group Group to create collection in
    @exception  std::runtime_error In case a phenomena group already exists
    @exception  std::runtime_error In case a phenomena group cannot
                be created
*/
Phenomena create_phenomena(
    hdf5::Group const& group)
{
    return create_collection<Phenomenon>(group, phenomena_tag);
}

} // namespace lue
