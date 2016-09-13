#include "lue/cxx_api/property_set.h"
#include "lue/cxx_api/data_type/time/omnipresent/property_set.h"
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

    assert(properties_exists(id()));
    _properties = std::make_unique<Properties>(id());
}


PropertySet::PropertySet(
    hdf5::Identifier&& location)

    : hdf5::Group(std::forward<hdf5::Identifier>(location))

{
    assert(id().is_valid());

    // Open domain and property collection.
    assert(domain_exists(id()));
    _domain = std::make_unique<Domain>(open_domain(id()));

    assert(properties_exists(id()));
    _properties = std::make_unique<Properties>(id());
}


Domain& PropertySet::domain() const
{
    assert(_domain);
    return *_domain;
}


Property& PropertySet::add_property(
    std::string const& name)
{
    return properties().add(name);
}


Properties& PropertySet::properties() const
{
    assert(_properties);
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


    hdf5::Identifier domain(::open_domain(property_set_location),
        ::close_domain);
    assert(domain.is_valid());


    auto const& time_configuration = domain_configuration.time();
    auto const& space_configuration = domain_configuration.space();

    switch(time_configuration.type()) {
        case TimeDomainType::omnipresent: {
            lue::api::time::omnipresent::configure_property_set(
                property_set_location, name.c_str(), space_configuration);
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
                "Time domain: unique variable collection");
            break;
        }
        case TimeDomainType::unique_variable_collection: {
            throw_unsupported_error(
                "Time domain: unique variable collection");
            break;
        }
    }


    {
        hdf5::Identifier time_domain(::open_time_domain(domain),
            ::close_time_domain);
        assert(time_domain.is_valid());

        hdf5::Attributes time_domain_attributes(time_domain);

        time_domain_attributes.write<std::string>("domain_type",
            time_domain_type_to_string(time_configuration.type()));
    }


    {
        hdf5::Identifier space_domain(::open_space_domain(domain),
            ::close_space_domain);
        assert(space_domain.is_valid());

        hdf5::Attributes space_domain_attributes(space_domain);

        space_domain_attributes.write<std::string>("domain_type",
            space_domain_type_to_string(space_configuration.type()));
        space_domain_attributes.write<std::string>("domain_item_type",
            space_domain_item_type_to_string(space_configuration.item_type()));
    }


    return PropertySet(std::move(property_set_location));
}

} // namespace lue
