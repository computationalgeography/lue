#include "lue/property_sets.hpp"
#include "lue/tag.hpp"


namespace lue {

/*!
    @brief      Construct an instance
    @param      parent Parent group where collection is stored
*/
PropertySets::PropertySets(
    hdf5::Group const& parent)

    : Collection(parent, property_sets_tag)

{
}


PropertySets::PropertySets(
    Collection<PropertySet>&& collection)

    : Collection(std::forward<Collection<PropertySet>>(collection))

{
}


// /*!
//     @brief      Create and add a PropertySet instance
//     @param      name Name of property set
//     @sa         Collection.add(), create_property_set()
// */
// PropertySet& PropertySets::add(
//     std::string const& name)
// {
//     return Collection::add(name, create_property_set(id(), name));
// }


/*!
    @brief      Return new PropertySets instance
    @param      group Group to create collection in
    @exception  std::runtime_error In case a property-sets group already exists
    @exception  std::runtime_error In case a property-sets group cannot
                be created
*/
PropertySets create_property_sets(
    hdf5::Group const& group)
{
    return create_collection<PropertySet>(group, property_sets_tag);
}

} // namespace lue
