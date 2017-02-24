#pragma once
#include "lue/cxx_api/time_domain_configuration.h"
#include "lue/cxx_api/hdf5/group.h"


namespace lue {

/*!
    @ingroup    lue_cxx_api_group
*/
class TimeDomain:
    public hdf5::Group
{

public:

                   TimeDomain          (hdf5::Identifier const& location);

                   TimeDomain          (hdf5::Identifier&& location);

                   TimeDomain          (TimeDomain const& other)=delete;

                   TimeDomain          (TimeDomain&& other)=default;

                   ~TimeDomain         ()=default;

    TimeDomain&    operator=           (TimeDomain const& other)=delete;

    TimeDomain&    operator=           (TimeDomain&& other)=default;

    TimeDomainConfiguration const&
                   configuration       () const;

private:

    TimeDomainConfiguration
                   _configuration;

};


TimeDomain         create_time_domain  (hdf5::Identifier const& location,
                                        TimeDomainConfiguration const&
                                            configuration);

} // namespace lue
