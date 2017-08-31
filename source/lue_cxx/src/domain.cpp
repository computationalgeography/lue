#include "lue/domain.h"
#include "lue/tag.h"
// #include <cassert>
// #include <stdexcept>
// #include <utility>


namespace lue {

// bool domain_exists(
//     hdf5::Identifier const& location)
// {
//     return ::domain_exists(location) > 0;
// }


Domain::Domain(
    hdf5::Identifier const& location)

    : hdf5::Group(location, domain_tag)
      // _time_domain{std::make_unique<TimeDomain>(id())},
      // _space_domain{std::make_unique<SpaceDomain>(id())},
      // _configuration{_time_domain->configuration(),
      //     _space_domain->configuration()}

{
    // if(!id().is_valid()) {
    //     throw std::runtime_error("Domain cannot be opened");
    // }
}


Domain::Domain(
    hdf5::Group&& group)

    : Group{std::forward<hdf5::Group>(group)}
      // _time_domain{std::make_unique<TimeDomain>(id())},
      // _space_domain{std::make_unique<SpaceDomain>(id())},
      // _configuration{_time_domain->configuration(),
      //     _space_domain->configuration()}

{
    // assert(id().is_valid());
}


// TimeDomain& Domain::time_domain() const
// {
//     assert(_time_domain);
//     return *_time_domain;
// }
// 
// 
// SpaceDomain& Domain::space_domain() const
// {
//     assert(_space_domain);
//     return *_space_domain;
// }
// 
// 
// DomainConfiguration const& Domain::configuration() const
// {
//     return _configuration;
// }


Domain create_domain(
    hdf5::Identifier const& location)
    // DomainConfiguration const& configuration)
{
    auto domain = hdf5::create_group(location, domain_tag);

    // if(domain_exists(location)) {
    //     throw std::runtime_error("Domain already exists");
    // }

    // hdf5::Identifier domain_location(::create_domain(location),
    //     ::close_domain);

    // if(!domain_location.is_valid()) {
    //     throw std::runtime_error("Domain cannot be created");
    // }

    // create_time_domain(domain_location, configuration.time());
    // create_space_domain(domain_location, configuration.space());

    return Domain(std::move(domain));
}

} // namespace lue
