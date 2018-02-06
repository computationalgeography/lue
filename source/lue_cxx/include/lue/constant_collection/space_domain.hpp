#pragma once
#include "lue/space_domain.hpp"


namespace lue {
namespace constant_collection {

class SpaceDomain:
    public lue::SpaceDomain
{

public:

    explicit       SpaceDomain         (hdf5::Group const& group);

                   SpaceDomain         (lue::SpaceDomain&& domain);

                   SpaceDomain         (SpaceDomain const& other)=delete;

                   SpaceDomain         (SpaceDomain&& other)=default;

                   ~SpaceDomain        ()=default;

    SpaceDomain&   operator=           (SpaceDomain const& other)=delete;

    SpaceDomain&   operator=           (SpaceDomain&& other)=default;

};


SpaceDomain        create_space_domain (hdf5::Group const& group,
                                        SpaceConfiguration const&
                                            configuration);

}  // namespace constant_collection
}  // namespace lue
