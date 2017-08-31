#pragma once
#include "lue/constant_size/domain.hpp"
#include "lue/constant_size/time/omnipresent/space_domain.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

class Domain:
    public constant_size::Domain
{

public:

                   Domain         (hdf5::Identifier const& location);

                   Domain         (constant_size::Domain&& domain);

                   Domain         (Domain const& other)=delete;

                   Domain         (Domain&& other)=default;

                   ~Domain        ()=default;

    Domain&        operator=      (Domain const& other)=delete;

    Domain&        operator=      (Domain&& other)=default;

    SpaceDomain const& space      () const;

    SpaceDomain&   space          ();

private:

    SpaceDomain    _space;

};


Domain             create_domain  (hdf5::Identifier const& location);

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
