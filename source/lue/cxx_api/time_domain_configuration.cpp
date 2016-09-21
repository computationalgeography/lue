#include "lue/cxx_api/time_domain_configuration.h"
#include <cassert>
#include <map>


namespace lue {
namespace {

static std::map<TimeDomainType, std::string> const string_by_domain_type = {
    { TimeDomainType::omnipresent, "omnipresent" },
    { TimeDomainType::shared_constant_collection,
        "shared_constant_collection" },
    { TimeDomainType::shared_variable_collection,
        "shared_variable_collection" }
};

static std::map<std::string, TimeDomainType> const domain_type_by_string = {
    { "omnipresent", TimeDomainType::omnipresent },
    { "shared_constant_collection",
        TimeDomainType::shared_constant_collection },
    { "shared_variable_collection",
        TimeDomainType::shared_variable_collection },
};


static std::map<TimeDomainItemType, std::string> const
        string_by_domain_item_type = {
    { TimeDomainItemType::none, "none" },
    { TimeDomainItemType::point, "point" },
    { TimeDomainItemType::period, "period" },
    { TimeDomainItemType::cell, "cell" }
};

static std::map<std::string, TimeDomainItemType> const
        domain_item_type_by_string = {
    { "none", TimeDomainItemType::none },
    { "point", TimeDomainItemType::point },
    { "period", TimeDomainItemType::period },
    { "cell", TimeDomainItemType::cell }
};

}  // Anonymous namespace


/*!
    @ingroup    lue_cxx_api_group
*/
std::string time_domain_type_to_string(
    TimeDomainType const type)
{
    auto const it = string_by_domain_type.find(type);
    assert(it != string_by_domain_type.end());

    return (*it).second;
}


/*!
    @ingroup    lue_cxx_api_group
*/
TimeDomainType parse_time_domain_type(
    std::string const& string)
{
    auto const it = domain_type_by_string.find(string);

    if(it == domain_type_by_string.end()) {
        throw std::runtime_error("Unknown time domain type: " + string);
    }

    return (*it).second;
}


/*!
    @ingroup    lue_cxx_api_group
*/
std::string time_domain_item_type_to_string(
    TimeDomainItemType const type)
{
    auto const it = string_by_domain_item_type.find(type);
    assert(it != string_by_domain_item_type.end());

    return (*it).second;
}


/*!
    @ingroup    lue_cxx_api_group
*/
TimeDomainItemType parse_time_domain_item_type(
    std::string const& string)
{
    auto const it = domain_item_type_by_string.find(string);

    if(it == domain_item_type_by_string.end()) {
        throw std::runtime_error("Unknown time domain item type: " + string);
    }

    return (*it).second;
}


TimeDomainConfiguration::TimeDomainConfiguration()

    : _type{TimeDomainType::omnipresent},
      _item_type{TimeDomainItemType::none}

{
}


TimeDomainConfiguration::TimeDomainConfiguration(
    TimeDomainType const type,
    TimeDomainItemType const item_type)

    : _type{type},
      _item_type{item_type}

{
    if(_type == TimeDomainType::omnipresent) {
        // Whatever the user passed in...
        _item_type = TimeDomainItemType::none;
    }
    else if(_item_type == TimeDomainItemType::none) {
        throw std::runtime_error("Domain item type must be set");
    }
}


TimeDomainType TimeDomainConfiguration::type() const
{
    return _type;
}


TimeDomainItemType TimeDomainConfiguration::item_type() const
{
    return _item_type;
}

}  // namespace lue
