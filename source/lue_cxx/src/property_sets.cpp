#include "lue/property_sets.hpp"


namespace lue {

// bool property_sets_exists(
//     hdf5::Identifier const& location)
// {
//     return collection_exists(location, "lue_property_sets");
// }


/*!
    @brief      Construct an instance
    @param      location Location where collection is stored
*/
PropertySets::PropertySets(
    hdf5::Identifier const& location)

    : Collection(location, "lue_property_sets")

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


// /*!
//     @brief      Create and add a PropertySet instance
//     @param      Name name of property set
//     @exception  std::runtime_error In case the property set cannot
//                 be created
//     @sa         Collection.add()
// */
// PropertySet& PropertySets::add(
//     std::string const& name)
// {
//     PropertySetConfiguration configuration(
//         SizeOfItemCollectionType::constant_size);
//     DomainConfiguration domain_configuration;
// 
//     return add(name, configuration, domain_configuration);
// }
// 
// 
// /*!
//     @brief      Create and add a PropertySet instance
//     @param      Name name of property set
//     @exception  std::runtime_error In case the property set cannot
//                 be created
//     @sa         Collection.add()
// */
// PropertySet& PropertySets::add(
//     std::string const& name,
//     PropertySetConfiguration const& configuration,
//     DomainConfiguration const& domain_configuration)
// {
//     return Collection::add(name, create_property_set(id(), name,
//         configuration, domain_configuration));
// }


PropertySets create_property_sets(
    hdf5::Identifier const& location)
{
    return create_collection<PropertySet>(location, "lue_property_sets");
}

} // namespace lue
