#include "lue/cxx_api/time_domain.h"
#include "lue/c_api/time_domain.h"
#include <cassert>
#include <stdexcept>
#include <utility>


namespace lue {

TimeDomain::TimeDomain(
    hdf5::Identifier&& location)

    : Group(std::forward<hdf5::Identifier>(location)),
      _configuration{TimeDomainConfiguration(parse_time_domain_type(
          attributes().read<std::string>("domain_type")))}

{
}


TimeDomainConfiguration const& TimeDomain::configuration() const
{
    return _configuration;
}


TimeDomain open_time_domain(
    hdf5::Identifier const& location)
{
    hdf5::Identifier domain_location(::open_time_domain(location),
        ::close_time_domain);

    if(!domain_location.is_valid()) {
        throw std::runtime_error("Cannot open time domain");
    }

    return TimeDomain(std::move(domain_location));
}

} // namespace lue
