#pragma once
#include "lue/constant_collection/space_domain.hpp"


namespace lue {
namespace constant_collection {
namespace time {
namespace located {

class SpaceDomain:
    public constant_collection::SpaceDomain
{

public:

                   SpaceDomain         (hdf5::Group const& group);

                   SpaceDomain         (constant_collection::SpaceDomain&& domain);

                   SpaceDomain         (SpaceDomain const& other)=delete;

                   SpaceDomain         (SpaceDomain&& other)=default;

                   ~SpaceDomain        ()=default;

    SpaceDomain&   operator=           (SpaceDomain const& other)=delete;

    SpaceDomain&   operator=           (SpaceDomain&& other)=default;

};


SpaceDomain        create_space_domain (hdf5::Group const& group,
                                        SpaceDomain::Configuration const&
                                            configuration);

}  // namespace located
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
