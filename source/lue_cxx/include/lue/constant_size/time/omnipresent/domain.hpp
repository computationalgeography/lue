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

                   Domain         (hdf5::Identifier const& location);

                   Domain         (constant_size::Domain&& domain);

                   Domain         (Domain const& other)=delete;

                   Domain         (Domain&& other)=default;

                   ~Domain        ()=default;

    Domain&        operator=      (Domain const& other)=delete;

    Domain&        operator=      (Domain&& other)=default;

private:

};

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue
