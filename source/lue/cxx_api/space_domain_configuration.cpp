#include "lue/cxx_api/space_domain_configuration.h"
#include <cassert>
#include <map>


namespace lue {
namespace {

static std::map<SpaceDomainType, std::string> const string_by_domain_type = {
    { SpaceDomainType::omnipresent, "omnipresent" },
    { SpaceDomainType::located, "located" }
};

static std::map<std::string, SpaceDomainType> const domain_type_by_string = {
    { "omnipresent", SpaceDomainType::omnipresent },
    { "located", SpaceDomainType::located }
};


static std::map<SpaceDomainItemType, std::string> const
        string_by_domain_item_type = {
    { SpaceDomainItemType::none, "none" },
    { SpaceDomainItemType::point, "point" },
    { SpaceDomainItemType::box, "box" },
    { SpaceDomainItemType::line, "line" },
    { SpaceDomainItemType::region, "region" },
    { SpaceDomainItemType::cell, "cell" }
};

static std::map<std::string, SpaceDomainItemType> const
        domain_item_type_by_string = {
    { "none", SpaceDomainItemType::none },
    { "point", SpaceDomainItemType::point },
    { "box", SpaceDomainItemType::box },
    { "line", SpaceDomainItemType::line },
    { "region", SpaceDomainItemType::region },
    { "cell", SpaceDomainItemType::cell }
};

}  // Anonymous namespace


/*!
    @ingroup    lue_cxx_api_group
*/
std::string space_domain_type_to_string(
    SpaceDomainType const type)
{
    auto const it = string_by_domain_type.find(type);
    assert(it != string_by_domain_type.end());

    return (*it).second;
}


/*!
    @ingroup    lue_cxx_api_group
*/
SpaceDomainType parse_space_domain_type(
    std::string const& string)
{
    auto const it = domain_type_by_string.find(string);

    if(it == domain_type_by_string.end()) {
        throw std::runtime_error("Unknown space domain type: " + string);
    }

    return (*it).second;
}


/*!
    @ingroup    lue_cxx_api_group
*/
std::string space_domain_item_type_to_string(
    SpaceDomainItemType const type)
{
    auto const it = string_by_domain_item_type.find(type);
    assert(it != string_by_domain_item_type.end());

    return (*it).second;
}


/*!
    @ingroup    lue_cxx_api_group
*/
SpaceDomainItemType parse_space_domain_item_type(
    std::string const& string)
{
    auto const it = domain_item_type_by_string.find(string);

    if(it == domain_item_type_by_string.end()) {
        throw std::runtime_error("Unknown space domain item type: " + string);
    }

    return (*it).second;
}


SpaceDomainConfiguration::SpaceDomainConfiguration()

    : _type{SpaceDomainType::omnipresent},
      _item_type{SpaceDomainItemType::none}

{
}


SpaceDomainConfiguration::SpaceDomainConfiguration(
     SpaceDomainItemType const item_type)

    : _type{SpaceDomainType::located},
      _item_type{item_type}

{
    if(_item_type == SpaceDomainItemType::none) {
        throw std::runtime_error("Domain item type must be set");
    }
}


SpaceDomainConfiguration::SpaceDomainConfiguration(
    SpaceDomainType const type,
    SpaceDomainItemType const item_type)

    : _type{type},
      _item_type{item_type}

{
    if(_type == SpaceDomainType::omnipresent) {
        // Whatever the user passed in...
        _item_type = SpaceDomainItemType::none;
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

}  // namespace lue
