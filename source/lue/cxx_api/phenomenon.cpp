#include "lue/cxx_api/phenomenon.h"
#include "lue/c_api/phenomenon.h"
#include <cassert>
#include <stdexcept>


namespace lue {

/*!
    @ingroup    lue_cxx_api_group
*/
bool phenomenon_exists(
    hdf5::Identifier const& location,
    std::string const& name)
{
    return ::phenomenon_exists(location, name.c_str()) > 0;
}


/*!
    @brief      Construct an instance
    @param      location Location of group of phenomenon to open
    @param      name Name of instance to open
    @exception  std::runtime_error In case the phenomenon cannot be opened
*/
Phenomenon::Phenomenon(
    hdf5::Identifier const& location,
    std::string const& name)

    : hdf5::Group(hdf5::Identifier(::open_phenomenon(location, name.c_str()),
        ::close_phenomenon))

{
    if(!id().is_valid()) {
        throw std::runtime_error("Phenomenon " + name + " cannot be opened");
    }

    // Open property sets collection.
    assert(property_sets_exists(id()));
    _property_sets = std::make_unique<PropertySets>(id());
}


/*!
    @brief      Construct an instance
    @param      location Location of open phenomenon
*/
Phenomenon::Phenomenon(
    hdf5::Identifier&& location)

    : hdf5::Group(std::forward<hdf5::Identifier>(location))

{
    assert(id().is_valid());

    // Open property sets collection.
    assert(property_sets_exists(id()));
    _property_sets = std::make_unique<PropertySets>(id());
}


PropertySet& Phenomenon::add_property_set(
    std::string const& name)
{
    return property_sets().add(name);
}


PropertySet& Phenomenon::add_property_set(
    std::string const& name,
    DomainConfiguration const& domain_configuration)
{
    return property_sets().add(name, domain_configuration);
}


/*!
    @brief      Return the property sets
*/
PropertySets& Phenomenon::property_sets() const
{
    assert(_property_sets);
    return *_property_sets;
}


/*!
    @ingroup    lue_cxx_api_group
    @brief      Add new phenomenon
    @param      Location Id of location of file or group to add
                phenomenon to
    @param      Name name of phenomenon
    @exception  std::runtime_error In case a phenomenon with the name
                passed in already exists
    @exception  std::runtime_error In case the phenomenon cannot be
                created
*/
Phenomenon create_phenomenon(
    hdf5::Identifier const& location,
    std::string const& name)
{
    if(phenomenon_exists(location, name)) {
        throw std::runtime_error("Phenomenon " + name + " already exists");
    }

    hdf5::Identifier phenomenon_location(::create_phenomenon(location,
        name.c_str()), ::close_phenomenon);

    if(!phenomenon_location.is_valid()) {
        throw std::runtime_error("Phenomenon " + name + " cannot be created");
    }

    return Phenomenon(std::move(phenomenon_location));
}

} // namespace lue
