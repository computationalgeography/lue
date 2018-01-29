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

                   SpaceDomain         (hdf5::Group const& parent);

                   SpaceDomain         (constant_collection::SpaceDomain&& domain);

                   SpaceDomain         (SpaceDomain const&)=delete;

                   SpaceDomain         (SpaceDomain&&)=default;

                   ~SpaceDomain        ()=default;

    SpaceDomain&   operator=           (SpaceDomain const&)=delete;

    SpaceDomain&   operator=           (SpaceDomain&&)=default;

};


SpaceDomain        create_space_domain (hdf5::Group const& parent,
                                        SpaceConfiguration const&
                                            configuration);

}  // namespace located
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
