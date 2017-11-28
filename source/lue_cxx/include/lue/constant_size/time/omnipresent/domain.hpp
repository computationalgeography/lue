#pragma once
#include "lue/constant_size/domain.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

class Domain:
    public constant_size::Domain
{

public:

                   // Domain         (hdf5::Group const& parent);

                   Domain         (constant_size::Domain&& domain);

                   Domain         (Domain const&)=delete;

                   Domain         (Domain&&)=default;

                   ~Domain        ()=default;

    Domain&        operator=      (Domain const&)=delete;

    Domain&        operator=      (Domain&&)=default;

private:

};

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
