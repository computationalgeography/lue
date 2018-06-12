#pragma once
#include "lue/space/space_configuration.hpp"
#include "lue/hdf5/group.hpp"


namespace lue {

/*!
    @sa         create_space_domain(hdf5::Group const&,
                SpaceDomain::Configuration const&),
                space_domain_exists(hdf5::Group const&)
*/
class SpaceDomain:
    public hdf5::Group
{

public:

    explicit       SpaceDomain         (hdf5::Group const& parent);

                   SpaceDomain         (hdf5::Group&& group);

                   SpaceDomain         (SpaceDomain const&)=delete;

                   SpaceDomain         (SpaceDomain&&)=default;

                   ~SpaceDomain        ()=default;

    SpaceDomain&   operator=           (SpaceDomain const&)=delete;

    SpaceDomain&   operator=           (SpaceDomain&&)=default;

    SpaceConfiguration const&
                   configuration       () const;

private:

    SpaceConfiguration _configuration;

};


SpaceDomain        create_space_domain (hdf5::Group const& parent,
                                        SpaceConfiguration const&
                                            configuration);

bool               space_domain_exists (hdf5::Group const& parent);

}  // namespace lue
