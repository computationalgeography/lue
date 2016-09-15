#include "lue/cxx_api/space_domain.h"
#include "lue/c_api/space_domain.h"
#include <cassert>
#include <stdexcept>
#include <utility>


namespace lue {

SpaceDomain::SpaceDomain(
    hdf5::Identifier&& location)

    : Group(std::forward<hdf5::Identifier>(location)),
      _configuration{SpaceDomainConfiguration(
          parse_space_domain_type(
              attributes().read<std::string>("domain_type")),
          parse_space_domain_item_type(
              attributes().read<std::string>("domain_item_type")))
      }

{
}


SpaceDomainConfiguration const& SpaceDomain::configuration() const
{
    return _configuration;
}


SpaceDomain create_space_domain(
    hdf5::Identifier const& location,
    SpaceDomainConfiguration const& configuration)
{
    if(space_domain_exists(location)) {
        throw std::runtime_error("Space domain already exists");
    }

    hdf5::Identifier space_domain_location(::create_space_domain(location),
        ::close_space_domain);

    if(!space_domain_location.is_valid()) {
        throw std::runtime_error("Space domain cannot be created");
    }

    hdf5::Attributes space_domain_attributes(space_domain_location);

    space_domain_attributes.write<std::string>("domain_type",
        space_domain_type_to_string(configuration.type()));
    space_domain_attributes.write<std::string>("domain_item_type",
        space_domain_item_type_to_string(configuration.item_type()));

    return SpaceDomain(std::move(space_domain_location));
}


SpaceDomain open_space_domain(
    hdf5::Identifier const& location)
{
    hdf5::Identifier domain_location(::open_space_domain(location),
        ::close_space_domain);

    if(!domain_location.is_valid()) {
        throw std::runtime_error("Cannot open space domain");
    }

    return SpaceDomain(std::move(domain_location));
}

} // namespace lue
