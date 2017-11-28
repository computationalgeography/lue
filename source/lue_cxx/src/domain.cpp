#include "lue/domain.hpp"
#include "lue/enum_string_bimap.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace {

detail::EnumStringBimap<Domain::Configuration::DomainType> const
        domain_type_map = {
    { Domain::Configuration::DomainType::omnipresent, "lue_omnipresent" },
    { Domain::Configuration::DomainType::located, "lue_located" },
};


std::string domain_type_to_string(
    Domain::Configuration::DomainType const type)
{
    return domain_type_map.as_string(type);
}


Domain::Configuration::DomainType parse_domain_type(
    std::string const& string)
{
    if(!domain_type_map.contains(string)) {
        throw std::runtime_error("Unknown time domain type: " + string);
    }

    return domain_type_map.as_value(string);
}

}  // Anonymous namespace


// bool domain_exists(
//     hdf5::Identifier const& location)
// {
//     return ::domain_exists(location) > 0;
// }


Domain::Configuration::Configuration(
    DomainType const domain_type)

    : _domain_type{domain_type}

{
}


Domain::Configuration::Configuration(
    hdf5::Attributes const& attributes)
{
    load(attributes);
}


Domain::Configuration::DomainType
    Domain::Configuration::domain_type() const
{
    return _domain_type;
}


void Domain::Configuration::save(
    hdf5::Attributes& attributes) const
{
    attributes.write<std::string>(
        time_domain_type_tag,
        domain_type_to_string(_domain_type)
    );
}


void Domain::Configuration::load(
    hdf5::Attributes const& attributes)
{
    _domain_type = parse_domain_type(
        attributes.read<std::string>(time_domain_type_tag));
}


Domain::Domain(
    hdf5::Group const& parent)

    : hdf5::Group(parent, domain_tag),
      _configuration{attributes()}

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
      _configuration{attributes()}

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
    hdf5::Group const& parent,
    Domain::Configuration const& configuration)
{
    auto group = hdf5::create_group(parent, domain_tag);

    configuration.save(group.attributes());

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

    return Domain{std::move(group)};
}

} // namespace lue
