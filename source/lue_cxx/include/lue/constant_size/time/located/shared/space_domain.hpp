#pragma once
#include "lue/constant_size/space_domain.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {

class SpaceDomain:
    public constant_size::SpaceDomain
{

public:

                   SpaceDomain         (hdf5::Group const& group);

                   SpaceDomain         (constant_size::SpaceDomain&& domain);

                   SpaceDomain         (SpaceDomain const& other)=delete;

                   SpaceDomain         (SpaceDomain&& other)=default;

                   ~SpaceDomain        ()=default;

    SpaceDomain&   operator=           (SpaceDomain const& other)=delete;

    SpaceDomain&   operator=           (SpaceDomain&& other)=default;

};


SpaceDomain        create_space_domain (hdf5::Group const& group,
                                        SpaceDomain::Configuration const&
                                            configuration);

}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
