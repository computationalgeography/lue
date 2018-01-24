#pragma once
#include "lue/constant_collection/domain.hpp"


namespace lue {
namespace constant_collection {
namespace time {
namespace omnipresent {

class Domain:
    public constant_collection::Domain
{

public:

                   // Domain         (hdf5::Group const& parent);

                   Domain         (constant_collection::Domain&& domain);

                   Domain         (Domain const&)=delete;

                   Domain         (Domain&&)=default;

                   ~Domain        ()=default;

    Domain&        operator=      (Domain const&)=delete;

    Domain&        operator=      (Domain&&)=default;

private:

};

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_collection
}  // namespace lue
