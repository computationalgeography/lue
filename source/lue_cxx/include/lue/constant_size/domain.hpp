#pragma once
#include "lue/domain.hpp"


namespace lue {
namespace constant_size {

class Domain:
    public lue::Domain
{

public:

                   Domain              (hdf5::Identifier const& location);

                   Domain              (lue::Domain&& group);

                   Domain              (Domain const& other)=delete;

                   Domain              (Domain&& other)=default;

                   ~Domain             ()=default;

    Domain&        operator=           (Domain const& other)=delete;

    Domain&        operator=           (Domain&& other)=default;

};


Domain             create_domain       (hdf5::Identifier const& location);

} // namespace constant_size
} // namespace lue
