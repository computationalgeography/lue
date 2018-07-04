#pragma once
#include "lue/core/configuration.hpp"
#include "lue/hdf5.hpp"


namespace lue {

using SpaceConfiguration = Configuration<
    Mobility,
    SpaceDomainItemType
>;


class SpaceDomain:
    public hdf5::Group
{

public:

    explicit       SpaceDomain         (hdf5::Group& parent);

    explicit       SpaceDomain         (hdf5::Group&& group);

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


SpaceDomain        create_space_domain (hdf5::Group& parent,
                                        SpaceConfiguration const&
                                            configuration);

bool               space_domain_exists (hdf5::Group const& parent);

}  // namespace lue
