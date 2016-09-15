// #include "lue/cxx_api/property_set.h"

#include "lue/cxx_api/data_type/omnipresent/property_set.h"
#include "lue/cxx_api/domain_configuration.h"
#include "lue/cxx_api/exception.h"
#include "lue/c_api/domain.h"
// #include "lue/c_api/properties.h"
#include "lue/c_api/property_set.h"
#include "lue/c_api/space_domain.h"
#include "lue/c_api/time_domain.h"
#include <cassert>


namespace lue {

bool property_set_exists(
    hdf5::Identifier const& location,
    std::string const& name)
{
    return ::property_set_exists(location, name.c_str());
}


PropertySet::PropertySet(
    hdf5::Identifier const& location,
    std::string const& name)

    : hdf5::Group(hdf5::Identifier(::open_property_set(location,
        name.c_str()), ::close_property_set))

{
    if(!id().is_valid()) {
        throw std::runtime_error("Property set " + name + " cannot be opened");
    }

    // Open domain and property collection.
    assert(domain_exists(id()));
    _domain = std::make_unique<Domain>(open_domain(id()));
    assert(_domain->id().is_valid());

    assert(properties_exists(id()));
    _properties = std::make_unique<Properties>(id());
    assert(_properties->id().is_valid());
}


PropertySet::PropertySet(
    hdf5::Identifier&& location)

    : hdf5::Group(std::forward<hdf5::Identifier>(location))

{
    assert(id().is_valid());

    // Open domain and property collection.
    assert(domain_exists(id()));
    _domain = std::make_unique<Domain>(open_domain(id()));
    assert(_domain->id().is_valid());

    assert(properties_exists(id()));
    _properties = std::make_unique<Properties>(id());
    assert(_properties->id().is_valid());
}


Domain& PropertySet::domain() const
{
    assert(_domain);
    assert(_domain->id().is_valid());
    return *_domain;
}


Property& PropertySet::add_property(
    std::string const& name)
{
    assert(_properties->id().is_valid());
    return properties().add(name);
}


Properties& PropertySet::properties() const
{
    assert(_properties);
    assert(_properties->id().is_valid());
    return *_properties;
}


PropertySet create_property_set(
    hdf5::Identifier const& location,
    std::string const& name,
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

    // // create_domain(property_set_location, domain_configuration);

    auto const& time_configuration = domain_configuration.time();
    // // auto const& space_configuration = domain_configuration.space();
    // std::unique_ptr<hdf5::Identifier> property_set_location;
    // std::unique_ptr<PropertySet> property_set;

    switch(time_configuration.type()) {
        case TimeDomainType::omnipresent: {


            // property_set = std::make_unique<omnipresent::PropertySet>(
            //     lue::omnipresent::create_property_set(location, name));
            //     // , space_configuration);

            // auto property = omnipresent::create_property_set(
            //     location, name.c_str()); // , space_configuration);
            // property_set_location = std::make_unique<hdf5::Identifier>(
            //     property.id());
            break;
        }
        case TimeDomainType::shared_constant_collection: {
            throw_unsupported_error(
                "Time domain: shared constant collection");
            break;
        }
        case TimeDomainType::shared_variable_collection: {
            throw_unsupported_error(
                "Time domain: shared variable collection");
            break;
        }
        case TimeDomainType::unique_constant_collection: {
            throw_unsupported_error(
                "Time domain: unique constant collection");
            break;
        }
        case TimeDomainType::unique_variable_collection: {
            throw_unsupported_error(
                "Time domain: unique variable collection");
            break;
        }
    }

    // // assert(property_set_location);
    // // return PropertySet(std::move(*property_set_location));

    // assert(property_set);
    // return std::move(*property_set);

    return PropertySet(std::move(property_set_location));
}

} // namespace lue
