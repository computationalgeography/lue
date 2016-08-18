#pragma once
#include "lue/cxx_api/space_domain_configuration.h"
#include "lue/cxx_api/time_domain_configuration.h"


namespace lue {

/*!
    @ingroup    lue_cxx_api_group
*/
class DomainConfiguration
{

public:

                   DomainConfiguration ();

                   DomainConfiguration (TimeDomainConfiguration const& time);

                   DomainConfiguration (SpaceDomainConfiguration const& space);

                   DomainConfiguration (TimeDomainConfiguration const& time,
                                        SpaceDomainConfiguration const& space);

                   ~DomainConfiguration()=default;

    TimeDomainConfiguration const&
                   time                () const;

    SpaceDomainConfiguration const&
                   space               () const;

private:

    TimeDomainConfiguration _time;

    SpaceDomainConfiguration _space;

};

}  // namespace lue
