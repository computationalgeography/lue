#include "lue/cxx_api/time_domain_configuration.h"
#include "lue/cxx_api/enum_string_bimap.h"


namespace lue {
namespace {

detail::EnumStringBimap<TimeDomainType> const time_domain_type_map = {
    { TimeDomainType::omnipresent, "omnipresent" },
    { TimeDomainType::shared, "shared" },
    // { TimeDomainType::unique, "unique" }
};


detail::EnumStringBimap<TimeDomainItemType> const time_domain_item_type_map = {
    { TimeDomainItemType::none, "none" },
    // { TimeDomainItemType::point, "point" },
    { TimeDomainItemType::period, "period" },
    // { TimeDomainItemType::cell, "cell" }
};

}  // Anonymous namespace


/*!
    @ingroup    lue_cxx_api_group
*/
std::string time_domain_type_to_string(
    TimeDomainType const type)
{
    return time_domain_type_map.as_string(type);
}


/*!
    @ingroup    lue_cxx_api_group
*/
TimeDomainType parse_time_domain_type(
    std::string const& string)
{
    if(!time_domain_type_map.contains(string)) {
        throw std::runtime_error("Unknown time domain type: " + string);
    }

    return time_domain_type_map.as_value(string);
}


/*!
    @ingroup    lue_cxx_api_group
*/
std::string time_domain_item_type_to_string(
    TimeDomainItemType const type)
{
    return time_domain_item_type_map.as_string(type);
}


/*!
    @ingroup    lue_cxx_api_group
*/
TimeDomainItemType parse_time_domain_item_type(
    std::string const& string)
{
    if(!time_domain_item_type_map.contains(string)) {
        throw std::runtime_error("Unknown time domain item type: " + string);
    }

    return time_domain_item_type_map.as_value(string);
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
