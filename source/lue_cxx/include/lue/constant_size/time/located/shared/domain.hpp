#pragma once
#include "lue/constant_size/time/located/domain.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {

class Domain:
    public located::Domain
{

public:

                   Domain              (hdf5::Identifier const& location);

                   Domain              (located::Domain&& domain);

                   Domain              (Domain const& other)=delete;

                   Domain              (Domain&& other)=default;

                   ~Domain             ()=default;

    Domain&        operator=           (Domain const& other)=delete;

    Domain&        operator=           (Domain&& other)=default;

private:

};

}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
