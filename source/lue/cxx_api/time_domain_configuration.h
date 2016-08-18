#pragma once
#include "lue/cxx_api/define.h"
#include <string>


namespace lue {

std::string        time_domain_type_to_string(
                                        TimeDomainType const type);

TimeDomainType     parse_time_domain_type(
                                        std::string const& string);


/*!
    @ingroup    lue_cxx_api_group
*/
class TimeDomainConfiguration
{

public:

                   TimeDomainConfiguration();

                   TimeDomainConfiguration(
                                        TimeDomainType const& type);

                   ~TimeDomainConfiguration()=default;

    TimeDomainType type                () const;

private:

    TimeDomainType _type;

};

}  // namespace lue
