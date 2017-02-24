#pragma once
#include "lue/cxx_api/domain_configuration.h"
#include "lue/cxx_api/space_domain.h"
#include "lue/cxx_api/time_domain.h"
#include <memory>


namespace lue {

bool               domain_exists       (hdf5::Identifier const& location);


/*!
    @ingroup    lue_cxx_api_group
*/
class Domain:
    public hdf5::Group
{

public:

                   Domain              (hdf5::Identifier const& location);

                   Domain              (hdf5::Identifier&& location);

                   Domain              (Domain const& other)=delete;

                   Domain              (Domain&& other)=default;

                   ~Domain             ()=default;

    Domain&        operator=           (Domain const& other)=delete;

    Domain&        operator=           (Domain&& other)=default;

    TimeDomain&    time_domain         () const;

    SpaceDomain&   space_domain        () const;

    DomainConfiguration const&
                   configuration       () const;

private:

    std::unique_ptr<TimeDomain> _time_domain;

    std::unique_ptr<SpaceDomain> _space_domain;

    DomainConfiguration _configuration;

};


Domain             create_domain       (hdf5::Identifier const& location,
                                        DomainConfiguration const&
                                            configuration);

} // namespace lue
