#pragma once
#include "lue/space_domain.hpp"


namespace lue {
namespace constant_size {

class SpaceDomain:
    public lue::SpaceDomain
{

public:

                   SpaceDomain         (hdf5::Group const& group);

                   SpaceDomain         (lue::SpaceDomain&& domain);

                   SpaceDomain         (SpaceDomain const& other)=delete;

                   SpaceDomain         (SpaceDomain&& other)=default;

                   ~SpaceDomain        ()=default;

    SpaceDomain&   operator=           (SpaceDomain const& other)=delete;

    SpaceDomain&   operator=           (SpaceDomain&& other)=default;

};


SpaceDomain        create_space_domain (hdf5::Group const& group,
                                        SpaceDomain::Configuration const&
                                            configuration);

}  // namespace constant_size
}  // namespace lue
