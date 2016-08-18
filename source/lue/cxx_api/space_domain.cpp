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
