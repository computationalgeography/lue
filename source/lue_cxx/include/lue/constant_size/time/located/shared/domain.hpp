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

                   Domain              (hdf5::Group const& parent);

                   Domain              (located::Domain&& domain);

                   Domain              (Domain const&)=delete;

                   Domain              (Domain&&)=default;

                   ~Domain             ()=default;

    Domain&        operator=           (Domain const&)=delete;

    Domain&        operator=           (Domain&&)=default;

private:

};

}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue
