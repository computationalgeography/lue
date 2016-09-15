#include "lue/cxx_api/domain.h"
#include "lue/c_api/domain.h"
#include <cassert>
#include <stdexcept>
#include <utility>


namespace lue {

/*!
    @ingroup    lue_cxx_api_group
*/
bool domain_exists(
    hdf5::Identifier const& location)
{
    return ::domain_exists(location) > 0;
}


Domain::Domain(
    hdf5::Identifier&& location)

    : Group{std::forward<hdf5::Identifier>(location)},
      _time_domain{std::make_unique<TimeDomain>(open_time_domain(id()))},
      _space_domain{std::make_unique<SpaceDomain>(open_space_domain(id()))},
      _configuration{_time_domain->configuration(),
          _space_domain->configuration()}

{
}


TimeDomain& Domain::time_domain() const
{
    assert(_time_domain);
    return *_time_domain;
}


SpaceDomain& Domain::space_domain() const
{
    assert(_space_domain);
    return *_space_domain;
}


DomainConfiguration const& Domain::configuration() const
{
    return _configuration;
}


Domain create_domain(
    hdf5::Identifier const& location,
    DomainConfiguration const& configuration)
{
    if(domain_exists(location)) {
        throw std::runtime_error("Domain already exists");
    }

    hdf5::Identifier domain_location(::create_domain(location), ::close_domain);

    if(!domain_location.is_valid()) {
        throw std::runtime_error("Domain cannot be created");
    }

    create_time_domain(domain_location, configuration.time());
    create_space_domain(domain_location, configuration.space());

    return Domain(std::move(domain_location));
}


Domain open_domain(
    hdf5::Identifier const& location)
{
    hdf5::Identifier domain_location(::open_domain(location), ::close_domain);

    if(!domain_location.is_valid()) {
        throw std::runtime_error("Cannot open property set domain");
    }

    return Domain(std::move(domain_location));
}

} // namespace lue
