#pragma once
#include "lue/domain.hpp"


namespace lue {
namespace constant_size {

class Domain:
    public lue::Domain
{

public:

                   Domain              (hdf5::Group const& parent);

                   Domain              (lue::Domain&& group);

                   Domain              (Domain const&)=delete;

                   Domain              (Domain&&)=default;

                   ~Domain             ()=default;

    Domain&        operator=           (Domain const&)=delete;

    Domain&        operator=           (Domain&&)=default;

};


// Domain             create_domain       (hdf5::Identifier const& location,
//                                         Domain::Configuration const&
//                                             configuration);

} // namespace constant_size
} // namespace lue
