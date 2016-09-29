#pragma once
#include "lue/cxx_api/define.h"
#include <string>


namespace lue {

std::string        time_domain_type_to_string(
                                        TimeDomainType const type);

TimeDomainType     parse_time_domain_type(
                                        std::string const& string);

std::string        time_domain_item_type_to_string(
                                        TimeDomainItemType const type);

TimeDomainItemType parse_time_domain_item_type(
                                        std::string const& string);


/*!
    @ingroup    lue_cxx_api_group
*/
class TimeDomainConfiguration
{

public:

                   TimeDomainConfiguration();

                   TimeDomainConfiguration(
                                        TimeDomainType const type,
                                        TimeDomainItemType const item_type);

                   ~TimeDomainConfiguration()=default;

    TimeDomainType type                () const;

    TimeDomainItemType item_type       () const;

private:

    TimeDomainType _type;

    TimeDomainItemType _item_type;

};

}  // namespace lue
