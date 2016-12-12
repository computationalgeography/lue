#include "lue/cxx_api/time_domain.h"
#include "lue/c_api/time_domain.h"
#include <cassert>
#include <stdexcept>
#include <utility>


namespace lue {

TimeDomain::TimeDomain(
    hdf5::Identifier const& location)

    : hdf5::Group{hdf5::Identifier(::open_time_domain(location),
        ::close_time_domain)},
      _configuration{TimeDomainConfiguration(
          parse_time_domain_type(
              attributes().read<std::string>("lue_domain_type")),
          parse_time_domain_item_type(
              attributes().read<std::string>("lue_domain_item_type")))
      }

{
    if(!id().is_valid()) {
        throw std::runtime_error("Time domain cannot be opened");
    }
}


TimeDomain::TimeDomain(
    hdf5::Identifier&& location)

    : Group(std::forward<hdf5::Identifier>(location)),
      _configuration{TimeDomainConfiguration(
          parse_time_domain_type(
              attributes().read<std::string>("lue_domain_type")),
          parse_time_domain_item_type(
              attributes().read<std::string>("lue_domain_item_type")))
      }

{
}


TimeDomainConfiguration const& TimeDomain::configuration() const
{
    return _configuration;
}


TimeDomain create_time_domain(
    hdf5::Identifier const& location,
    TimeDomainConfiguration const& configuration)
{
    if(time_domain_exists(location)) {
        throw std::runtime_error("Time domain already exists");
    }

    hdf5::Identifier time_domain_location(::create_time_domain(location),
        ::close_time_domain);

    if(!time_domain_location.is_valid()) {
        throw std::runtime_error("Time domain cannot be created");
    }

    hdf5::Attributes time_domain_attributes(time_domain_location);

    time_domain_attributes.write<std::string>("lue_domain_type",
        time_domain_type_to_string(configuration.type()));
    time_domain_attributes.write<std::string>("lue_domain_item_type",
        time_domain_item_type_to_string(configuration.item_type()));

    return TimeDomain(std::move(time_domain_location));
}

} // namespace lue
