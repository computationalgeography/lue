#include "lue/domain.hpp"
#include "lue/enum_string_bimap.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace {

detail::EnumStringBimap<TimeDomainType> const
        time_domain_type_map = {
    { TimeDomainType::omnipresent, "lue_omnipresent" }
};


std::string time_domain_type_to_string(
    TimeDomainType const type)
{
    return time_domain_type_map.as_string(type);
}


TimeDomainType parse_time_domain_type(
    std::string const& string)
{
    if(!time_domain_type_map.contains(string)) {
        throw std::runtime_error("Unknown time domain type: " + string);
    }

    return time_domain_type_map.as_value(string);
}

}  // Anonymous namespace


// bool domain_exists(
//     hdf5::Identifier const& location)
// {
//     return ::domain_exists(location) > 0;
// }


Domain::Configuration::Configuration(
    TimeDomainType const type)

    : _time_domain_type{type}

{
}


Domain::Configuration::Configuration(
    hdf5::Attributes const& attributes)
{
    load(attributes);
}


TimeDomainType Domain::Configuration::time_domain_type() const
{
    return _time_domain_type;
}


void Domain::Configuration::save(
    hdf5::Attributes& attributes) const
{
    attributes.write<std::string>(
        time_domain_type_tag,
        time_domain_type_to_string(_time_domain_type)
    );
}


void Domain::Configuration::load(
    hdf5::Attributes const& attributes)
{
    _time_domain_type =
        parse_time_domain_type(
            attributes.read<std::string>(
                time_domain_type_tag));
}


Domain::Domain(
    hdf5::Identifier const& location)

    : hdf5::Group(location, domain_tag),
      _configuration(attributes())

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

    : hdf5::Group{std::forward<hdf5::Group>(group)},
      _configuration(attributes())

      // _time_domain{std::make_unique<TimeDomain>(id())},
      // _space_domain{std::make_unique<SpaceDomain>(id())},
      // _configuration{_time_domain->configuration(),
      //     _space_domain->configuration()}

{
    // assert(id().is_valid());
}


Domain::Configuration const& Domain::configuration() const
{
    return _configuration;
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
    hdf5::Identifier const& location,
    Domain::Configuration const& configuration)
{
    auto domain = hdf5::create_group(location, domain_tag);

    configuration.save(domain.attributes());

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
