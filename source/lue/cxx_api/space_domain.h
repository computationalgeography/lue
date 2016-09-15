#pragma once
#include "lue/cxx_api/space_domain_configuration.h"
#include "lue/cxx_api/hdf5/group.h"


namespace lue {

/*!
    @ingroup    lue_cxx_api_group
*/
class SpaceDomain:
    public hdf5::Group
{

public:

                   SpaceDomain         (hdf5::Identifier&& location);

                   SpaceDomain         (SpaceDomain const& other)=delete;

                   SpaceDomain         (SpaceDomain&& other)=default;

                   ~SpaceDomain        ()=default;

    SpaceDomain&   operator=           (SpaceDomain const& other)=delete;

    SpaceDomain&   operator=           (SpaceDomain&& other)=default;

    SpaceDomainConfiguration const&
                   configuration       () const;

private:

    SpaceDomainConfiguration
                   _configuration;

};


SpaceDomain         create_space_domain(hdf5::Identifier const& location,
                                        SpaceDomainConfiguration const&
                                            configuration);

SpaceDomain         open_space_domain  (hdf5::Identifier const& location);

} // namespace lue
