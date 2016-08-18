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


TimeDomainConfiguration::TimeDomainConfiguration()

    : _type{TimeDomainType::omnipresent}

{
}


TimeDomainConfiguration::TimeDomainConfiguration(
    TimeDomainType const& type)

    : _type{type}

{
}


TimeDomainType TimeDomainConfiguration::type() const
{
    return _type;
}

}  // namespace lue
