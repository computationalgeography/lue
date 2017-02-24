#pragma once
#include "lue/domain.h"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

class Domain:
    public lue::Domain
{

public:

                   Domain         (hdf5::Identifier const& location);

                   Domain         (lue::Domain&& domain);

                   Domain         (Domain const& other)=delete;

                   Domain         (Domain&& other)=default;

                   ~Domain        ()=default;

    Domain&        operator=      (Domain const& other)=delete;

    Domain&        operator=      (Domain&& other)=default;

private:

};


Domain             create_domain  (hdf5::Identifier const& location);

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
