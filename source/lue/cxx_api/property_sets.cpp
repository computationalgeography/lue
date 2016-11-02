#include "lue/cxx_api/property_sets.h"


namespace lue {

/*!
    @ingroup    lue_cxx_api_group
*/
bool property_sets_exists(
    hdf5::Identifier const& location)
{
    return collection_exists(location, "property_sets");
}


/*!
    @brief      Construct an instance
    @param      location Location where collection is stored
*/
PropertySets::PropertySets(
    hdf5::Identifier const& location)

    : Collection(location, "property_sets")

{
}


/*!
    @brief      Create and add a PropertySet instance
    @param      Name name of property set
    @exception  std::runtime_error In case the property set cannot
                be created
    @sa         Collection.add()
*/
PropertySet& PropertySets::add(
    std::string const& name)
{
    PropertySetConfiguration configuration(
        SizeOfItemCollectionType::constant_size);
    DomainConfiguration domain_configuration;

    return add(name, configuration, domain_configuration);
}


/*!
    @brief      Create and add a PropertySet instance
    @param      Name name of property set
    @exception  std::runtime_error In case the property set cannot
                be created
    @sa         Collection.add()
*/
PropertySet& PropertySets::add(
    std::string const& name,
    PropertySetConfiguration const& configuration,
    DomainConfiguration const& domain_configuration)
{
    return Collection::add(name, create_property_set(id(), name,
        configuration, domain_configuration));
}


PropertySets create_property_sets(
    hdf5::Identifier const& location)
{
    // TODO Ideally we should be able to create a PropertySets instance based
    // on the already open property_sets group. But the constructor would
    // conflict with the existing one accepting a parent location.

    create_collection(location, "property_sets");

    return PropertySets(location);
}

} // namespace lue
