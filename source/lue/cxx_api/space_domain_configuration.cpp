#include "lue/cxx_api/space_domain_configuration.h"
#include "lue/cxx_api/enum_string_bimap.h"


namespace lue {
namespace {

detail::EnumStringBimap<SpaceDomainType> const space_domain_type_map = {
    { SpaceDomainType::omnipresent, "omnipresent" },
    { SpaceDomainType::located, "located" }
};


detail::EnumStringBimap<SpaceDomainItemType> const
        space_domain_item_type_map = {
    { SpaceDomainItemType::none, "none" },
    // { SpaceDomainItemType::point, "point" },
    { SpaceDomainItemType::box, "box" },
    // { SpaceDomainItemType::line, "line" },
    // { SpaceDomainItemType::region, "region" },
    // { SpaceDomainItemType::cell, "cell" }
};


detail::EnumStringBimap<Mobility> const mobility_map = {
    { Mobility::stationary, "stationary" },
    { Mobility::mobile, "mobile" }
};

}  // Anonymous namespace


/*!
    @ingroup    lue_cxx_api_group
*/
std::string space_domain_type_to_string(
    SpaceDomainType const type)
{
    return space_domain_type_map.as_string(type);
}


/*!
    @ingroup    lue_cxx_api_group
*/
SpaceDomainType parse_space_domain_type(
    std::string const& string)
{
    if(!space_domain_type_map.contains(string)) {
        throw std::runtime_error("Unknown space domain type: " + string);
    }

    return space_domain_type_map.as_value(string);
}


/*!
    @ingroup    lue_cxx_api_group
*/
std::string space_domain_item_type_to_string(
    SpaceDomainItemType const type)
{
    return space_domain_item_type_map.as_string(type);
}


/*!
    @ingroup    lue_cxx_api_group
*/
SpaceDomainItemType parse_space_domain_item_type(
    std::string const& string)
{
    if(!space_domain_item_type_map.contains(string)) {
        throw std::runtime_error("Unknown space domain item type: " + string);
    }

    return space_domain_item_type_map.as_value(string);
}


std::string mobility_to_string(
    Mobility const mobility)
{
    return mobility_map.as_string(mobility);
}


Mobility parse_mobility(
    std::string const& string)
{
    if(!mobility_map.contains(string)) {
        throw std::runtime_error("Unknown mobility: " + string);
    }

    return mobility_map.as_value(string);
}


SpaceDomainConfiguration::SpaceDomainConfiguration()

    : _type{SpaceDomainType::omnipresent},
      _item_type{SpaceDomainItemType::none},
      _mobility{Mobility::stationary}

{
}


SpaceDomainConfiguration::SpaceDomainConfiguration(
     SpaceDomainItemType const item_type)

    : _type{SpaceDomainType::located},
      _item_type{item_type},
      _mobility{Mobility::stationary}

{
    if(_item_type == SpaceDomainItemType::none) {
        throw std::runtime_error("Domain item type must be set");
    }
}


SpaceDomainConfiguration::SpaceDomainConfiguration(
    SpaceDomainType const type,
    SpaceDomainItemType const item_type)

    : _type{type},
      _item_type{item_type},
      _mobility{Mobility::stationary}

{
    if(_type == SpaceDomainType::omnipresent) {
        // Whatever the user passed in...
        _item_type = SpaceDomainItemType::none;
    }
    else if(_item_type == SpaceDomainItemType::none) {
        throw std::runtime_error("Domain item type must be set");
    }
}


SpaceDomainConfiguration::SpaceDomainConfiguration(
    SpaceDomainType const type,
    SpaceDomainItemType const item_type,
    Mobility const mobility)

    : _type{type},
      _item_type{item_type},
      _mobility{mobility}

{
    if(_type == SpaceDomainType::omnipresent) {
        // Whatever the user passed in...
        _item_type = SpaceDomainItemType::none;
        _mobility = Mobility::stationary;
    }
    else if(_item_type == SpaceDomainItemType::none) {
        throw std::runtime_error("Domain item type must be set");
    }
}


SpaceDomainType SpaceDomainConfiguration::type() const
{
    return _type;
}


SpaceDomainItemType SpaceDomainConfiguration::item_type() const
{
    return _item_type;
}


Mobility SpaceDomainConfiguration::mobility() const
{
    return _mobility;
}

}  // namespace lue
