#include "lue/cxx_api/property_set.h"
#include "lue/cxx_api/time/omnipresent/property_set.h"
#include "lue/cxx_api/time/shared/property_set.h"
#include "lue/cxx_api/time/shared/time_period_domain.h"
#include "lue/cxx_api/domain_configuration.h"
#include "lue/cxx_api/exception.h"
#include "lue/c_api/domain.h"
#include "lue/c_api/property_set.h"
#include "lue/c_api/space_domain.h"
#include "lue/c_api/time_domain.h"
#include <cassert>


namespace lue {

/*!
    @ingroup    lue_cxx_api_group
*/
bool property_set_exists(
    hdf5::Identifier const& location,
    std::string const& name)
{
    return ::property_set_exists(location, name.c_str());
}


/*!
    @brief      Construct an instance based on an existing property set
    @param      location Location in dataset of property set named @a name
    @param      name Name of property set to open
    @exception  std::runtime_error In case property set cannot be opened
    @warning    It is assumed that a domain and property set collection
                exist in property set @a name at @a location
*/
PropertySet::PropertySet(
    hdf5::Identifier const& location,
    std::string const& name)

    : hdf5::Group(hdf5::Identifier(::open_property_set(location,
        name.c_str()), ::close_property_set)),
      _configuration{PropertySetConfiguration(
          parse_size_of_item_collection_type(
              attributes().read<std::string>(
                  "lue_size_of_item_collection_type")))
      }

{
    if(!id().is_valid()) {
        throw std::runtime_error("Property set " + name + " cannot be opened");
    }

    // Open domain and property collection.
    assert(domain_exists(id()));
    _domain = std::make_unique<Domain>(id());
    assert(_domain->id().is_valid());

    assert(properties_exists(id()));
    _properties = std::make_unique<Properties>(id());
    assert(_properties->id().is_valid());
}


/*!
    @brief      Construct an instance based on an existing property set
    @param      location Location in dataset of property set
    @warning    It is assumed that a property exists at @a location
*/
PropertySet::PropertySet(
    hdf5::Identifier&& location)

    : hdf5::Group(std::forward<hdf5::Identifier>(location)),
      _configuration{PropertySetConfiguration(
          parse_size_of_item_collection_type(
              attributes().read<std::string>(
                  "lue_size_of_item_collection_type")))
      }

{
    assert(id().is_valid());

    // Open domain and property collection.
    assert(domain_exists(id()));
    _domain = std::make_unique<Domain>(id());
    assert(_domain->id().is_valid());

    assert(properties_exists(id()));
    _properties = std::make_unique<Properties>(id());
    assert(_properties->id().is_valid());
}


PropertySetConfiguration const& PropertySet::configuration() const
{
    return _configuration;
}


/*!
    @brief      Return domain
*/
Domain& PropertySet::domain() const
{
    assert(_domain);
    assert(_domain->id().is_valid());
    return *_domain;
}


/*!
    @brief      Add property to collection
    @param      name Name of property to add
*/
Property& PropertySet::add_property(
    std::string const& name,
    ValueConfiguration const& value_configuration)
{
    assert(_properties->id().is_valid());
    return properties().add(name, domain().id(), value_configuration);
}


/*!
    @brief      Return property collection
*/
Properties& PropertySet::properties() const
{
    assert(_properties);
    assert(_properties->id().is_valid());
    return *_properties;
}


/*!
    @ingroup    lue_cxx_api_group

    @brief      Create a new property set in the dataset
    @param      location Where to create property set
    @param      name Name of property set to create
    @param      domain_configuration Configuration of domain
    @return     Instance refering to the new property set
    @exception  std::runtime_error In case property set @a name already exists
                at @a location
    @exception  std::runtime_error In case property set cannot be created
    @sa         PropertySets::add()
*/
PropertySet create_property_set(
    hdf5::Identifier const& location,
    std::string const& name,
    PropertySetConfiguration const& configuration,
    DomainConfiguration const& domain_configuration)
{
    if(property_set_exists(location, name)) {
        throw std::runtime_error("Property set " + name + " already exists");
    }

    hdf5::Identifier property_set_location(::create_property_set(location,
        name.c_str()), ::close_property_set);

    if(!property_set_location.is_valid()) {
        throw std::runtime_error("Property set " + name +
            " cannot be created");
    }


    hdf5::Attributes property_set_attributes(property_set_location);

    property_set_attributes.write<std::string>(
        "lue_size_of_item_collection_type",
        size_of_item_collection_type_to_string(
            configuration.size_of_item_collection_type()));


    auto const& domain = create_domain(property_set_location,
        domain_configuration);

    auto const& time_configuration = domain_configuration.time();
    auto const& space_configuration = domain_configuration.space();

    // TODO
    assert(configuration.size_of_item_collection_type() ==
        SizeOfItemCollectionType::constant_size);

    switch(time_configuration.type()) {
        case TimeDomainType::omnipresent: {
            time::omnipresent::configure_property_set(property_set_location,
                name, space_configuration);
            break;
        }
        case TimeDomainType::shared: {
            time::shared::configure_property_set(
                property_set_location, name, space_configuration);

            hid_t const file_type_id = H5T_STD_I32LE;
            hid_t const memory_type_id = H5T_NATIVE_INT32;

            time::shared::configure_time_period_domain(
                domain.time_domain().id(), file_type_id, memory_type_id);

            break;
        }
    }

    return PropertySet(std::move(property_set_location));
}

} // namespace lue
